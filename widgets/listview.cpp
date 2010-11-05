#include <QtGui>

#include "listview.h"
#include "trace.h"

ListView::ListView(QWidget *parent): QListView(parent)
{
#ifndef Q_OS_SYMBIAN
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setUniformItemSizes(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

int ListView::contentsHeight() const
{
    return QListView::contentsSize().height() + 10;
}
