#include <QtGlobal>
#include <QDir>

#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
#   include <unistd.h>
#endif

#include "platform.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_SYMBIAN)
#   define DORIAN_BASE "dorian"
#else
#   define DORIAN_BASE ".dorian"
#endif

#if defined(Q_WS_MAC)
#   define DORIAN_ICON_PREFIX ":/icons/mac/"
#elif defined(Q_OS_SYMBIAN)
#   define DORIAN_ICON_PREFIX ":/icons/symbian/"
#else
#   define DORIAN_ICON_PREFIX ":/icons/"
#endif

static const char *DORIAN_VERSION =
#include "pkg/version.txt"
;

QString Platform::dbPath()
{
    QString base(QDir::home().absoluteFilePath(DORIAN_BASE));
    return QDir(base).absoluteFilePath("books.db");
}

QString Platform::icon(const QString &name)
{
    QString iconName = QString(DORIAN_ICON_PREFIX) + name + ".png";
    if (QFile(iconName).exists()) {
        return iconName;
    } else {
        return QString(":/icons/") + name + ".png";
    }
}

void Platform::restart(char *argv[])
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    extern char **environ;
    execve(argv[0], argv, environ);
#else
    Q_UNUSED(argv);
#endif
}

QString Platform::version()
{
    return QString(DORIAN_VERSION);
}

QString Platform::downloadDir()
{
    return QDir::home().absoluteFilePath("Books");
}

QString Platform::defaultFont()
{
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_X11)
    return QString("Serif");
#elif defined(Q_WS_MAC)
    return QString("Hoefler Text");
#elif defined Q_WS_S60
    return QString("Nokia Sans S60");
#else
    return QString("Times New Roman");
#endif
}
