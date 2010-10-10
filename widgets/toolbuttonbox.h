#ifndef TOOLBUTTONBOX_H
#define TOOLBUTTONBOX_H

#include <QFrame>

class QString;
class QButtonGroup;

/** Frame with tool buttons. */
class ToolButtonBox: public QFrame
{
    Q_OBJECT

public:
    explicit ToolButtonBox(QWidget *parent = 0);
    void addButton(int id, const QString &title, const QString &icon = "");
    void addStretch();
    void toggle(int id);
    int checkedId() const;

signals:
    void buttonClicked(int id);

public slots:

protected:
    QButtonGroup *group;
};

#endif // TOOLBUTTONBOX_H
