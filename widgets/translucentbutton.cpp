#include "translucentbutton.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

TranslucentButton::TranslucentButton(const QString &name_, QWidget *parent):
    QWidget(parent), name(name_), opacity(1)
{
    setGeometry(0, 0, 50, 50);
    timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(stopFlash()));
    show();
}

void TranslucentButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (opacity < 1) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawPixmap(0, 0, QPixmap(ICON_PREFIX + name + ".png").scaled(
                QSize(95, 95), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        painter.fillRect(0, 0, 95, 95, Qt::NoBrush);
    }
}

void TranslucentButton::flash(int duration)
{
    opacity = 0;
    update();
    timer->start(duration);
}

void TranslucentButton::stopFlash()
{
    opacity = 1;
    update();
}
