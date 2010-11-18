#include <QtGui>

#include "bookmarksdialog.h"
#include "book.h"
#include "bookmarkinfodialog.h"
#include "trace.h"

BookmarksDialog::BookmarksDialog(Book *book_, QWidget *parent):
    ListWindow(tr("(No bookmarks)\n"), parent), book(book_)
{
    setWindowTitle(tr("Bookmarks"));
    if (!book) {
        return;
    }

    // Build and set bookmark model
    // FIXME: Localize me
    foreach (Book::Bookmark bookmark, book_->bookmarks()) {
        QString label("At ");
        label += QString::number((int)(100 * book_->
            getProgress(bookmark.part, bookmark.pos))) + "%";
        if (!bookmark.note.isEmpty()) {
            label += ": " + bookmark.note;
        }
        label += "\n";
        int chapterIndex = book_->chapterFromPart(bookmark.part);
        if (chapterIndex != -1) {
            QString chapterId = book_->chapters[chapterIndex];
            label += "In \"" + book_->content[chapterId].name + "\"";
        }
        data.append(label);
    }
    QStringListModel *model = new QStringListModel(data, this);
    setModel(model);

    addButton(tr("Add bookmark"), this, SLOT(onAdd()), "add");

    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
}

void BookmarksDialog::onGo()
{
    TRACE;
    QModelIndex current = currentItem();
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
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add bookmark"),
        tr("Note (optional):"), QLineEdit::Normal, QString(), &ok);
    if (ok) {
        emit addBookmark(text);
        close();
    }
}

void BookmarksDialog::onDelete(bool really)
{
    QModelIndex current = currentItem();
    if (!current.isValid()) {
        return;
    }
    if (!really) {
        if (QMessageBox::Yes !=
            QMessageBox::question(this, tr("Delete bookmark"),
                tr("Delete bookmark?"), QMessageBox::Yes | QMessageBox::No)) {
            return;
        }
    }
    int row = current.row();
    model()->removeRow(row);
    book->deleteBookmark(row);
}
