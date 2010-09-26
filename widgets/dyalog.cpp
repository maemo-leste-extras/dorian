#include <QtGui>

#include "dyalog.h"

Dyalog::Dyalog(QWidget *parent, bool showButtons_):
    QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                    Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
    showButtons(showButtons_)
{
    scroller = new QScrollArea(this);

#ifdef Q_WS_MAEMO_5
    scroller->setProperty("FingerScrollable", true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setFrameStyle(QFrame::NoFrame);

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

#ifdef Q_OS_SYMBIAN
    QAction *closeAction = new QAction(tr("Back"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(reject()));
    addAction(closeAction);
    menu = 0;
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
    if (!showButtons) {
        return;
    }
#ifdef Q_OS_SYMBIAN
    Q_UNUSED(role);
    if (!menu) {
        QAction *menuAction = new QAction(tr("Options"), this);
        menuAction->setSoftKeyRole(QAction::PositiveSoftKey);
        menu = new QMenu(this);
        menuAction->setMenu(menu);
    }
    QAction *action = new QAction(label, this);
    connect(action, SIGNAL(triggered()), receiver, slot);
    menu->addAction(action);
#else
    QPushButton *button = new QPushButton(label, this);
    connect(button, SIGNAL(clicked()), receiver, slot);
    buttonBox->addButton(button, role);
#endif // Q_OS_SYMBIAN
}

#ifdef Q_OS_SYMBIAN

void Dyalog::show()
{
    showMaximized();
}

int Dyalog::exec()
{
    showMaximized();
    return QDialog::exec();
}

#endif // Q_OS_SYMBIAN
