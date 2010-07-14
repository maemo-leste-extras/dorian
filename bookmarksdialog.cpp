#include <QtGui>

#include "bookmarksdialog.h"
#include "book.h"

BookmarksDialog::BookmarksDialog(Book *book_, QWidget *parent):
    QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
            Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(tr("Bookmarks"));
#ifndef Q_WS_MAEMO_5
    setSizeGripEnabled(true);
#endif

    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    foreach (Book::Bookmark bookmark, book_->bookmarks()) {
        list->addItem("Chapter " + QString::number(bookmark.chapter) + ", at " +
                      QString::number((int)(bookmark.pos * 100)) + "%");
    }

    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(list);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical);
#ifndef Q_WS_MAEMO_5
    QPushButton *goButton = new QPushButton(tr("Go"), this);
    buttonBox->addButton(goButton, QDialogButtonBox::AcceptRole);
    connect(goButton, SIGNAL(clicked()), this, SLOT(onGo()));
#endif
    QPushButton *addButton = new QPushButton(tr("Add"), this);
    buttonBox->addButton(addButton, QDialogButtonBox::ActionRole);
    connect(addButton, SIGNAL(clicked()), this, SLOT(onAdd()));

    horizontalLayout->addWidget(buttonBox);

#ifdef Q_WS_MAEMO_5
    connect(list, SIGNAL(itemActivated(QListWidgetItem *)),
            this, SLOT(onItemActivated(QListWidgetItem *)));
#endif
}

void BookmarksDialog::onGo()
{
    if (list->selectedItems().isEmpty()) {
        return;
    }
    QListWidgetItem *item = list->selectedItems()[0];
    int index = list->row(item) + 1;
    done(index);
}

void BookmarksDialog::onItemActivated(QListWidgetItem *item)
{
    done(list->row(item) + 1);
}

void BookmarksDialog::onAdd()
{
    done(-1);
}
