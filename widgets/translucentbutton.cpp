#include <QtGui>

#include "translucentbutton.h"
#include "trace.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

const int TranslucentButton::pixels = 95;

TranslucentButton::TranslucentButton(const QString &name_, QWidget *parent):
    QLabel(parent), name(name_), transparent(true)
{
    setGeometry(0, 0, pixels, pixels);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(stopFlash()));
    show();
}

void TranslucentButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (!transparent) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawPixmap(0, 0, QPixmap(ICON_PREFIX + name + ".png").scaled(
                QSize(pixels, pixels), Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation));
    } else {
        painter.fillRect(0, 0, pixels, pixels, Qt::NoBrush);
    }
}

void TranslucentButton::flash(int duration)
{
    raise();
    transparent = false;
    update();
    timer->start(duration);
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
