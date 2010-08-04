#include <QtGui>

#include "listwindow.h"
#include "trace.h"

ListWindow::ListWindow(QWidget *parent): QMainWindow(parent)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    QFrame *frame = new QFrame(this);
    setCentralWidget(frame);
    layout = new QHBoxLayout(frame);
    frame->setLayout(layout);

#ifndef Q_WS_MAEMO_5
    buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    layout->addWidget(buttonBox);
#endif
}

void ListWindow::addList(QListView *list)
{
    layout->insertWidget(0, list);
}

void ListWindow::addAction(const QString &title, QObject *receiver,
                           const char *slot, QDialogButtonBox::ButtonRole role)
{
#ifndef Q_WS_MAEMO_5
    QPushButton *button = new QPushButton(title, this);
    QList<QAction *> actions = button->actions();
    Trace::trace(QString("ListWindow::addAction: Button has %1 default action(s)").arg(actions.length()));
    buttonBox->addButton(button, role);
    connect(button, SIGNAL(clicked()), receiver, slot);
#else
    Q_UNUSED(role);
    QAction *action = menuBar()->addAction(title);
    connect(action, SIGNAL(triggered()), receiver, slot);
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
