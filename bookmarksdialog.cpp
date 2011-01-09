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
    foreach (Book::Bookmark bookmark, book_->bookmarks()) {
        data.append(bookmarkToText(bookmark));
    }
    QStringListModel *model = new QStringListModel(data, this);
    setModel(model);

    addButton(tr("Add bookmark"), this, SLOT(onAdd()), "add");
    addItemButton(tr("Go to bookmark"), this, SLOT(onGo()), "goto");
    addItemButton(tr("Edit bookmark"), this, SLOT(onEdit()), "edit");
    addItemButton(tr("Delete bookmark"), this, SLOT(onDelete()), "delete");

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
    TRACE;
#ifdef Q_WS_MAEMO_5
    switch ((new BookmarkInfoDialog(book, index.row(), this))->exec()) {
    case BookmarkInfoDialog::GoTo:
        onGo();
        break;
    case BookmarkInfoDialog::Delete:
        reallyDelete();
        break;
    default:
        ;
    }
#else
    Q_UNUSED(index);
#endif
}

void BookmarksDialog::onAdd()
{
    TRACE;
    bool ok;
    QString text = QInputDialog::getText(this, tr("Add bookmark"),
        tr("Note (optional):"), QLineEdit::Normal, QString(), &ok);
    if (ok) {
        emit addBookmark(text);
        close();
    }
}

void BookmarksDialog::onDelete()
{
    TRACE;
    if (!currentItem().isValid()) {
        return;
    }
    if (QMessageBox::Yes !=
        QMessageBox::question(this, tr("Delete bookmark"),
            tr("Delete bookmark?"), QMessageBox::Yes | QMessageBox::No)) {
        return;
    }
    reallyDelete();
}

void BookmarksDialog::reallyDelete()
{
    TRACE;
    QModelIndex current = currentItem();
    if (!current.isValid()) {
        return;
    }
    int row = current.row();
    model()->removeRow(row);
    book->deleteBookmark(row);
}

void BookmarksDialog::onEdit()
{
    TRACE;
    QModelIndex current = currentItem();
    if (!current.isValid()) {
        return;
    }
    int row = current.row();
    Book::Bookmark b = book->bookmarks()[row];
    bool ok;
    QString text = QInputDialog::getText(this, tr("Edit bookmark"),
        tr("Note:"), QLineEdit::Normal, b.note, &ok);
    if (!ok) {
        return;
    }
    b.note = text;
    book->setBookmarkNote(row, text);
    QStringListModel *m = qobject_cast<QStringListModel *>(model());
    if (m) {
        m->setData(current, bookmarkToText(b), Qt::DisplayRole);
    }
}

QString BookmarksDialog::bookmarkToText(const Book::Bookmark &bookmark)
{
    // FIXME: Localize me
    QString label("At ");
    label += QString::number((int)(100 * book->
        getProgress(bookmark.part, bookmark.pos))) + "%";
    if (!bookmark.note.isEmpty()) {
        label += ": " + bookmark.note;
    }
    label += "\n";
    int chapterIndex = book->chapterFromPart(bookmark.part);
    if (chapterIndex != -1) {
        QString chapterId = book->chapters[chapterIndex];
        label += "In \"" + book->content[chapterId].name + "\"";
    }
    return label;
}
