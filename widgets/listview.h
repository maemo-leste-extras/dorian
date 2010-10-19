#ifndef LISTVIEW_H
#define LISTVIEW_H

#include <QListView>

#ifdef Q_OS_SYMBIAN_ONE_DAY
#include "flickable.h"
#endif

/** Same as QListView, except contentsHeight() is public. */
class ListView: public QListView
#ifdef Q_OS_SYMBIAN_ONE_DAY
        , public Flickable
#endif
{
    Q_OBJECT

public:
    explicit ListView(QWidget *parent = 0);
    int contentsHeight() const;

#ifdef Q_OS_SYMBIAN_ONE_DAY

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QPoint scrollOffset() const;
    void setScrollOffset(const QPoint &offset);

private:
    int offset;

#endif // Q_OS_SYMBIAN_ONE_DAY
};

#endif // LISTVIEW_H
