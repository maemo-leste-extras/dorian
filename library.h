#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QList>

#include "book.h"

/** Library of books. */
class Library: public QObject
{
    Q_OBJECT

public:
    static Library *instance();
    static void close();
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
    void bookAdded();
    void bookRemoved(int index);
    void currentBookChanged();

private:
    Library();
    ~Library();
    void load();
    void clear();
    static Library *mInstance;
    QList<Book *> mBooks;
    Book *mCurrent;
};

#endif // LIBRARY_H
