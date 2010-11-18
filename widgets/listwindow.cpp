#include <QtGui>
#include <QListWidget>
#include <QAbstractItemModel>

#include "listwindow.h"
#include "trace.h"
#include "platform.h"

#ifdef Q_OS_SYMBIAN
#include "flickcharm.h"
#endif

ListWindow::ListWindow(QWidget *parent): QMainWindow(parent), model(0)
{
#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    list = new QListWidget(this);
    populateList();
    setCentralWidget(list);

#ifdef Q_OS_SYMBIAN
    charm = new FlickCharm(this);
    charm->activateOn(list);
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#endif // Q_OS_SYMBIAN

#ifdef Q_WS_MAC
    // FIXME
    // addAction(tr("Close"), this, SLOT(close()), QString(),
    //           QDialogButtonBox::RejectRole);
#endif // Q_WS_MAC

    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActicvated(const QModelIndex &)));
}

void ListWindow::populateList()
{
    TRACE;

    list->clear();
    if (model) {
        for (int i = 0; i < model->rowCount(); i++) {
            QModelIndex index = model->index(i, 0);
            QString text = model->data(index, Qt::DisplayRole).toString();
            QIcon icon;
            QVariant iconData = model->data(index, Qt::DecorationRole);
            if (iconData.canConvert<QIcon>()) {
                icon = iconData.value<QIcon>();
            }
            (void)new QListWidgetItem(icon, text, list);
        }
    }
    for (int i = 0; i < buttons.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem();
        list->insertItem(i, item);
        list->setItemWidget(item, buttons[i]);
    }
}

void ListWindow::setModel(QAbstractItemModel *model_)
{
    model = model_;
    populateList();
    if (model) {
        connect(model,
                SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                this, SLOT(populateList()));
    }
}

void ListWindow::addButton(const QString &title, QObject *receiver,
                           const char *slot, const QString &iconName)
{
    TRACE;

    QPushButton *button = new QPushButton(QIcon(Platform::instance()->
                                                icon(iconName)), title, this);
    connect(button, SIGNAL(clicked()), receiver, slot);
    buttons.append(button);

    int pos = buttons.length() - 1;
    QListWidgetItem *item = new QListWidgetItem();
    list->insertItem(pos, item);
    list->setItemWidget(item, button);
}

QAction *ListWindow::addMenuAction(const QString &title, QObject *receiver,
                                   const char *slot)
{
    TRACE;
    QAction *action = 0;
#if defined(Q_WS_MAEMO_5)
    action = menuBar()->addAction(title);
    connect(action, SIGNAL(triggered()), receiver, slot);
#elif defined(Q_OS_SYMBIAN)
    action = new QAction(title, this);
    connect(action, SIGNAL(triggered()), receiver, slot);
    action->setSoftKeyRole(QAction::PositiveSoftKey);
    menuBar()->addAction(action);
#else
    Q_UNUSED(title);
    Q_UNUSED(receiver);
    Q_UNUSED(slot);
    action = new QAction(this);
#endif
    action->setCheckable(true);
    return action;
}

void ListWindow::onItemActivated(const QModelIndex &)
{
    TRACE;
    // FIXME
}

#ifdef Q_WS_MAEMO_5

void ListWindow::closeEvent(QCloseEvent *event)
{
    // Work around Maemo/Qt bug: Menu items are not removed on close
    menuBar()->clear();
    // FIXME: Is this needed? event->accept();
    QMainWindow::closeEvent(event);
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
