#ifndef DEVTOOLS_H
#define DEVTOOLS_H

#include <QtGui>

class DevTools: public QDialog
{
    Q_OBJECT

public:
    explicit DevTools(QWidget *parent = 0);

public slots:
    void onClear();
};

#endif // DEVTOOLS_H
