#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QWebFrame>
#include <QWebPage>
#include <QWebElementCollection>
#include <QWebElement>

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

    searchResults.clear();
    QNetworkRequest request;
    request.setUrl(QUrl("http://www.gutenberg.org/catalog/world/results"));
    // request.setRawHeader("User-Agent", "Dorian " + Platform::version());
    QString title = query.title;
    if (title.isEmpty()) {
        title = ".";
    }
    QByteArray data;
    data = "title=" + QUrl::toPercentEncoding(title) + "&author=" +
           QUrl::toPercentEncoding(query.author);
    qDebug() << "Request:" << (request.url().toString() + "?" + data);
    reply = manager->post(request, data);
    connect(reply, SIGNAL(finished()), this, SLOT(finished()));
}

QList<Search::Result> Search::results()
{
    return searchResults;
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

    if (!reply) {
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << data;

    // Parse search results

    QWebPage page(this);
    QWebFrame *frame = page.mainFrame();
    frame->setHtml(QString(data));
    QWebElementCollection tables = frame->findAllElements("table");
    if (tables.count() == 1) {
        QWebElement table = tables[0];
        foreach (QWebElement row, table.findAll("tr")) {
            QWebElementCollection cols = row.findAll("td");
            if (cols.count() < 5) {
                continue;
            }
            QString id = cols[0].toPlainText().trimmed();
            if (id.isEmpty()) {
                continue;
            }
            QString author = cols[2].toPlainText().trimmed();
            QStringList titles = cols[3].toPlainText().trimmed().
                                 split("\n", QString::SkipEmptyParts);
            Result r;
            r.authors = author.split("\n", QString::SkipEmptyParts);
            r.id = id;
            if (titles.count()) {
                r.title = titles[0];
            }
            r.language = cols[4].toPlainText().trimmed();
            searchResults.append(r);
            qDebug() << id;
            qDebug() << " Authors:" << r.authors;
            qDebug() << " Title:" << r.title;
            qDebug() << " Language:" << r.language;
        }
    }

    reply->deleteLater();
    reply = 0;
    emit endSearch();
}
