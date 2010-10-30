#ifndef SEARCHRESULTSDIALOG_H
#define SEARCHRESULTSDIALOG_H

#include <QList>
#include <QStringList>

#include "listwindow.h"
#include "search.h"

class ListView;
class QModelIndex;
class ProgressDialog;

class SearchResultsDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit SearchResultsDialog(const QList<Search::Result> searchResults,
                                 QWidget *parent = 0);

protected slots:
    void onDownload();
    void onItemActivated(const QModelIndex &index);
    void onBeginDownload(int size);
    void onEndDownload(int status, const Search::Result &result,
                       const QString &fileName);

protected:
    QString downloadName() const;

private:
    const QList<Search::Result> results;
    ListView *list;
    QStringList data;
    ProgressDialog *progress;
};

#endif // SEARCHRESULTSDIALOG_H
