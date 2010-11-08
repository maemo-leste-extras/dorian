#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class QWidget;

/** Platform abstractions. */
class Platform
{
public:
    static Platform *instance();
    static void close();

    QString dbPath();
    QString icon(const QString &name);
    void restart(char *argv[]);
    QString version();
    QString downloadDir();
    QString defaultFont();
    int defaultZoom();
    QString defaultOrientation();
    void information(const QString &label, QWidget *parent = 0);
    void showBusy(QWidget *w, bool isBusy);
    QString traceFileName();
};

#endif // PLATFORM_H
