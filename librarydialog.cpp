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
#include "listwindow.h"
#include "listview.h"
#include "trace.h"
#include "bookfinder.h"

LibraryDialog::LibraryDialog(QWidget *parent): ListWindow(parent)
{
    setWindowTitle(tr("Library"));

    // Add actions

#ifndef Q_WS_MAEMO_5
    addItemAction(tr("Details"), this, SLOT(onDetails()));
    addItemAction(tr("Read"), this, SLOT(onRead()));
    addItemAction(tr("Delete"), this, SLOT(onRemove()));
#endif // ! Q_WS_MAEMO_5

    addAction(tr("Add book"), this, SLOT(onAdd()));
    addAction(tr("Add books from folder"), this, SLOT(onAddFolder()));

    // Create and add list view
    list = new ListView(this);
    sortedLibrary = new SortedLibrary(this);
    list->setModel(sortedLibrary);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setSpacing(1);
    Library *library = Library::instance();
    QModelIndex current = library->nowReading();
    setSelected(current);
    addList(list);

    progress = new QProgressDialog(tr("Adding books"), "", 0, 0, this);
    progress->reset();
    progress->setMinimumDuration(0);
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(0);

    connect(Library::instance(), SIGNAL(nowReadingChanged()),
            this, SLOT(onCurrentBookChanged()));
    connect(Library::instance(),
            SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this,
            SLOT(onBookAdded()));
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
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
            QMessageBox::question(this, tr("Delete book"),
                tr("Delete book \"%1\" from library?").
                    arg(currentBook->shortName()),
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

void LibraryDialog::onAddFolder()
{
    Trace t("LibraryDialog::onAddFolder");

    // Get folder name
    Settings *settings = Settings::instance();
    QString last =
            settings->value("lastfolderadded", QDir::homePath()).toString();
    QString path =
            QFileDialog::getExistingDirectory(this, tr("Select folder"), last);
    if (path == "") {
        return;
    }
    settings->setValue("lastfolderadded", QFileInfo(path).absolutePath());
    qDebug() << path;

    // Add books from folder
    progress->setWindowTitle(tr("Adding books"));
    BookFinder *bookFinder = new BookFinder(this);
    Library *library = Library::instance();
    connect(bookFinder, SIGNAL(begin(int)), progress, SLOT(setMaximum(int)));
    connect(bookFinder, SIGNAL(add(const QString &)),
            this, SLOT(onAddFromFolder(const QString &)));
    connect(bookFinder, SIGNAL(add(const QString &)),
            library, SLOT(add(const QString &)));
    connect(bookFinder, SIGNAL(done(int)),
            this, SLOT(onAddFromFolderDone(int)));
    bookFinder->find(path, Library::instance()->bookPaths());
}

void LibraryDialog::onAddFromFolderDone(int added)
{
    QString msg;

    switch (added) {
    case 0: msg = tr("No new books found"); break;
    case 1: msg = tr("One new book added"); break;
    default: msg = tr("%1 new books added").arg(added);
    }

    progress->reset();
    qDebug() << "LibraryDialog::onRefreshDone:" << msg;
#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::information(this, msg);
#else
    // FIXME
#endif
}

void LibraryDialog::onAddFromFolder(const QString &path)
{
    progress->setLabelText(QFileInfo(path).fileName());
    progress->setValue(progress->value() + 1);
}
