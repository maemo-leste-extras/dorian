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

#define DORIAN_LOG "dorian.txt"

#ifdef Q_WS_MAEMO_5
#   include <QtMaemo5/QMaemo5InformationBox>
#else
#   include <QMessageBox>
#endif

static Platform *theInstance;

Platform *Platform::instance()
{
    if (!theInstance) {
        theInstance = new Platform();
    }
    return theInstance;
}

void Platform::close()
{
    delete theInstance;
    theInstance = 0;
}

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

void Platform::information(const QString &label, QWidget *parent)
{
#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::information(parent, label,
                                       QMaemo5InformationBox::DefaultTimeout);
#else
    (void)QMessageBox::information(parent, QObject::tr("Dorian"), label,
                                   QMessageBox::Ok);
#endif
}

void Platform::showBusy(QWidget *w, bool isBusy)
{
#ifdef Q_WS_MAEMO_5
    w->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, isBusy);
#else
    Q_UNUSED(w);
    Q_UNUSED(isBusy);
#endif
}

QString Platform::traceFileName()
{
    return QDir::home().absoluteFilePath(DORIAN_LOG);
}

int Platform::defaultZoom()
{
    return 150;
}

QString Platform::defaultOrientation()
{
#ifdef Q_OS_SYMBIAN
    return QString("portrait");
#else
    return QString("landscape");
#endif
}
