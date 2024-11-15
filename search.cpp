#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtGui>
#include <QFile>
#include <QNetworkAccessManager>
#include <QWebPage>
#include <QWebElementCollection>
#include <QWebFrame>

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

Search::Search(): QObject(0), reply(0), downloadReply(0)
{
    manager = new QNetworkAccessManager(this);
    downloadManager = new QNetworkAccessManager(this);
}

void Search::start(const Query &query)
{
    TRACE;

    emit beginSearch();

    searchResults.clear();
    QNetworkRequest request;
    request.setUrl(QUrl("https://www.gutenberg.org/ebooks/results/"));
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

void Search::download(const Search::Result &result, const QString &fileName)
{
    TRACE;
    downloadResult = result;
    downloadFileName = fileName;
    qDebug() << "UID" << result.id;
    Q_UNUSED(fileName);
    emit beginDownload(0);
    QUrl url("https://www.gutenberg.org/ebooks/" + result.id + ".epub");
    qDebug() << "Requesting" << url;
    QNetworkRequest request;
    request.setUrl(url);
    downloadReply = downloadManager->get(request);
    connect(downloadReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void Search::finished()
{
    TRACE;

    if (!reply) {
        return;
    }

    QByteArray data = reply->readAll();

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
            r.source = "Project Gutenberg";
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

void Search::downloadFinished()
{
    TRACE;

    if (!downloadReply) {
        return;
    }

    // Handle download errors
    if (QNetworkReply::NoError != downloadReply->error()) {
        qCritical() << "Search::downloadFinished: Network error"
                << downloadReply->error();
        downloadReply->deleteLater();
        downloadReply = 0;
        emit endDownload(Search::DownloadError, downloadResult, downloadFileName);
        return;
    }

    // Handle redirection
    QVariant header = downloadReply->header(QNetworkRequest::LocationHeader);
    if (header.isValid()) {
        // Handle redirection: Download again with the new URL
        qDebug() << "Redirected to" << header;
        QNetworkRequest request;
        request.setUrl(header.toUrl());
        downloadReply->deleteLater();
        downloadReply = downloadManager->get(request);
        connect(downloadReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
        return;
    }

    // Handle download success
    QByteArray data = downloadReply->readAll();
    qint64 size = (qint64)data.size();
    qDebug() << "Got" << size << "bytes";
    downloadReply->deleteLater();
    downloadReply = 0;
    QFile out(downloadFileName);
    int status = Search::FileError;
    if (out.open(QIODevice::WriteOnly)) {
        if (size == out.write(data, size)) {
            qDebug() << "Book saved to" << downloadFileName;
            status = Search::Ok;
        } else {
            qCritical() << "Search::downloadFinished: Failed to write" << size
                    << "bytes to" << downloadFileName;
        }
        out.close();
    } else {
        qCritical() << "Search::downloadFinished: Could not open"
                << downloadFileName;
    }
    emit endDownload(status, downloadResult, downloadFileName);
}
