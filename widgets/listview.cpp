#include <QtGui>

#include "listview.h"
#include "trace.h"

ListView::ListView(QWidget *parent): QListView(parent)
{
#ifndef Q_OS_SYMBIAN
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
#ifdef Q_OS_SYMBIAN_ONE_DAY
    offset = 0;
    Flickable::setAcceptMouseClick(this);
#endif
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setUniformItemSizes(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
}

int ListView::contentsHeight() const
{
    return QListView::contentsSize().height() + 10;
}

#ifdef Q_OS_SYMBIAN_ONE_DAY

QPoint ListView::scrollOffset() const
{
    TRACE;
    qDebug() << "0," << offset;
    return QPoint(0, offset);
}

void ListView::setScrollOffset(const QPoint &o)
{
    TRACE;
    qDebug() << o;
    offset = o.y();
    QListView::scrollContentsBy(0, offset)
}

void ListView::mousePressEvent(QMouseEvent *event)
{
    Flickable::handleMousePress(event);
    if (!event->isAccepted()) {
        QListView::mousePressEvent(event);
    }
}

void ListView::mouseReleaseEvent(QMouseEvent *event)
{
    Flickable::handleMouseRelease(event);
    if (!event->isAccepted()) {
        QListView::mouseReleaseEvent(event);
    }
}

void ListView::mouseMoveEvent(QMouseEvent *event)
{
    Flickable::handleMouseMove(event);
    if (!event->isAccepted()) {
        QListView::mouseMoveEvent(eveevent);
    }
}

#endif // Q_OS_SYMBIAN_ONE_DAY
