#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

/** Same as QListView, except contentsHeight() is public. */
class ListView: public QListView
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = 0);
    int contentsHeight() const;
};

#endif // LISTVIEW_H
