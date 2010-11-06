#ifndef SEARCHRESULTINFODIALOG_H
#define SEARCHRESULTINFODIALOG_H

#include "dyalog.h"
#include "search.h"

/** Details of a single search result. */
class SearchResultInfoDialog: public Dyalog
{
    Q_OBJECT
public:
    explicit SearchResultInfoDialog(const Search::Result &result,
                                    QWidget *parent = 0);

private:
    const Search::Result &result;
};

#endif // SEARCHRESULTINFODIALOG_H
