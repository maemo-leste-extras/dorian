#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "search.h"
#include "platform.h"
#include "trace.h"

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

Search::Search(): QObject(0), reply(0)
{
    manager = new QNetworkAccessManager(this);
}

void Search::start(const Query &query)
{
    Trace t("Search::start");

    emit beginSearch();

    QNetworkRequest request;
    request.setUrl(QUrl("http://www.gutenberg.org/catalog/world/results"));
    // request.setRawHeader("User-Agent", "Dorian " + Platform::version());
    QByteArray data;
    data = "title=" + QUrl::toPercentEncoding(query.title) + "&author=" +
           QUrl::toPercentEncoding(query.author);
    qDebug() << "Request:" << data;
    reply = manager->post(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
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

void Search::finished()
{
    Trace t("Search::finished");
    QByteArray data = reply->readAll();
    qDebug() << data;
    reply->deleteLater();
    emit endSearch();
}
