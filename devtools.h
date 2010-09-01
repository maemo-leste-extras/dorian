#ifndef DEVTOOLS_H
#define DEVTOOLS_H

#include <QtGui>

/** Display developer tools. */
class DevTools: public QDialog
{
    Q_OBJECT

public:
    explicit DevTools(QWidget *parent = 0);

public slots:
    void onClear();
    void onLevelButtonClicked(int level);
};

#endif // DEVTOOLS_H
