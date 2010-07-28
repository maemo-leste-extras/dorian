#include <QtGui>

#include "fullscreenwindow.h"
#include "translucentbutton.h"

FullScreenWindow::FullScreenWindow(QWidget *child, QWidget *parent):
        QMainWindow(parent)
{
    Q_ASSERT(parent);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5PortraitOrientation,
                 parent->testAttribute(Qt::WA_Maemo5PortraitOrientation));
    setAttribute(Qt::WA_Maemo5LandscapeOrientation,
                 parent->testAttribute(Qt::WA_Maemo5LandscapeOrientation));
#endif // Q_WS_MAEMO_5
    child->setParent(this);
    setCentralWidget(child);
    restoreButton = new TranslucentButton("view-fullscreen", this);
}

void FullScreenWindow::showFullScreen()
{
    QWidget::showFullScreen();
    restoreButton->flash();
}

void FullScreenWindow::MOUSE_ACTIVATE_EVENT(QMouseEvent *event)
{
    if (fullScreenZone().contains(event->x(), event->y())) {
        emit restore();
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
