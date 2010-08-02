#include <QtGui>

#include "toolbuttonbox.h"

ToolButtonBox::ToolButtonBox(QWidget *parent): QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    group = new QButtonGroup(this);
    group->setExclusive(true);
    connect(group, SIGNAL(buttonClicked(int)), this, SIGNAL(buttonClicked(int)));
}

void ToolButtonBox::addButton(int id, const QString &title, const QString &icon)
{
    QToolButton *button = new QToolButton(this);
    if (icon == "") {
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    } else {
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(81, 81));
        button->setIcon(QIcon(icon));
    }
    button->setText(title);
    button->setCheckable(true);
    layout()->addWidget(button);
    group->addButton(button, id);
}

void ToolButtonBox::addStretch()
{
    qobject_cast<QHBoxLayout *>(layout())->addStretch();
}

void ToolButtonBox::toggle(int id)
{
    group->button(id)->toggle();
}

int ToolButtonBox::checkedId() const
{
    return group->checkedId();
}
