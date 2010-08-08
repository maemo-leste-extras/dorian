#include <QtGui>

#include "progress.h"
#include "trace.h"

Progress::Progress(QWidget *parent): QLabel(parent), progress(0), timer(-1)
{
    hide();
}

void Progress::setProgress(qreal p)
{
    if (progress != p) {
        progress = p;
        flash();
        update();
    }
}

void Progress::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(100, 100, 100, 177)));
    painter.setPen(Qt::NoPen);
    int w = int(width() * progress);
    int h = height();
    painter.drawRect(0, 0, w, h);
    painter.setBrush(QBrush(QColor(100, 100, 100, 50)));
    painter.drawRect(w, 0, width(), h);
}

void Progress::flash()
{
    killTimer(timer);
    show();
    timer = startTimer(400);
}

void Progress::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == timer) {
        killTimer(timer);
        hide();
    }
}
