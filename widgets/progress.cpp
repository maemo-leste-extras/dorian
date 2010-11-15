#include <QtGui>

#include "progress.h"
#include "trace.h"

Progress::Progress(QWidget *parent):
        QLabel(parent), progress(-1.0), timer(-1), mThickness(15)
{
    hide();
}

void Progress::setProgress(qreal p)
{
    TRACE;
    qDebug() << p;
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
    painter.drawRect(0, 0, w, mThickness);
    painter.setBrush(QBrush(QColor(100, 100, 100, 50)));
    painter.drawRect(w, 0, width() - w, mThickness);
}

void Progress::flash()
{
    killTimer(timer);
    show();
    timer = startTimer(700);
}

void Progress::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == timer) {
        killTimer(timer);
        timer = -1;
        hide();
    }
    QLabel::timerEvent(e);
}

int Progress::thickness() const
{
    return mThickness;
}
