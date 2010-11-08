#include <QtGui>

#if defined(Q_WS_MAEMO_5)
#   include <QtGui/QX11Info>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <QAbstractKineticScroller>
#endif

#include "adopterwindow.h"
#include "trace.h"
#include "bookview.h"
#include "platform.h"
#include "settings.h"

AdopterWindow::AdopterWindow(QWidget *parent): QMainWindow(parent), bookView(0)
{
    TRACE;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif // Q_WS_MAEMO_5

    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    setCentralWidget(frame);

#ifdef Q_OS_SYMBIAN
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#else
    // Tool bar
    setUnifiedTitleAndToolBarOnMac(true);
    toolBar = addToolBar("controls");
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->toggleViewAction()->setVisible(false);
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
    toolBar->setIconSize(QSize(42, 42));
#endif
#endif // Q_OS_SYMBIAN

    // Monitor settings
    Settings *settings = Settings::instance();
    connect(settings, SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    settings->setValue("usevolumekeys", settings->value("usevolumekeys"));
}

void AdopterWindow::takeChildren(BookView *view, const QList<QWidget *> &others)
{
    TRACE;
    leaveChildren();
    if (view) {
        bookView = view;
        bookView->setParent(centralWidget());
        centralWidget()->layout()->addWidget(bookView);
        bookView->show();
    }
    foreach (QWidget *child, others) {
        if (child) {
            child->setParent(this);
        }
    }
}

void AdopterWindow::leaveChildren()
{
    TRACE;
    if (bookView) {
        centralWidget()->layout()->removeWidget(bookView);
        bookView = 0;
    }
}

void AdopterWindow::show()
{
#ifdef Q_OS_SYMBIAN
    foreach (QWidget *w, QApplication::allWidgets()) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
    showMaximized();
    raise();
#else
    QMainWindow::show();
#endif
}

QAction *AdopterWindow::addToolBarAction(QObject *receiver,
                                         const char *member,
                                         const QString &iconName,
                                         const QString &text)
{
    TRACE;
    qDebug() << "icon" << iconName << "text" << text;
#ifndef Q_OS_SYMBIAN
    return toolBar->addAction(QIcon(Platform::instance()->icon(iconName)),
                              text, receiver, member);
#else
    Q_UNUSED(iconName);
    QAction *action = new QAction(text, this);
    menuBar()->addAction(action);
    connect(action, SIGNAL(triggered()), receiver, member);
    return action;
#endif
}

void AdopterWindow::addToolBarSpace()
{
#ifndef Q_OS_SYMBIAN
    QFrame *frame = new QFrame(toolBar);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(frame);
#endif
}

void AdopterWindow::grabVolumeKeys(bool grab)
{
    TRACE;
    grabbingVolumeKeys = grab;
#ifdef Q_WS_MAEMO_5
    doGrabVolumeKeys(grab);
#endif
}

#ifdef Q_WS_MAEMO_5

void AdopterWindow::doGrabVolumeKeys(bool grab)
{
    TRACE;
    if (!isVisible()) {
        qDebug() << "Not visible - skipping";
        return;
    }
    if (!winId()) {
        qDebug() << "Could not get window ID - skipping";
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
    qDebug() << "Grabbed volume keys";
}

#endif // Q_WS_MAEMO_5

#ifdef Q_WS_MAEMO_5

void AdopterWindow::showEvent(QShowEvent *e)
{
    TRACE;
    doGrabVolumeKeys(grabbingVolumeKeys);
    QMainWindow::showEvent(e);
}

#endif // Q_WS_MAEMO_5

void AdopterWindow::keyPressEvent(QKeyEvent *event)
{
    TRACE;
    if (bookView && grabbingVolumeKeys) {
        switch (event->key()) {
#ifdef Q_WS_MAEMO_5
        case Qt::Key_F7:
            qDebug() << "F7";
            bookView->goNextPage();
            event->accept();
            break;
        case Qt::Key_F8:
            qDebug() << "F8";
            bookView->goPreviousPage();
            event->accept();
            break;
#endif // Q_WS_MAEMO_5
        case Qt::Key_PageUp:
            bookView->goPreviousPage();
            event->accept();
            break;
        case Qt::Key_PageDown:
            bookView->goNextPage();
            event->accept();
            break;
        default:
            ;
        }
    }
    QMainWindow::keyPressEvent(event);
}

void AdopterWindow::onSettingsChanged(const QString &key)
{
    TRACE;
    if (key == "usevolumekeys") {
        qDebug() << key;
        grabVolumeKeys(Settings::instance()->value(key).toBool());
    }
}

