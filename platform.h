#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>

class Platform
{
public:
    static QString dbPath();
    static QString icon(const QString &name);
    static void restart(char *argv[]);
};

#endif // PLATFORM_H
