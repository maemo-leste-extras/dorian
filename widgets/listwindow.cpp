#include <QtGui>
#include <QListWidget>
#include <QAbstractItemModel>

#include "listwindow.h"
#include "trace.h"
#include "platform.h"

#ifdef Q_OS_SYMBIAN
#include "flickcharm.h"
#endif

ListWindow::ListWindow(const QString &noItems_, QWidget *parent):
        QMainWindow(parent), mModel(0), noItems(noItems_)
{
#if defined(Q_WS_MAEMO_5)
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif
    setAttribute(Qt::WA_DeleteOnClose);

    list = new QListWidget(this);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
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

    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
}

void ListWindow::populateList()
{
    TRACE;

    list->clear();
    list->setIconSize(QSize(48, 48)); // FIXME
    list->setUniformItemSizes(true);
    if (mModel && mModel->rowCount()) {
        for (int i = 0; i < mModel->rowCount(); i++) {
            QModelIndex index = mModel->index(i, 0);
            QString text = mModel->data(index, Qt::DisplayRole).toString();
            QVariant imageData = mModel->data(index, Qt::DecorationRole);
            QIcon icon(QPixmap::fromImage(imageData.value<QImage>()));
            (void)new QListWidgetItem(icon, text, list);
        }
    } else {
        QListWidgetItem *item = new QListWidgetItem(noItems);
        item->setFlags(Qt::NoItemFlags);
        list->addItem(item);
    }
    for (int i = 0; i < buttons.count(); i++) {
        insertButton(i, buttons[i]);
    }
}

void ListWindow::insertButton(int row, const Button &b)
{
    QPushButton *pushButton = new QPushButton(
        QIcon(Platform::instance()->icon(b.iconName)), b.title, this);
    connect(pushButton, SIGNAL(clicked()), b.receiver, b.slot);
    QListWidgetItem *item = new QListWidgetItem();
    item->setFlags(Qt::NoItemFlags);
    list->insertItem(row, item);
    list->setItemWidget(item, pushButton);
}

void ListWindow::setModel(QAbstractItemModel *aModel)
{
    TRACE;
    mModel = aModel;
    populateList();
    if (mModel) {
        connect(mModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
                this, SLOT(populateList()));
        connect(mModel, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(populateList()));
        connect(mModel, SIGNAL(rowsInserted(QModelIndex, int, int)),
                this, SLOT(populateList()));
    }
}

QAbstractItemModel *ListWindow::model() const
{
    return mModel;
}

void ListWindow::addButton(const QString &title, QObject *receiver,
                           const char *slot, const QString &iconName)
{
    TRACE;

    Button b;
    b.title = title;
    b.receiver = receiver;
    b.slot = slot;
    b.iconName = iconName;

    insertButton(buttons.length(), b);
    buttons.append(b);
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

void ListWindow::onItemActivated(const QModelIndex &index)
{
    TRACE;
    int row = index.row() - buttons.count();
    qDebug() << "Activated" << index.row() << ", emit activated(" << row << ")";
    emit activated(mModel->index(row, 0));
}

void ListWindow::setCurrentItem(const QModelIndex &item)
{
    int index = item.row();
    list->setCurrentItem(list->item(index + buttons.count()));
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
