#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

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
};

#endif // PLATFORM_H
