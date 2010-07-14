#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

class Book;

class InfoDialog: public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(Book *book, QWidget *parent = 0);

public slots:
    void onReadBook();
    void onRemoveBook();

private:
    Book *book;
};

#endif // INFODIALOG_H
