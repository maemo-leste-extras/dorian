#include <QtGui>

#include "fullscreenwindow.h"
#include "translucentbutton.h"
#include "progress.h"
#include "trace.h"
#include "settings.h"
#include "platform.h"

static const int MARGIN = 9;

FullScreenWindow::FullScreenWindow(QWidget *parent): AdopterWindow(parent)
{
    TRACE;
    Q_ASSERT(parent);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5NonComposited, true);
#endif // Q_WS_MAEMO_5
#ifndef Q_OS_SYMBIAN
    toolBar->hide();
#endif
    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    setCentralWidget(frame);
    restoreButton = new TranslucentButton("view-normal", this);
    QRect screen = QApplication::desktop()->screenGeometry();
    restoreButton->setGeometry(
        screen.width() - TranslucentButton::pixels - MARGIN,
        screen.height() - TranslucentButton::pixels - MARGIN,
        TranslucentButton::pixels,
        TranslucentButton::pixels);
    connect(restoreButton, SIGNAL(triggered()), this, SIGNAL(restore()));
}

void FullScreenWindow::showEvent(QShowEvent *e)
{
    Trace t("FullScreenWindow::showEvent");
    AdopterWindow::showEvent(e);
    placeChildren();
}

void FullScreenWindow::resizeEvent(QResizeEvent *e)
{
    Trace t("FullScreenWindow::resizeEvent");
    AdopterWindow::resizeEvent(e);
    placeChildren();
}

void FullScreenWindow::placeChildren()
{
    Trace t("FullScreenWindow::placeChildren");

    QRect screen = QApplication::desktop()->screenGeometry();
    int w = screen.width();
    int h = screen.height();

#ifdef Q_WS_MAEMO_5
    // Hack: FullScreenWindow can lose orientation on Maemo...
    QString orientation = Settings::instance()->value("orientation",
        Platform::instance()->defaultOrientation()).toString();
    if (((orientation == "portrait") && (w > h)) ||
        ((orientation == "landscape") && (w < h))) {
        int tmp = w;
        w = h;
        h = tmp;
    }
#endif // Q_WS_MAEMO_5

    restoreButton->setGeometry(
        w - TranslucentButton::pixels - MARGIN,
        h - TranslucentButton::pixels - MARGIN,
        TranslucentButton::pixels,
        TranslucentButton::pixels);
    restoreButton->flash(3000);
}
