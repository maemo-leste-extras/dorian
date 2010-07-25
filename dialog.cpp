#include <QtGui>

#include "dialog.h"

Dialog::Dialog(QWidget *parent) :
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

    buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(scroller);
    horizontalLayout->addWidget(buttonBox);
    setLayout(horizontalLayout);

    scroller->setWidget(content);
    content->show();
    scroller->setWidgetResizable(true);
}

void Dialog::addWidget(QWidget *widget)
{
    contentLayout->addWidget(widget);
}

void Dialog::addStretch(int stretch)
{
    contentLayout->addStretch(stretch);
}

void Dialog::addButton(QPushButton *button, QDialogButtonBox::ButtonRole role)
{
    buttonBox->addButton(button, role);
}
