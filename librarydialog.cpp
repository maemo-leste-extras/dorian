#include <QDir>
#include <QtGui>

#include "librarydialog.h"
#include "library.h"
#include "sortedlibrary.h"
#include "book.h"
#include "infodialog.h"
#include "settings.h"
#include "listwindow.h"
#include "trace.h"
#include "bookfinder.h"
#include "searchdialog.h"
#include "platform.h"
#include "searchresultsdialog.h"
#include "progressdialog.h"
#include "settings.h"

LibraryDialog::LibraryDialog(QWidget *parent):
        ListWindow(tr("(No books)"), parent)
{
    TRACE;
    setWindowTitle(tr("Library"));
    setAttribute(Qt::WA_DeleteOnClose, true);

    // Add menu actions
    sortByTitle =
            addMenuAction(tr("Sort by title"), this, SLOT(onSortByTitle()));
    sortByAuthor =
            addMenuAction(tr("Sort by author"), this, SLOT(onSortByAuthor()));

    // Set model
    sortedLibrary = new SortedLibrary(this);
    setModel(sortedLibrary);

    // Add action buttons
    addButton(tr("Add book"), this, SLOT(onAdd()), "add");
    addButton(tr("Add books from folder"), this,
              SLOT(onAddFolder()), "folder");
    addButton(tr("Search the Web"), this, SLOT(onSearch()), "search");
    addItemButton(tr("Delete"), this, SLOT(onDelete()), "delete");

    // Set selected item
    Library *library = Library::instance();
    QModelIndex current = library->nowReading();
    setCurrentItem(sortedLibrary->mapFromSource(current));

    // Search dialog box
    searchDialog = new SearchDialog(this);
    connect(Search::instance(), SIGNAL(endSearch()),
            this, SLOT(showSearchResults()));

    // Progress bar
    progress = new ProgressDialog(tr("Adding books"), this);

    connect(library, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(onBookAdded()));
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));

    // Retrieve default sort criteria
    switch (Settings::instance()->value("lib/sortby").toInt()) {
    case SortedLibrary::SortByAuthor:
        onSortByAuthor();
        break;
    default:
        onSortByTitle();
    }
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
    QString path = QFileDialog::getOpenFileName(this, tr("Add book"),
                                                lastDir, "Books (*.epub)");
    if (path == "") {
        return;
    }

    // Save directory selected
    Settings::instance()->setValue("lastdir", QFileInfo(path).absolutePath());

    // Add book to library
    QModelIndex index = library->find(path);
    if (index.isValid()) {
        Platform::instance()->information(
                tr("This book is already in the library"), this);
        setSelected(sortedLibrary->mapFromSource(index));
    }
    else {
        library->add(path);
    }
}

void LibraryDialog::onBookAdded()
{
    Library *library = Library::instance();
    setSelected(sortedLibrary->
                mapFromSource(library->index(library->rowCount() - 1)));
}

void LibraryDialog::onItemActivated(const QModelIndex &index)
{
    TRACE;

    QModelIndex libraryIndex = sortedLibrary->mapToSource(index);
    Book *book = Library::instance()->book(libraryIndex);
    int ret = (new InfoDialog(book, this))->exec();

    switch (ret) {
    case InfoDialog::Read:
        Library::instance()->setNowReading(libraryIndex);
        close();
        break;
    case InfoDialog::Delete:
        Library::instance()->remove(libraryIndex);
        break;
    default:
        ;
    }
}

void LibraryDialog::onDelete()
{
    QModelIndex current = currentItem();
    if (!current.isValid()) {
        return;
    }
    QModelIndex libraryIndex = sortedLibrary->mapToSource(current);
    Book *book = Library::instance()->book(libraryIndex);
    if (QMessageBox::Yes !=
        QMessageBox::question(this, tr("Delete book"),
            tr("Delete book \"%1\"?").arg(book->shortName()),
            QMessageBox::Yes | QMessageBox::No)) {
        return;
    }
    Library::instance()->remove(libraryIndex);
}

QString LibraryDialog::createItemText(Book *book)
{
    Q_ASSERT(book);
    QString text = book->shortName() + "\n";
    if (book->creators.size()) {
        text += book->creators[0];
        for (int i = 1; i < book->creators.size(); i++) {
            text += ", " + book->creators[i];
        }
    }
    return text;
}

void LibraryDialog::setSelected(const QModelIndex &libraryIndex)
{
    QModelIndex sortedIndex = sortedLibrary->mapFromSource(libraryIndex);
    if (sortedIndex.isValid()) {
        setCurrentItem(sortedIndex);
    }
}

void LibraryDialog::onAddFolder()
{
    TRACE;

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
    case 1: msg = tr("One book added"); break;
    default: msg = tr("%1 books added").arg(added);
    }

    progress->reset();
    qDebug() << "LibraryDialog::onRefreshDone:" << msg;
    Platform::instance()->information(msg, this);
}

void LibraryDialog::onAddFromFolder(const QString &path)
{
    progress->setLabelText(QFileInfo(path).fileName());
    progress->setValue(progress->value() + 1);
}

void LibraryDialog::onSearch()
{
    TRACE;
    int ret = searchDialog->exec();
    qDebug() << "Search dialog returned" << ret;
    if (ret != QDialog::Accepted) {
        return;
    }
    progress->setLabelText(tr("Searching Project Gutenberg"));
    progress->showWait();
    Search::instance()->start(searchDialog->query());
}

void LibraryDialog::showSearchResults()
{
    progress->reset();
    QList<Search::Result> results = Search::instance()->results();
    if (results.isEmpty()) {
        QMessageBox::information(this, tr("Search results"),
                                 tr("No books found"));
        return;
    }

    SearchResultsDialog *dialog = new SearchResultsDialog(results, this);
    connect(dialog, SIGNAL(add(const Search::Result &)),
            this, SLOT(onAddSearchResult(const Search::Result &)));
    dialog->show();
}

void LibraryDialog::onSortByAuthor()
{
    TRACE;
    sortedLibrary->setSortBy(SortedLibrary::SortByAuthor);
    Settings::instance()->setValue("lib/sortby", SortedLibrary::SortByAuthor);
    sortByAuthor->setChecked(true);
    sortByTitle->setChecked(false);
}

void LibraryDialog::onSortByTitle()
{
    TRACE;
    sortedLibrary->setSortBy(SortedLibrary::SortByTitle);
    Settings::instance()->setValue("lib/sortby", SortedLibrary::SortByTitle);
    sortByAuthor->setChecked(false);
    sortByTitle->setChecked(true);
}
