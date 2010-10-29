#include <QtGui>

#include "listview.h"
#include "searchresultsdialog.h"
#include "searchresultinfodialog.h"
#include "trace.h"

SearchResultsDialog::SearchResultsDialog(const QList<Search::Result> results_,
    QWidget *parent): ListWindow(parent), results(results_)
{
    setWindowTitle(tr("Search results"));

    foreach (Search::Result result, results) {
        QString author;
        if (result.authors.length()) {
            author = result.authors[0];
        }
        data.append(result.title + "\n" + author);
    }

    QStringListModel *model = new QStringListModel(data, this);
    list = new ListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setModel(model);
    list->setUniformItemSizes(true);
    addList(list);
    addItemAction(tr("Download book"), this, SLOT(onDownload()));
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
}

void SearchResultsDialog::onItemActivated(const QModelIndex &index)
{
    Trace t("SearchResultsDialog::onItemActivated");
    Search::Result result = results[index.row()];
    qDebug() << "Book" << index.row() << ":" << result.title;
    SearchResultInfoDialog *d = new SearchResultInfoDialog(result, this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    int ret = d->exec();
    if (ret == QDialog::Accepted) {
        qDebug() << "Accepted -> Start download";
    }
}

void SearchResultsDialog::onDownload()
{
    onItemActivated(list->currentIndex());
}
