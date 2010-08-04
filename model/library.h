#ifndef LIBRARY_H
#define LIBRARY_H

#include <QAbstractListModel>
#include <QVariant>
#include <QString>
#include <QList>
#include <QStringList>

class QObject;
class Book;

/** Library of books. */
class Library: public QAbstractListModel
{
    Q_OBJECT

public:
    static Library *instance();
    static void close();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void save();
    QModelIndex find(QString path) const;
    QModelIndex find(const Book *book) const;
    void setNowReading(const QModelIndex &index);
    QModelIndex nowReading() const;
    Book *book(const QModelIndex &index);
    QStringList bookPaths();
    QStringList folders() const;
    bool addFolder(const QString &folder);
    bool removeFolder(const QString &folder);
    void scanFolders();

signals:
    void nowReadingChanged();

public slots:
    bool add(const QString &path);
    void remove(const QString &path);
    void remove(const QModelIndex &index);
    void onBookOpened(const QString &path);

private:
    explicit Library(QObject *parent = 0);
    ~Library();
    void load();
    void clear();
    static Library *mInstance;
    QList<Book *> mBooks;
    QModelIndex mNowReading;
    QStringList mFolders;
};

#endif // LIBRARY_H
