#include <QtGui>

#ifdef Q_WS_MAEMO_5
#   include <QtGui/QX11Info>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#endif // Q_WS_MAEMO_5

#include "bookwindow.h"
#include "trace.h"

BookWindow::BookWindow(QWidget *parent):
        QMainWindow(parent), grabbingZoomKeys(false), mainChild(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif // Q_WS_MAEMO_5

    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    setCentralWidget(frame);
}

void BookWindow::takeChildren(QWidget *main, const QList<QWidget *> &others)
{
    Trace t("BookWindow::takeChildren");
    leaveChildren();
    if (main) {
        mainChild = main;
        mainChild->setParent(centralWidget());
        centralWidget()->layout()->addWidget(mainChild);
        mainChild->show();
    }
    foreach (QWidget *child, others) {
        if (child) {
            child->setParent(this);
        }
    }
}

void BookWindow::leaveChildren()
{
    Trace t("BookWindow::leaveChildren");
    if (mainChild) {
        centralWidget()->layout()->removeWidget(mainChild);
        mainChild = 0;
    }
}

void BookWindow::grabZoomKeys(bool grab)
{
    Trace t("BookWindow::grabZoomKeys");
    grabbingZoomKeys = grab;
    doGrabZoomKeys(grab);
}

void BookWindow::showEvent(QShowEvent *e)
{
    Trace t("BookWindow::showEvent");
    doGrabZoomKeys(grabbingZoomKeys);
    QMainWindow::showEvent(e);
}

void BookWindow::doGrabZoomKeys(bool grab)
{
    Trace t("BookWindow::doGrabZoomKeys");
#ifdef Q_WS_MAEMO_5
    if (!isVisible()) {
        t.trace("Not visible - skipping");
    }
    if (!winId()) {
        t.trace("Could not get window ID - skipping");
        return;
    }
    unsigned long val = grab? 1: 0;
    Atom atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
    if (!atom) {
        qCritical() << "Unable to obtain _HILDON_ZOOM_KEY_ATOM";
        return;
    }
    XChangeProperty(QX11Info::display(),
        winId(),
        atom,
        XA_INTEGER,
        32,
        PropModeReplace,
        reinterpret_cast<unsigned char *>(&val),
        1);
#else
    Q_UNUSED(grab);
#endif // Q_WS_MAEMO_5
}
