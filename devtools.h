#ifndef DEVTOOLS_H
#define DEVTOOLS_H

#include "dyalog.h"
#include <QCoreApplication>

class QWidget;

/** Display developer tools. */
class DevTools: public Dyalog
{
    Q_OBJECT

public:
    explicit DevTools(QWidget *parent = 0);

public slots:
    void onClear();
    void onLevelButtonClicked(int level);
    void onTraceToFileToggled(bool enable);
};

#endif // DEVTOOLS_H
