#include "translucentbutton.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

TranslucentButton::TranslucentButton(const QString &name_, QWidget *parent):
    QWidget(parent), name(name_), mOpacity(1.)
{
    setGeometry(0, 0, 50, 50);
    show();
}

void TranslucentButton::paintEvent(QPaintEvent *)
{
    if (mOpacity < 1) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawPixmap(0, 0, QPixmap(ICON_PREFIX + name + ".png").scaled(
                QSize(95, 95), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}

void TranslucentButton::flash(int duration)
{
    QPropertyAnimation *ani = new QPropertyAnimation(this, "opacity", 0);
    ani->setDuration(duration);
    ani->setStartValue(0.);
    ani->setEndValue(1.);
    ani->setEasingCurve(QEasingCurve::OutQuart);
    ani->start(QPropertyAnimation::DeleteWhenStopped);
}

void TranslucentButton::setOpacity(qreal opacity)
{
    mOpacity = opacity;
    update();
}
