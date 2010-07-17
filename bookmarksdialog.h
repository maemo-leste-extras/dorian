#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QDialog>

class Book;
class QListWidget;
class QListWidgetItem;

/** Dialog box managing bookmarks. */
class BookmarksDialog: public QDialog
{
    Q_OBJECT

public:
    explicit BookmarksDialog(Book *book, QWidget *parent = 0);

signals:

public slots:
    void onGo();
    void onAdd();
    void onItemActivated(QListWidgetItem *);

protected:
    Book *book;
    QListWidget *list;
};

#endif // BOOKMARKSDIALOG_H
