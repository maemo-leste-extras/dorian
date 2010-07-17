#include <QtGui>
#include <QDebug>
#include <QFileInfo>
#include <QDir>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "librarydialog.h"
#include "library.h"
#include "book.h"
#include "infodialog.h"

LibraryDialog::LibraryDialog(QWidget *parent):
        QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(tr("Library"));
    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
#ifndef Q_WS_MAEMO_5
    setSizeGripEnabled(true);
#endif

    Library *library = Library::instance();
    for (int i = 0; i < library->size(); i++) {
        Book *book = library->at(i);
        (void)new QListWidgetItem(book->cover, createItemText(book), list);
    }
    Book *current = library->current();
    if (library->size() && current) {
        list->setItemSelected(list->item(library->find(current)), true);
    }

    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(list);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical);
#ifndef Q_WS_MAEMO_5
    detailsButton = new QPushButton(tr("Details"), this);
    readButton = new QPushButton(tr("Read"), this);
    removeButton = new QPushButton(tr("Delete"), this);
#endif
    addButton = new QPushButton(tr("Add"), this);

#ifndef Q_WS_MAEMO_5
    buttonBox->addButton(detailsButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(readButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(removeButton, QDialogButtonBox::ActionRole);
#endif // Q_WS_MAEMO_5
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);

    horizontalLayout->addWidget(buttonBox);

    connect(library, SIGNAL(bookAdded()), this, SLOT(onBookAdded()));
    connect(library, SIGNAL(currentBookChanged()),
            this, SLOT(onCurrentBookChanged()));
#ifndef Q_WS_MAEMO_5
    connect(list, SIGNAL(itemSelectionChanged()),
            this, SLOT(onItemSelectionChanged()));
    connect(detailsButton, SIGNAL(clicked()), this, SLOT(onDetails()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onRead()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(onRemove()));
#endif
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAdd()));
#ifdef Q_WS_MAEMO_5
    connect(list, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(onItemActivated(QListWidgetItem *)));
#endif

#ifndef Q_WS_MAEMO_5
    onItemSelectionChanged();
#endif
}

void LibraryDialog::onAdd()
{
    Library *library = Library::instance();

    if (lastDir == "") {
        if (library->size()) {
            QFileInfo info(library->at(library->size() - 1)->path());
            lastDir = info.absolutePath();
        }
    }
    if (lastDir == "") {
        lastDir = QDir::homePath();
    }
    QString path = QFileDialog::getOpenFileName(this, tr("Add Book"),
                                                lastDir, "Books (*.epub)");
    qDebug() << "LibraryDialog::onAdd" << path;
    if (path == "") {
        return;
    }

    lastDir = QFileInfo(path).absolutePath();
    if (library->find(path) != -1) {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
            tr("Book is alreadButtony in the library"),
            QMaemo5InformationBox::DefaultTimeout);
#endif
        // FIXME: Select existing book
    }
    else {
        library->add(path);
    }
}

void LibraryDialog::onBookAdded()
{
    Library *library = Library::instance();
    int index = library->size() - 1;
    Book *book = library->at(index);
    QListWidgetItem *item = new QListWidgetItem(book->cover,
                                                createItemText(book));
    list->addItem(item);
    list->setCurrentItem(item);
}

#ifndef Q_WS_MAEMO_5

void LibraryDialog::onRemove()
{
    qDebug() << "LibraryDialog::onRemove";
    if (list->selectedItems().isEmpty()) {
        return;
    }
    QListWidgetItem *item = list->selectedItems()[0];
    int row = list->row(item);
    QString title = Library::instance()->at(row)->title;
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Delete book",
                              "Delete book \"" + title + "\"?", QMessageBox::Yes
#ifndef Q_WS_MAEMO_5
                              , QMessageBox::No
#endif
                              )) {
        list->takeItem(row);
        Library::instance()->remove(row);
    }
}

void LibraryDialog::onRead()
{
    qDebug() << "LibraryDialog::onRead";
    if (list->selectedItems().isEmpty()) {
        return;
    }
    QListWidgetItem *item = list->selectedItems()[0];
    int row = list->row(item);
    Library::instance()->setCurrent(row);
}

void LibraryDialog::onDetails()
{
    onItemActivated(list->selectedItems()[0]);
}

#endif // Q_WS_MAEMO_5

void LibraryDialog::onItemActivated(QListWidgetItem *item)
{
    qDebug() << "LibraryDialog::onItemActivated";
    int row = list->row(item);
    Book *book = Library::instance()->at(row);
    InfoDialog *info = new InfoDialog(book, this);
    info->exec();
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
    bool enable = list->selectedItems().size();
    qDebug() << "LibraryDialog::onItemSelectionChanged" << enable;
    readButton->setEnabled(enable);
    qDebug() << " readButton";
    detailsButton->setEnabled(enable);
    qDebug() << " detailsButton";
    removeButton->setEnabled(enable);
    qDebug() << " removeButton";
}

#endif // Q_WS_MAEMO_5

void LibraryDialog::onCurrentBookChanged()
{
    close();
}
