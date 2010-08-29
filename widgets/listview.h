#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

class ListView: public QListView
{
    Q_OBJECT
public:
    explicit ListView(QWidget *parent = 0): QListView(parent) {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
    int contentsHeight() const {return QListView::contentsSize().height() + 10;}
};

#endif // LISTVIEW_H
