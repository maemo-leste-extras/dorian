#ifndef SEARCH_H
#define SEARCH_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>
#include <QList>

class QNetworkAccessManager;
class QNetworkReply;

/** Search for books, display and download results. */
class Search: public QObject
{
    Q_OBJECT

public:
    struct Query
    {
        QString title;
        QString author;
        QStringList languages;
    };

    struct Result
    {
        QString id;
        QString source;
        QString title;
        QStringList authors;
        QString language;
        QImage cover;
    };

    static Search *instance();
    static void close();

signals:
    void beginSearch();
    void searching();
    void endSearch();
    void beginDownload(int totalBlocks);
    void downloading(int blocks);
    void endDownload();

public slots:
    void start(const Query &query);
    QList<Result> results();
    bool download(const Result &result, const QString &fileName);
    void finished();
    void downloadFinished();

private:
    explicit Search();
    QNetworkAccessManager *manager;
    QNetworkAccessManager *downloadManager;
    QNetworkReply *reply;
    QNetworkReply *downloadReply;
    QList<Result> searchResults;
};

#endif // SEARCH_H
