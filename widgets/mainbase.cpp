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
#ifdef Q_OS_SYMBIAN
    if (important) {
        if (!toolBar) {
            // Create tool bar if needed
            toolBar = new QToolBar("", this);
            toolBar->setStyleSheet("margin:0; border:0; padding:0");
            addToolBar(Qt::BottomToolBarArea, toolBar);
        }
        // Add tool bar action
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
    action = toolBar->addAction(QIcon(Platform::instance()->icon(iconName)),
                                text, receiver, member);
#endif

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
#if 0 // ifdef Q_OS_SYMBIAN
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
