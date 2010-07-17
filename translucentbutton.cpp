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
    hide();
}

void TranslucentButton::paintEvent(QPaintEvent *e)
{
    (void)e;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(0, 0, QPixmap(ICON_PREFIX + name + ".png").
        scaled(QSize(95, 95), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void TranslucentButton::flash()
{
    QPropertyAnimation *ani = new QPropertyAnimation(this, "opacity", 0);
    ani->setDuration(5000);
    ani->setStartValue(0.);
    ani->setEndValue(1.);
    ani->setEasingCurve(QEasingCurve::OutQuart);
    show();
    ani->start(QPropertyAnimation::DeleteWhenStopped);
    connect(ani, SIGNAL(destroyed()), this, SLOT(onAnimationEnd()));
}

void TranslucentButton::setOpacity(qreal opacity)
{
    mOpacity = opacity;
    update();
}

void TranslucentButton::onAnimationEnd()
{
    hide();
}
