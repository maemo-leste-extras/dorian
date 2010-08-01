#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#include <QtGui/QApplication>

#include "mainwindow.h"
#include "trace.h"
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Dorian");
    a.setApplicationVersion("0.0.1");
    a.setOrganizationDomain("pipacs.com");
    a.setOrganizationName("Pipacs");

    Trace::level = (QtMsgType)Settings::instance()->
        value("tracelevel", (int)QtWarningMsg).toInt();
    qInstallMsgHandler(Trace::messageHandler);

    MainWindow w;
#ifdef Q_WS_S60
    w.showMaximized();
#else
    w.show();
#endif

    int ret = a.exec();
    if (ret == 1000) {
#ifdef Q_OS_UNIX
        extern char **environ;
        execve(argv[0], argv, environ);
#endif
    }
    return ret;
}
