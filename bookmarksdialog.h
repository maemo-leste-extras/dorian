#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QStringList>

#include "listwindow.h"

class QCloseEvent;
class Book;

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
    void onDelete();
    void onEdit();
    void onItemActivated(const QModelIndex &index);

protected:
    void reallyDelete();

private:
    Book *book;
    QStringList data;
};

#endif // BOOKMARKSDIALOG_H
