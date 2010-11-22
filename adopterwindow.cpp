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

AdopterWindow::AdopterWindow(QWidget *parent):
    QMainWindow(parent), bookView(0), grabbingVolumeKeys(false), toolBar(0)
{
    TRACE;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    frame->show();
    setCentralWidget(frame);

#ifdef Q_OS_SYMBIAN
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#else
    // Tool bar
    setUnifiedTitleAndToolBarOnMac(true);
    toolBar = addToolBar("");
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->toggleViewAction()->setVisible(false);
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
    toolBar->setIconSize(QSize(42, 42));
#endif
#endif // Q_OS_SYMBIAN

    // Monitor settings
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
}

void AdopterWindow::takeChildren(BookView *view, const QList<QWidget *> &others)
{
    TRACE;
    leaveChildren();
    if (view) {
        bookView = view;
        bookView->setParent(centralWidget());
        bookView->show();
        QVBoxLayout *layout =
                qobject_cast<QVBoxLayout *>(centralWidget()->layout());
        layout->addWidget(bookView, 1);
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

bool AdopterWindow::hasChild(QWidget *child)
{
    if (child == bookView) {
        return true;
    }
    return this == child->parent();
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
                                         const QString &text,
                                         bool important)
{
    TRACE;
    qDebug() << "icon" << iconName << "text" << text;
    QAction *action;
#ifndef Q_OS_SYMBIAN
    Q_UNUSED(important);
    action = toolBar->addAction(QIcon(Platform::instance()->icon(iconName)),
                                text, receiver, member);
#else
    if (toolBar && important) {
        QPushButton *button = new QPushButton(this);
        button->setIconSize(QSize(60, 60));
        button->setFixedSize(89, 60);
        button->setIcon(QIcon(Platform::instance()->icon(iconName)));
        button->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Maximum);
        connect(button, SIGNAL(clicked()), receiver, member);
        toolBar->addWidget(button);
    }
    action = new QAction(text, this);
    menuBar()->addAction(action);
    connect(action, SIGNAL(triggered()), receiver, member);
#endif

#if defined Q_WS_MAEMO_5
    action->setText("");
    action->setToolTip("");
#endif

    return action;
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
    if (key == "usevolumekeys") {
        bool grab = Settings::instance()->value(key, false).toBool();
        qDebug() << "AdopterWindow::onSettingsChanged: usevolumekeys" << grab;
        grabVolumeKeys(grab);
    }
}

