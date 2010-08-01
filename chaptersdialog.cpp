#include <QtGui>

#include "chaptersdialog.h"
#include "book.h"

ChaptersDialog::ChaptersDialog(Book *b, QWidget *parent):
    QMainWindow(parent), book(b)
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
    foreach (QString id, book->toc) {
        QString contentTitle = book->content[id].name;
        (void)new QListWidgetItem(contentTitle, list);
    }
    horizontalLayout->addWidget(list);
    connect(list, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(onItemActivated(QListWidgetItem*)));

#ifndef Q_WS_MAEMO_5
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical);
    QPushButton *closeButton = buttonBox->addButton(QDialogButtonBox::Close);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
    horizontalLayout->addWidget(buttonBox);
#endif // Q_WS_MAEMO_5
}

void ChaptersDialog::onItemActivated(QListWidgetItem *item)
{
    emit goToChapter(list->row(item));
    close();
}

void ChaptersDialog::onClose()
{
    close();
}
