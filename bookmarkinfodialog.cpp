#include <QtGui>

#include "bookmarkinfodialog.h"
#include "book.h"

BookmarkInfoDialog::BookmarkInfoDialog(Book *b, int i, QWidget *parent):
    Dialog(parent),
    book(b),
    index(i)
{
    setWindowTitle(tr("Bookmark Details"));

    Book::Bookmark bookmark = book->bookmarks()[index];
    QString contentId = book->parts[bookmark.part];
    QString contentTitle = book->content[contentId].name;
    QLabel *info = new QLabel(contentTitle + "\nAt " +
        QString::number((int)(bookmark.pos*100)) + "%", this);
    addWidget(info);
    addStretch();

    QPushButton *read = new QPushButton(tr("Go to"), this);
    QPushButton *remove = new QPushButton(tr("Delete"), this);
    connect(read, SIGNAL(clicked()), this, SLOT(onRead()));
    connect(remove, SIGNAL(clicked()), this, SLOT(onRemove()));
    addButton(read, QDialogButtonBox::ActionRole);
    addButton(remove, QDialogButtonBox::DestructiveRole);
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
