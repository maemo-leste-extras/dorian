#include <QtGui>

#include "mainbase.h"
#include "trace.h"
#include "platform.h"

MainBase::MainBase(QWidget *parent): QMainWindow(parent), toolBar(0)
{
    TRACE;

#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    setCentralWidget(frame);

#if defined(Q_OS_SYMBIAN)
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#endif // Q_OS_SYMBIAN
}

void MainBase::addToolBar()
{
    TRACE;

    if (toolBar) {
        return;
    }

#if defined(Q_OS_SYMBIAN)
    toolBar = new QToolBar("", this);
    QMainWindow::addToolBar(Qt::BottomToolBarArea, toolBar);
#else
    toolBar = QMainWindow::addToolBar("");
#endif

    setUnifiedTitleAndToolBarOnMac(true);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->toggleViewAction()->setVisible(false);

#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
    toolBar->setIconSize(QSize(42, 42));
#endif
}

QAction *MainBase::addToolBarAction(QObject *receiver,
                                    const char *member,
                                    const QString &iconName,
                                    const QString &text,
                                    bool important)
{
    TRACE;
    qDebug() << "icon" << iconName << "text" << text;
    QAction *action;
#ifdef Q_OS_SYMBIAN
    if (important) {
        // Add tool bar action
        addToolBar();
        QPushButton *button = new QPushButton(this);
        button->setIconSize(QSize(60, 60));
        button->setFixedHeight(60);
        button->setIcon(QIcon(Platform::instance()->icon(iconName)));
        button->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Maximum);
        connect(button, SIGNAL(clicked()), receiver, member);
        toolBar->addWidget(button);
    }
    // Add menu action, too
    action = menuBar()->addAction(text);
    connect(action, SIGNAL(triggered()), receiver, member);
#else
    Q_UNUSED(important);
    addToolBar();
    action = toolBar->addAction(QIcon(Platform::instance()->icon(iconName)),
                                text, receiver, member);
#endif

    action->setToolTip("");
    return action;
}

void MainBase::addToolBarSpace()
{
    addToolBar();
    QFrame *frame = new QFrame(toolBar);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(frame);
}

int MainBase::toolBarHeight()
{
    return toolBar? toolBar->height(): 0;
}

void MainBase::show()
{
    Trace t("MainBase::show");
#ifdef Q_OS_SYMBIAN
    foreach (QWidget *w, QApplication::allWidgets()) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
    showMaximized();
#else
    QMainWindow::show();
#endif
}
