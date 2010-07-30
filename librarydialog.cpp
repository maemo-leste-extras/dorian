#include <QtGui>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QModelIndex>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "librarydialog.h"
#include "library.h"
#include "sortedlibrary.h"
#include "book.h"
#include "infodialog.h"
#include "settings.h"

LibraryDialog::LibraryDialog(QWidget *parent): QMainWindow(parent)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif
    setWindowTitle(tr("Library"));

    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(frame);
    frame->setLayout(horizontalLayout);

    list = new QListView(this);
    sortedLibrary = new SortedLibrary(this);
    list->setModel(sortedLibrary);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setSpacing(1);
    list->setUniformItemSizes(true);

    Library *library = Library::instance();
    QModelIndex current = library->nowReading();
    setSelected(current);
    horizontalLayout->addWidget(list);

#ifndef Q_WS_MAEMO_5
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical);
    detailsButton = new QPushButton(tr("Details"), this);
    readButton = new QPushButton(tr("Read"), this);
    removeButton = new QPushButton(tr("Delete"), this);
    addButton = new QPushButton(tr("Add"), this);

    buttonBox->addButton(detailsButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(readButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(removeButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    horizontalLayout->addWidget(buttonBox);
#else
    QAction *addBookAction = menuBar()->addAction(tr("Add book"));
#endif // Q_WS_MAEMO_5

    connect(Library::instance(), SIGNAL(nowReadingChanged()),
            this, SLOT(onCurrentBookChanged()));
    connect(Library::instance(),
            SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this,
            SLOT(onBookAdded()));
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
#ifndef Q_WS_MAEMO_5
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(onDetails()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onRead()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemove()));
    connect(list->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection &,
                                    const QItemSelection &)),
            this, SLOT(onItemSelectionChanged()));
    onItemSelectionChanged();
#else
    connect(addBookAction, SIGNAL(triggered()), this, SLOT(onAdd()));
#endif // !Q_WS_MAEMO_5
}

void LibraryDialog::onAdd()
{
    Library *library = Library::instance();

    // Figure out directory to show
    QString lastDir = Settings::instance()->value("lastdir").toString();
    if (lastDir == "") {
        lastDir = QDir::homePath();
    }

    // Get book file name
    QString path = QFileDialog::getOpenFileName(this, tr("Add Book"),
                                                lastDir, "Books (*.epub)");
    if (path == "") {
        return;
    }

    // Save directory selected
    Settings::instance()->setValue("lastdir", QFileInfo(path).absolutePath());

    // Add book to library
    QModelIndex index = library->find(path);
    if (index.isValid()) {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
            tr("This book is already in the library"),
            QMaemo5InformationBox::DefaultTimeout);
#else
        (void)QMessageBox::information(this, tr("Dorian"),
            tr("This book is already in the library"), QMessageBox::Ok);
#endif // Q_WS_MAEMO_5
        setSelected(index);
    }
    else {
        library->add(path);
    }
}

void LibraryDialog::onBookAdded()
{
    Library *library = Library::instance();
    setSelected(library->index(library->rowCount() - 1));
}

#ifndef Q_WS_MAEMO_5

void LibraryDialog::onRemove()
{
    QModelIndex current = sortedLibrary->mapToSource(list->currentIndex());
    if (current.isValid()) {
        Book *currentBook = Library::instance()->book(current);
        QString title = currentBook->name();
        if (QMessageBox::Yes ==
            QMessageBox::question(this, "Delete book",
                                  "Delete book \"" + title + "\"?",
                                  QMessageBox::Yes | QMessageBox::No)) {
            Library::instance()->remove(current);
        }
    }
}

void LibraryDialog::onRead()
{
    qDebug() << "LibraryDialog::onRead";
    QModelIndex current = sortedLibrary->mapToSource(list->currentIndex());
    if (current.isValid()) {
        Library::instance()->setNowReading(current);
    }
}

void LibraryDialog::onDetails()
{
    onItemActivated(list->currentIndex());
}

#endif // Q_WS_MAEMO_5

void LibraryDialog::onItemActivated(const QModelIndex &index)
{
    qDebug() << "LibraryDialog::onItemActivated";
    QModelIndex libraryIndex = sortedLibrary->mapToSource(index);
    Book *book = Library::instance()->book(libraryIndex);
    (new InfoDialog(book, this))->exec();
}

QString LibraryDialog::createItemText(const Book *book)
{
    QString text = book->title + "\n";
    if (book->creators.size()) {
        text += book->creators[0];
        for (int i = 1; i < book->creators.size(); i++) {
            text += ", " + book->creators[i];
        }
    }
    return text;
}

#ifndef Q_WS_MAEMO_5

void LibraryDialog::onItemSelectionChanged()
{
    bool enable = selected().isValid();
    readButton->setEnabled(enable);
    detailsButton->setEnabled(enable);
    removeButton->setEnabled(enable);
}

#endif // Q_WS_MAEMO_5

void LibraryDialog::onCurrentBookChanged()
{
    close();
}

void LibraryDialog::setSelected(const QModelIndex &libraryIndex)
{
    QModelIndex sortedIndex = sortedLibrary->mapFromSource(libraryIndex);
    list->selectionModel()->clearSelection();
    if (sortedIndex.isValid()) {
        list->selectionModel()->select(sortedIndex,
                                       QItemSelectionModel::Select);
        list->setCurrentIndex(sortedIndex);
    }
}

QModelIndex LibraryDialog::selected() const
{
    QModelIndexList selectedItems = list->selectionModel()->selectedIndexes();
    if (selectedItems.size()) {
        return sortedLibrary->mapToSource(selectedItems[0]);
    }
    return QModelIndex();
}

void LibraryDialog::closeEvent(QCloseEvent *event)
{
#ifdef Q_WS_MAEMO_5
    menuBar()->clear();
#endif
    event->accept();
}
