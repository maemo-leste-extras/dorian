#include <QtGui>

#include "mainbase.h"
#include "trace.h"
#include "platform.h"

MainBase::MainBase(QWidget *parent): QMainWindow(parent), toolBar(0)
{
    TRACE;

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    QFrame *frame = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(0);
    frame->setLayout(layout);
    //frame->show();
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
#ifndef Q_OS_SYMBIAN
    Q_UNUSED(important);
    action = toolBar->addAction(QIcon(Platform::instance()->icon(iconName)),
                                text, receiver, member);
#else
    if (!toolBar && important) {
        // Create tool bar if needed
        toolBar = new QToolBar("", this);
        // toolBar->setFixedHeight(63);
        toolBar->setStyleSheet("margin:0; border:0; padding:0");
        toolBar->setSizePolicy(QSizePolicy::MinimumExpanding,
                               QSizePolicy::Maximum);
        addToolBar(Qt::BottomToolBarArea, toolBar);
    }
    if (important) {
        // Add tool bar action
        QPushButton *button = new QPushButton(this);
        button->setIconSize(QSize(60, 60));
        button->setFixedSize(89, 60);
        button->setIcon(QIcon(Platform::instance()->icon(iconName)));
        button->setSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Maximum);
        connect(button, SIGNAL(clicked()), receiver, member);
        toolBar->addWidget(button);
    }
    // Add menu action, too
    action = new QAction(text, this);
    menuBar()->addAction(action);
    connect(action, SIGNAL(triggered()), receiver, member);
#endif

    action->setText("");
    action->setToolTip("");

    return action;
}

void MainBase::addToolBarSpace()
{
#ifndef Q_OS_SYMBIAN
    QFrame *frame = new QFrame(toolBar);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(frame);
#endif
}

void MainBase::updateToolBar()
{
#ifdef Q_OS_SYMBIAN
    TRACE;
    if (toolBar) {
        QRect geometry = QApplication::desktop()->geometry();
        bool isPortrait = geometry.width() < geometry.height();
        bool isToolBarHidden = toolBar->isHidden();
        if (isPortrait && isToolBarHidden) {
            qDebug() << "Show tool bar";
            toolBar->setVisible(true);
        } else if (!isPortrait && !isToolBarHidden) {
            qDebug() << "Hide tool bar";
            toolBar->setVisible(false);
        }
    }
#endif // Q_OS_SYMBIAN
}

void MainBase::showEvent(QShowEvent *event)
{
    Trace t("MainBase::showEvent");
    updateToolBar();
    QMainWindow::showEvent(event);
}

void MainBase::resizeEvent(QResizeEvent *event)
{
    Trace t("MainBase::resizeEvent");
    updateToolBar();
    QMainWindow::resizeEvent(event);
}

void MainBase::hideToolBar()
{
    if (toolBar) {
        toolBar->hide();
    }
}

bool MainBase::isToolBarHidden()
{
    return toolBar && toolBar->isHidden();
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
