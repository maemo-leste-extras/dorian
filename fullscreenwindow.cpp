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

#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5NonComposited, true);
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
    //qDebug() << "Softkeys visible?"
    //         << (windowFlags() & Qt::WindowSoftkeysVisibleHint);
    placeChildren();
    AdopterWindow::showEvent(e);
}

void FullScreenWindow::resizeEvent(QResizeEvent *e)
{
    Trace t("FullScreenWindow::resizeEvent");
    placeChildren();
    AdopterWindow::resizeEvent(e);
}

void FullScreenWindow::closeEvent(QCloseEvent *e)
{
    Trace t("FullscreenWindow::closeEvent");
    AdopterWindow::closeEvent(e);
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
