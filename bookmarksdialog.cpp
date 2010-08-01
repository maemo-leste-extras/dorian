#include <QtGui>

#include "bookmarksdialog.h"
#include "book.h"
#include "bookmarkinfodialog.h"

BookmarksDialog::BookmarksDialog(Book *book_, QWidget *parent):
    QMainWindow(parent), book(book_)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif
    setWindowTitle(tr("Bookmarks"));

    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(frame);
    frame->setLayout(horizontalLayout);

    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    foreach (Book::Bookmark bookmark, book_->bookmarks()) {
        QString contentId = book_->toc[bookmark.chapter];
        QString contentTitle = book_->content[contentId].name;
        (void)new QListWidgetItem(QIcon(":icons/bookmark.png"), contentTitle +
            "\nAt " + QString::number((int)(bookmark.pos*100)) + "%", list);
    }

    horizontalLayout->addWidget(list);

#ifndef Q_WS_MAEMO_5
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical);

    QPushButton *goButton = new QPushButton(tr("Go to"), this);
    buttonBox->addButton(goButton, QDialogButtonBox::ActionRole);
    connect(goButton, SIGNAL(clicked()), this, SLOT(onGo()));

    QPushButton *closeButton = buttonBox->addButton(QDialogButtonBox::Close);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onClose()));

    QPushButton *addButton = new QPushButton(tr("Add"), this);
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAdd()));

    QPushButton *deleteButton = new QPushButton(tr("Delete"), this);
    buttonBox->addButton(deleteButton, QDialogButtonBox::DestructiveRole);
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDelete()));

    horizontalLayout->addWidget(buttonBox);
#else
    QAction *addBookmarkAction = menuBar()->addAction(tr("Add bookmark"));
    connect(addBookmarkAction, SIGNAL(triggered()), this, SLOT(onAdd()));
#endif // Q_WS_MAEMO_5
    connect(list, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(onItemActivated(QListWidgetItem *)));
}

void BookmarksDialog::onGo()
{
    if (!list->selectedItems().isEmpty()) {
        QListWidgetItem *item = list->selectedItems()[0];
        emit goToBookmark(list->row(item));
        close();
    }
}

void BookmarksDialog::onItemActivated(QListWidgetItem *item)
{
    switch ((new BookmarkInfoDialog(book, list->row(item), this))->exec()) {
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

void BookmarksDialog::onClose()
{
    close();
}

void BookmarksDialog::onDelete(bool really)
{
    if (list->selectedItems().isEmpty()) {
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
    QListWidgetItem *item = list->selectedItems()[0];
    int row = list->row(item);
    book->deleteBookmark(row);
    delete item;
}

void BookmarksDialog::closeEvent(QCloseEvent *event)
{
#ifdef Q_WS_MAEMO_5
    menuBar()->clear();
#endif
    event->accept();
}
