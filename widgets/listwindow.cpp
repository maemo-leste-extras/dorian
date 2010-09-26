#include <QtGui>

#include "listwindow.h"
#include "trace.h"
#include "listview.h"

ListWindow::ListWindow(QWidget *parent): QMainWindow(parent), list(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    popup = new QMenu(this);

    QScrollArea *scroller = new QScrollArea(this);
    setCentralWidget(scroller);
    scroller->setProperty("FingerScrollable", true);
    // scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setFrameStyle(QFrame::NoFrame);
    scroller->show();

    QWidget *content = new QWidget(scroller);
    contentLayout = new QVBoxLayout(content);
    contentLayout->setMargin(0);
    content->setLayout(contentLayout);
    content->show();

    scroller->setWidget(content);
    scroller->setWidgetResizable(true);
#else
    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    contentLayout = new QHBoxLayout();
    frame->setLayout(contentLayout);
#ifdef Q_OS_SYMBIAN
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#else
    buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    contentLayout->addWidget(buttonBox);
#endif // Q_OS_SYMBIAN
#endif // Q_WS_MAEMO_5
}

void ListWindow::addList(ListView *listView)
{
    Trace t("ListWindow::addList");
    list = listView;
#ifdef Q_WS_MAEMO_5
    list->installEventFilter(this);
    list->setMinimumHeight(list->contentsHeight());
    contentLayout->addWidget(list);
    connect(list->model(),
            SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(onModelChanged()));
    connect(list->model(),
            SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(onModelChanged()));
#else
    contentLayout->insertWidget(0, list);
#endif
    connect(list->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this,
      SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void ListWindow::addAction(const QString &title, QObject *receiver,
                           const char *slot, const QString &iconPath,
                           QDialogButtonBox::ButtonRole role)
{
    Trace t("ListWindow::addAction");
#ifdef Q_WS_MAEMO_5
    Q_UNUSED(role);
    QPushButton *button = new QPushButton(QIcon(iconPath), title, this);
    contentLayout->addWidget(button);
    connect(button, SIGNAL(clicked()), receiver, slot);
#elif defined(Q_OS_SYMBIAN)
    Q_UNUSED(role);
    QAction *action = new QAction(title, this);
    connect(action, SIGNAL(triggered()), receiver, slot);
    action->setSoftKeyRole(QAction::PositiveSoftKey);
    menuBar()->addAction(action);
#else
    Q_UNUSED(iconPath);
    QPushButton *button = buttonBox->addButton(title, role);
    connect(button, SIGNAL(clicked()), receiver, slot);
#endif // Q_WS_MAEMO_5
}

void ListWindow::addItemAction(const QString &title, QObject *receiver,
                               const char *slot)
{
    Trace t("ListWindow::addItemAction");
#ifdef Q_WS_MAEMO
    popup->addAction(title, receiver, slot);
#elif defined Q_OS_SYMBIAN
    QAction *action = new QAction(title, this);
    connect(action, SIGNAL(triggered()), receiver, slot);
    action->setSoftKeyRole(QAction::PositiveSoftKey);
    menuBar()->addAction(action);
    // FIXME: Add action to the list of item specific actions
#else
    QPushButton *button =
            buttonBox->addButton(title, QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), receiver, slot);
    itemButtons.append(button);
    activateItemButtons();
#endif // Q_WS_MAEMO_5
}

#ifdef Q_WS_MAEMO_5

void ListWindow::closeEvent(QCloseEvent *event)
{
    // Work around Maemo/Qt but: Menu items are not removed on close
    menuBar()->clear();
    event->accept();
}

#endif // Q_WS_MAEMO_5

void ListWindow::onSelectionChanged(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);
#ifndef Q_WS_MAEMO_5
    activateItemButtons();
#endif
}

#ifndef Q_WS_MAEMO_5

void ListWindow::activateItemButtons()
{
    bool enable = false;
    if (list) {
        enable = list->selectionModel()->hasSelection();
    }
    foreach (QPushButton *button, itemButtons) {
        button->setEnabled(enable);
    }
}

#endif // ! Q_WS_MAEMO_5

#ifdef Q_WS_MAEMO_5

bool ListWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        qDebug() << "ListWindow::eventFiler: Received QEvent::ContextMenu";
        if (popup->actions().size()) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*> (event);
            QPoint pos = mouseEvent->globalPos();
            pos.setX(pos.x() - 150);
            if (pos.x() < 0) {
                pos.setX(0);
            }
            popup->exec(pos);
        }
        return true;
    } else {
        return QObject::eventFilter(obj, event);
    }
}

void ListWindow::onModelChanged()
{
    qDebug() << "ListWindow::onModelChanged";
    list->setMinimumHeight(list->contentsHeight());
}

#endif // Q_WS_MAEMO_5

#ifdef Q_OS_SYMBIAN

void ListWindow::show()
{
    foreach (QWidget *w, QApplication::allWidgets()) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
    showMaximized();
}

#endif // Q_OS_SYMBIAN
