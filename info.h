#ifndef INFO_H
#define INFO_H

#include <QScrollArea>

class QLabel;
class Book;

class Info: public QScrollArea
{
    Q_OBJECT

public:
    explicit Info(Book *book, QWidget *parent = 0);

private:
    QLabel *title;
    QLabel *path;
    QLabel *creators;
};

#endif // INFO_H
