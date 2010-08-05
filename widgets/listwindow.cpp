#include <QtGui>

#include "listwindow.h"
#include "trace.h"

ListWindow::ListWindow(QWidget *parent): QMainWindow(parent), list(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    frameLayout = new QHBoxLayout(frame);
    frame->setLayout(frameLayout);

#ifndef Q_WS_MAEMO_5
    buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    frameLayout->addWidget(buttonBox);
#endif
}

void ListWindow::addList(QListView *listView)
{
    list = listView;
    frameLayout->insertWidget(0, list);
    connect(list->selectionModel(),
      SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
      this,
      SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection&)));
}

void ListWindow::addAction(const QString &title, QObject *receiver,
                           const char *slot, QDialogButtonBox::ButtonRole role)
{
#ifndef Q_WS_MAEMO_5
    QPushButton *button = buttonBox->addButton(title, role);
    connect(button, SIGNAL(clicked()), receiver, slot);
#else
    Q_UNUSED(role);
    QAction *action = menuBar()->addAction(title);
    connect(action, SIGNAL(triggered()), receiver, slot);
#endif // ! Q_WS_MAEMO_5
}

void ListWindow::addItemAction(const QString &title, QObject *receiver,
                               const char *slot)
{
#ifndef Q_WS_MAEMO_5
    QPushButton *button =
            buttonBox->addButton(title, QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()), receiver, slot);
    itemButtons.append(button);
    activateItemButtons();
#else
    // FIXME
#endif // ! Q_WS_MAEMO_5
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
