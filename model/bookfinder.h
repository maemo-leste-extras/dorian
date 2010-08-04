#ifndef BOOKFINDER_H
#define BOOKFINDER_H

#include <QObject>
#include <QThread>

class QStringList;
class QString;

class BookFinder: public QObject
{
    Q_OBJECT

public:
    explicit BookFinder(QObject *parent = 0);

public slots:
    void find(const QStringList &directories, const QStringList &books);

signals:
    void add(const QString &book);
    void remove(const QString &book);

public slots:
};

class BookFinderThread: public QThread
{
public:
   void run () {
       exec();
   }
};

#endif // BOOKFINDER_H