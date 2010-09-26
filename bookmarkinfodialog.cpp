#include <QtGui>

#include "bookmarkinfodialog.h"
#include "book.h"

BookmarkInfoDialog::BookmarkInfoDialog(Book *b, int i, QWidget *parent):
    Dyalog(parent, true),
    book(b),
    index(i)
{
    setWindowTitle(tr("Bookmark Details"));

    Book::Bookmark bookmark = book->bookmarks()[index];
    QString label("At ");
    label += QString::number((int)(100 * book->
        getProgress(bookmark.part, bookmark.pos))) + "%";
    int chapterIndex = book->chapterFromPart(bookmark.part);
    if (chapterIndex != -1) {
        QString chapterId = book->chapters[chapterIndex];
        label += ", in\n\"" + book->content[chapterId].name + "\"";
    }
    QLabel *info = new QLabel(label, this);
    addWidget(info);
    addStretch();
    addButton(tr("Go to"), this, SLOT(onRead()), QDialogButtonBox::ActionRole);
    addButton(tr("Delete"), this, SLOT(onRemove()),
              QDialogButtonBox::DestructiveRole);
}

void BookmarkInfoDialog::onRead()
{
    done(GoTo);
}

void BookmarkInfoDialog::onRemove()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this, tr("Delete bookmark"),
                              tr("Delete bookmark?"),
                              QMessageBox::Yes | QMessageBox::No)) {
        done(Delete);
    }
}
