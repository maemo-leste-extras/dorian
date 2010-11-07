#include <QtGui>

#include "translucentbutton.h"
#include "platform.h"

const int TranslucentButton::pixels = 95;
const int TranslucentButton::elevatorInterval = 750;

TranslucentButton::TranslucentButton(const QString &name_, QWidget *parent):
    QLabel(parent), name(name_), transparent(true)
{
    setGeometry(0, 0, pixels, pixels);
    elevatorTimer = startTimer(elevatorInterval);
}

TranslucentButton::~TranslucentButton()
{
    killTimer(elevatorTimer);
}

void TranslucentButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!transparent) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawPixmap(0, 0, QPixmap(Platform::icon(name)).scaled(
                QSize(pixels, pixels), Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation));
    } else {
        painter.fillRect(0, 0, pixels, pixels, Qt::NoBrush);
    }
}

void TranslucentButton::flash(int duration)
{
    show();
    raise();
    transparent = false;
    update();
    QTimer::singleShot(duration, this, SLOT(stopFlash()));
}

void TranslucentButton::stopFlash()
{
    transparent = true;
    update();
}

void TranslucentButton::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit triggered();
    e->accept();
}

void TranslucentButton::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == elevatorTimer) {
        raise();
    }
}
