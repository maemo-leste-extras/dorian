#include <QtGui>

#include "dyalog.h"
#include "trace.h"

#ifdef Q_OS_SYMBIAN
#include "flickcharm.h"
#endif

Dyalog::Dyalog(QWidget *parent, bool showButtons_):
    QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                    Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    showButtons(showButtons_)
{
    setAttribute(Qt::WA_DeleteOnClose);

    scroller = new QScrollArea(this);

#if defined(Q_WS_MAEMO_5)
    scroller->setProperty("FingerScrollable", true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#elif defined(Q_OS_SYMBIAN)
    FlickCharm *charm = new FlickCharm(this);
    charm->activateOn(scroller);
#else
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setFrameStyle(QFrame::NoFrame);
#if defined(Q_OS_SYMBIAN)
    setStyleSheet("QFrame {margin:0; border:0; padding:0}");
    setStyleSheet("QScrollArea {margin:0; border:0; padding:0}");
#endif

    content = new QWidget(scroller);
    contentLayout = new QVBoxLayout(content);
    contentLayout->setMargin(0);
    content->setLayout(contentLayout);

    QBoxLayout *boxLayout;
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    if (screenGeometry.width() < screenGeometry.height()) {
#ifndef Q_OS_SYMBIAN
        if (showButtons) {
            buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
        }
#endif
        boxLayout = new QVBoxLayout(this);
    } else {
#ifndef Q_OS_SYMBIAN
        if (showButtons) {
            buttonBox = new QDialogButtonBox(Qt::Vertical, this);
        }
#endif
        boxLayout = new QHBoxLayout(this);
    }
    boxLayout->addWidget(scroller);
#ifndef Q_OS_SYMBIAN
    if (showButtons) {
        boxLayout->addWidget(buttonBox);
    }
#endif
    setLayout(boxLayout);

    scroller->setWidget(content);
    content->show();
    scroller->setWidgetResizable(true);

#if defined(Q_OS_SYMBIAN)
    QAction *closeAction = new QAction(tr("Back"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(reject()));
    addAction(closeAction);
    leftSoftKey = 0;
    menuBar = 0;
#endif // Q_OS_SYMBIAN
}

void Dyalog::addWidget(QWidget *widget)
{
    contentLayout->addWidget(widget);
}

void Dyalog::addStretch(int stretch)
{
    contentLayout->addStretch(stretch);
}

void Dyalog::addButton(const QString &label, QObject *receiver,
                       const char *slot, QDialogButtonBox::ButtonRole role)
{
    TRACE;
    if (!showButtons) {
        qDebug() << "Ignored: showButtons is false";
        return;
    }
#ifdef Q_OS_SYMBIAN
    Q_UNUSED(role);
    if (!leftSoftKey) {
        // Add new action as left softkey
        leftSoftKey = new QAction(label, this);
        leftSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
        connect(leftSoftKey, SIGNAL(triggered()), receiver, slot);
        addAction(leftSoftKey);
    } else {
        if (!menuBar) {
            // Create menu bar
            menuBar = new QMenuBar(this);
            // Add previous LSK to menu bar
            leftSoftKey->setSoftKeyRole(QAction::NoSoftKey);
            menuBar->addAction(leftSoftKey);
        }
        // Add new action to menu bar
        QAction *action = new QAction(label, this);
        connect(action, SIGNAL(triggered()), receiver, slot);
        menuBar->addAction(action);
    }
#else
    QPushButton *button = new QPushButton(label, this);
    connect(button, SIGNAL(clicked()), receiver, slot);
    buttonBox->addButton(button, role);
#endif // Q_OS_SYMBIAN
}

#ifdef Q_OS_SYMBIAN

void Dyalog::show()
{
    foreach (QWidget *w, QApplication::allWidgets()) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
    showMaximized();
}

int Dyalog::exec()
{
    show();
    return QDialog::exec();
}

#endif // Q_OS_SYMBIAN
