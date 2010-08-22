#ifndef BOOKMARKINFODIALOG_H
#define BOOKMARKINFODIALOG_H

#include "dyalog.h"

class Book;

class BookmarkInfoDialog: public Dyalog
{
    Q_OBJECT

public:
    enum {GoTo = 1, Delete};
    explicit BookmarkInfoDialog(Book *b, int i, QWidget *parent);

public slots:
    void onRead();
    void onRemove();

protected:
    Book *book;
    int index;
};


#endif // BOOKMARKINFODIALOG_H
