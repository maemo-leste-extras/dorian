#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QStringList>

#include "listwindow.h"

class QCloseEvent;
class Book;
class ListView;

/** Dialog box managing bookmarks. */
class BookmarksDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit BookmarksDialog(Book *book, QWidget *parent = 0);

signals:
    void goToBookmark(int index);
    void addBookmark(const QString &note);

public slots:
    void onGo();
    void onAdd();
    void onDelete(bool really = false);
    void onItemActivated(const QModelIndex &index);

protected:
    Book *book;
    ListView *list;
    QStringList data;
};

#endif // BOOKMARKSDIALOG_H
