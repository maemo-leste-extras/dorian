#include <QtGui>

#include "listview.h"
#include "searchresultsdialog.h"

SearchResultsDialog::SearchResultsDialog(const QList<Search::Result> results_,
    QWidget *parent): ListWindow(parent), results(results_)
{
    setWindowTitle(tr("Search results"));

    foreach (Search::Result result, results) {
        QString author;
        if (result.authors.length()) {
            author = result.authors[0];
        }
        data.append(author + "\n" + result.title);
    }

    QStringListModel *model = new QStringListModel(data, this);
    list = new ListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setModel(model);
    list->setUniformItemSizes(true);
    addList(list);
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
}

