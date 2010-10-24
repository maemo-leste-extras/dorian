#ifndef SEARCHRESULTSDIALOG_H
#define SEARCHRESULTSDIALOG_H

#include <QList>
#include <QStringList>

#include "listwindow.h"
#include "search.h"

class ListView;

class SearchResultsDialog: public ListWindow
{
public:
    explicit SearchResultsDialog(const QList<Search::Result> searchResults,
                                 QWidget *parent = 0);

protected:
    const QList<Search::Result> results;
    ListView *list;
    QStringList data;
};

#endif // SEARCHRESULTSDIALOG_H
