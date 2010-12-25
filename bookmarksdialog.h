#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QStringList>

#include "listwindow.h"
#include "book.h"

class QCloseEvent;

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
    QString bookmarkToText(const Book::Bookmark &bookmark);

private:
    Book *book;
    QStringList data;
};

#endif // BOOKMARKSDIALOG_H
