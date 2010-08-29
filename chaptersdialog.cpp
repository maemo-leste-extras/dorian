#include <QtGui>

#include "chaptersdialog.h"
#include "book.h"
#include "listview.h"

ChaptersDialog::ChaptersDialog(Book *book, QWidget *parent): ListWindow(parent)
{
    setWindowTitle(tr("Chapters"));

    if (book) {
        foreach (QString id, book->chapters) {
            QString contentTitle = book->content[id].name;
            data.append(contentTitle);
        }
    }
    QStringListModel *model = new QStringListModel(data, this);
    list = new ListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setModel(model);
    list->setUniformItemSizes(true);
    addList(list);
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));

#ifndef Q_WS_MAEMO_5
    addAction(tr("Close"), this, SLOT(close()), QString(),
              QDialogButtonBox::RejectRole);
#endif // Q_WS_MAEMO_5
}

void ChaptersDialog::onItemActivated(const QModelIndex &index)
{
    emit goToChapter(index.row());
    close();
}
