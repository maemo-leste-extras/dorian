#include <QtGui>

#include "fullscreenwindow.h"
#include "translucentbutton.h"
#include "trace.h"

FullScreenWindow::FullScreenWindow(QWidget *parent): QMainWindow(parent), child(0)
{
    Q_ASSERT(parent);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5NonComposited, true);
#endif // Q_WS_MAEMO_5
    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    setCentralWidget(frame);
    restoreButton = new TranslucentButton("view-fullscreen", this);
}

void FullScreenWindow::showFullScreen()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5PortraitOrientation, parentWidget()->
                 testAttribute(Qt::WA_Maemo5PortraitOrientation));
    setAttribute(Qt::WA_Maemo5LandscapeOrientation, parentWidget()->
                 testAttribute(Qt::WA_Maemo5LandscapeOrientation));
#endif // Q_WS_MAEMO_5
    QWidget::showFullScreen();
    restoreButton->flash();
}

void FullScreenWindow::MOUSE_ACTIVATE_EVENT(QMouseEvent *event)
{
    Trace t("FullScreenWindow::MOUSE_ACTIVATE_EVENT");
    if (fullScreenZone().contains(event->x(), event->y())) {
        emit restore();
    } else {
        restoreButton->flash();
    }
    QMainWindow::MOUSE_ACTIVATE_EVENT(event);
}

QRect FullScreenWindow::fullScreenZone() const
{
    return QRect(width() / 2 - 45, height() - 104, 95, 95);
}

void FullScreenWindow::resizeEvent(QResizeEvent *e)
{
    restoreButton->setGeometry(fullScreenZone());
    QMainWindow::resizeEvent(e);
}

void FullScreenWindow::takeChild(QWidget *c)
{
    leaveChild();
    if (c) {
        child = c;
        child->setParent(centralWidget());
        centralWidget()->layout()->addWidget(child);
        connect(child, SIGNAL(suppressedMouseButtonPress()),
                              restoreButton, SLOT(flash()));
    }
}

void FullScreenWindow::leaveChild()
{
    if (child) {
        centralWidget()->layout()->removeWidget(child);
        child = 0;
    }
}
