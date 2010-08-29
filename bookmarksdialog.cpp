#include <QtGui>

#include "bookmarksdialog.h"
#include "book.h"
#include "bookmarkinfodialog.h"
#include "listview.h"

BookmarksDialog::BookmarksDialog(Book *book_, QWidget *parent):
    ListWindow(parent), book(book_)
{
    setWindowTitle(tr("Bookmarks"));
    if (!book) {
        return;
    }

    addAction(tr("Add bookmark"), this, SLOT(onAdd()));
#ifndef Q_WS_MAEMO_5
    addItemAction(tr("Go to"), this, SLOT(onGo()));
    addItemAction(tr("Delete"), this, SLOT(onDelete()));
#endif // ! Q_WS_MAEMO_5

    // Build bookmark list
    // FIXME: Localize me
    foreach (Book::Bookmark bookmark, book_->bookmarks()) {
        QString label("At ");
        label += QString::number((int)(100 * book_->
            getProgress(bookmark.part, bookmark.pos))) + "%";
        int chapterIndex = book_->chapterFromPart(bookmark.part);
        if (chapterIndex != -1) {
            QString chapterId = book_->chapters[chapterIndex];
            label += "\nIn \"" + book_->content[chapterId].name + "\"";
        }
        data.append(label);
    }

    // Create bookmark list view
    QStringListModel *model = new QStringListModel(data, this);
    list = new ListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setModel(model);
    addList(list);
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
    addList(list);
}

void BookmarksDialog::onGo()
{
    QModelIndex current = list->currentIndex();
    if (current.isValid()) {
        emit goToBookmark(current.row());
        close();
    }
}

void BookmarksDialog::onItemActivated(const QModelIndex &index)
{
    switch ((new BookmarkInfoDialog(book, index.row(), this))->exec()) {
    case BookmarkInfoDialog::GoTo:
        onGo();
        break;
    case BookmarkInfoDialog::Delete:
        onDelete(true);
        break;
    default:
        ;
    }
}

void BookmarksDialog::onAdd()
{
    emit addBookmark();
    close();
}

void BookmarksDialog::onDelete(bool really)
{
    QModelIndex current = list->currentIndex();
    if (!current.isValid()) {
        return;
    }
    if (!really) {
        if (QMessageBox::Yes !=
            QMessageBox::question(this, tr("Delete bookmark"),
                                  tr("Delete bookmark?"),
                                  QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }
    int row = current.row();
    list->model()->removeRow(row);
    book->deleteBookmark(row);
}
