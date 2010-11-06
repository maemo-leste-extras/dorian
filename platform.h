#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class QWidget;

/** Platform abstractions. */
class Platform
{
public:
    static QString dbPath();
    static QString icon(const QString &name);
    static void restart(char *argv[]);
    static QString version();
    static QString downloadDir();
    static QString defaultFont();
    static void information(const QString &label, QWidget *parent = 0);
    static void showBusy(QWidget *w, bool isBusy);
};

#endif // PLATFORM_H
