#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QMainWindow>

class QCloseEvent;
class Book;
class QListWidget;
class QListWidgetItem;

/** Dialog box managing bookmarks. */
class BookmarksDialog: public QMainWindow
{
    Q_OBJECT

public:
    explicit BookmarksDialog(Book *book, QWidget *parent = 0);

signals:
    void goToBookmark(int index);
    void addBookmark();

public slots:
    void onGo();
    void onAdd();
    void onItemActivated(QListWidgetItem *);
    void onClose();
    void onDelete(bool really = false);

protected:
    void closeEvent(QCloseEvent *e);
    Book *book;
    QListWidget *list;
};

#endif // BOOKMARKSDIALOG_H
