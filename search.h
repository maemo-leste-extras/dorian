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
    /** Search query. */
    struct Query
    {
        QString title;
        QString author;
        QStringList languages;
    };

    /** Search result. */
    struct Result
    {
        QString id;
        QString source;
        QString title;
        QStringList authors;
        QString language;
        QImage cover;
        bool operator ==(const Result &other) const {
            return (source == other.source) && (id == other.id);
        }
    };

    /** Download status. */
    enum
    {
        Ok,
        DownloadError,
        FileError,
    };

    static Search *instance();
    static void close();

signals:
    void beginSearch();
    void searching();
    void endSearch();
    void beginDownload(int totalBlocks);
    void downloading(int blocks);
    void endDownload(int status, const Search::Result &result,
                     const QString &fileName);

public slots:
    void start(const Query &query);
    QList<Result> results();
    void download(const Result &result, const QString &fileName);
    void finished();
    void downloadFinished();

private:
    explicit Search();
    QNetworkAccessManager *manager;
    QNetworkAccessManager *downloadManager;
    QNetworkReply *reply;
    QNetworkReply *downloadReply;
    QList<Result> searchResults;
    Search::Result downloadResult;
    QString downloadFileName;
};

#endif // SEARCH_H
