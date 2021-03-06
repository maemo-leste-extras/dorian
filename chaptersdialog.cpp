#include <QStringListModel>

#include "chaptersdialog.h"
#include "book.h"

ChaptersDialog::ChaptersDialog(Book *book, QWidget *parent):
        ListWindow(tr("(No chapters)"), parent)
{
    setWindowTitle(tr("Chapters"));
    if (book) {
        foreach (QString id, book->chapters) {
            QString contentTitle = book->content[id].name;
            data.append(contentTitle);
        }
    }
    QStringListModel *model = new QStringListModel(data, this);
    setModel(model);
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
}

void ChaptersDialog::onItemActivated(const QModelIndex &index)
{
    emit goToChapter(index.row());
    close();
}
