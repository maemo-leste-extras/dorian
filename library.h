#ifndef LIBRARY_H
#define LIBRARY_H

#include <QAbstractListModel>
#include <QVariant>
#include <QString>
#include <QList>

class QObject;
class Book;

/** Library of books. */
class Library: public QAbstractListModel
{
    Q_OBJECT

public:
    enum {
        BookRole = Qt::UserRole + 1,
    };

    static Library *instance();
    static void close();
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void save();
    int find(QString path) const;
    int find(const Book *book) const;
    Book *at(int index) const;
    int size() const;
    Book *current() const;
    bool add(QString path);
    void remove(int index);
    void setCurrent(int index);

signals:
    void currentBookChanged();

private:
    explicit Library(QObject *parent = 0);
    ~Library();
    void load();
    void clear();
    static Library *mInstance;
    QList<Book *> mBooks;
    Book *mCurrent;
};

#endif // LIBRARY_H
