#include <QtGui>

#include "fullscreenwindow.h"
#include "translucentbutton.h"
#include "trace.h"

FullScreenWindow::FullScreenWindow(QWidget *parent): AdopterWindow(parent)
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
    restoreButton = new TranslucentButton("view-normal", this);
    QRect screen = QApplication::desktop()->screenGeometry();
    restoreButton->setGeometry((screen.width() - TranslucentButton::pixels) / 2,
        screen.height() - TranslucentButton::pixels - 9,
        TranslucentButton::pixels, TranslucentButton::pixels);
    connect(restoreButton, SIGNAL(triggered()), this, SIGNAL(restore()));
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

void FullScreenWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    QRect screen = QApplication::desktop()->screenGeometry();
    restoreButton->setGeometry(screen.width() - TranslucentButton::pixels - 9,
        screen.height() - TranslucentButton::pixels - 9,
        TranslucentButton::pixels, TranslucentButton::pixels);
}
