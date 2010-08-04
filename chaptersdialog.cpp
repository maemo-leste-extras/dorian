#include <QtGui>

#include "chaptersdialog.h"
#include "book.h"

ChaptersDialog::ChaptersDialog(Book *book, QWidget *parent): ListWindow(parent)
{
    setWindowTitle(tr("Chapters"));

    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    if (book) {
        foreach (QString id, book->toc) {
            QString contentTitle = book->content[id].name;
            (void)new QListWidgetItem(contentTitle, list);
        }
    }
    addList(list);
    connect(list, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(onItemActivated(QListWidgetItem*)));

#ifndef Q_WS_MAEMO_5
    addAction(tr("Close"), this, SLOT(close()), QDialogButtonBox::RejectRole);
#endif // Q_WS_MAEMO_5
}

void ChaptersDialog::onItemActivated(QListWidgetItem *item)
{
    emit goToChapter(list->row(item));
    close();
}
