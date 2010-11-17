#ifndef INFODIALOG_H
#define INFODIALOG_H

#include "dyalog.h"

class QWidget;
class Book;

/** Display book information. */
class InfoDialog: public Dyalog
{
    Q_OBJECT

public:
    enum {Close, Read, Delete}; //< Dialog result codes.
    explicit InfoDialog(Book *book, QWidget *parent, bool showButtons = true);

public slots:
    void onReadBook();
    void onRemoveBook();

private:
    Book *book;
};

#endif // INFODIALOG_H
