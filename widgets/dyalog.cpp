#include <QtGui>

#include "dyalog.h"

Dyalog::Dyalog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                    Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)
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
        buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
        boxLayout = new QVBoxLayout(this);
    } else {
        buttonBox = new QDialogButtonBox(Qt::Vertical, this);
        boxLayout = new QHBoxLayout(this);
    }
    boxLayout->addWidget(scroller);
    boxLayout->addWidget(buttonBox);
    setLayout(boxLayout);

    scroller->setWidget(content);
    content->show();
    scroller->setWidgetResizable(true);
}

void Dyalog::addWidget(QWidget *widget)
{
    contentLayout->addWidget(widget);
}

void Dyalog::addStretch(int stretch)
{
    contentLayout->addStretch(stretch);
}

void Dyalog::addButton(QPushButton *button, QDialogButtonBox::ButtonRole role)
{
    buttonBox->addButton(button, role);
}

QPushButton *Dyalog::addButton(QDialogButtonBox::StandardButton button)
{
    return buttonBox->addButton(button);
}