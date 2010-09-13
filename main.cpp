#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
#include <unistd.h>
#endif

#include <QtGui/QApplication>

#include "mainwindow.h"
#include "trace.h"
#include "settings.h"

static const char *DORIAN_VERSION =
#include "pkg/version.txt"
;

static const QtMsgType DORIAN_DEFAULT_TRACE_LEVEL =
#ifdef Q_OS_SYMBIAN
        QtDebugMsg
#else
        QtWarningMsg
#endif
        ;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Dorian");
    a.setApplicationVersion(DORIAN_VERSION);
    a.setOrganizationDomain("pipacs.com");
    a.setOrganizationName("Pipacs");

    Trace::level = (QtMsgType)Settings::instance()->
        value("tracelevel", (int)DORIAN_DEFAULT_TRACE_LEVEL).toInt();
    qInstallMsgHandler(Trace::messageHandler);

    MainWindow w;
    w.show();

#if 0 // FIXME #ifdef Q_OS_SYMBIAN
    // Remove context menu from all widgets
    foreach (QWidget *w, QApplication::allWidgets()) {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
#endif // Q_OS_SYMBIAN

    int ret = a.exec();
    if (ret == 1000) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
        extern char **environ;
        execve(argv[0], argv, environ);
#endif
    }
    return ret;
}
