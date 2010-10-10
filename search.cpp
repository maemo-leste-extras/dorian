#include "search.h"

Search *inst = 0;

Search *Search::instance()
{
    if (!inst) {
        inst = new Search();
    }
    return inst;
}

void Search::close()
{
    delete inst;
    inst = 0;
}

Search::Search(): QObject(0)
{
}

void Search::start(const Query &query)
{
    emit beginSearch();
    emit endSearch();
}

QList<Search::Result> Search::results()
{
    QList<Search::Result> ret;
    return ret;
}

bool Search::download(const Search::Result &result, const QString &fileName)
{
    Q_UNUSED(result);
    Q_UNUSED(fileName);
    emit beginDownload(0);
    emit endDownload();
    return false;
}
