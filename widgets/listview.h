#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

/** Same as QListView, except contentsHeight() is public. */
class ListView: public QListView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = 0): QListView(parent) {
#ifndef Q_OS_SYMBIAN
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setUniformItemSizes(true);
        setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
    int contentsHeight() const {return QListView::contentsSize().height() + 10;}
};

#endif // LISTVIEW_H
