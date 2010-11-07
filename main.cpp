#include <QtGui/QApplication>

#include "mainwindow.h"
#include "trace.h"
#include "settings.h"
#include "library.h"
#include "settings.h"
#include "bookdb.h"
#include "search.h"
#include "platform.h"

#ifdef Q_OS_SYMBIAN
#   include "mediakeysobserver.h"
#endif

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
    int ret;

    a.setApplicationName("Dorian");
    a.setApplicationVersion(DORIAN_VERSION);
    a.setOrganizationDomain("pipacs.com");
    a.setOrganizationName("Pipacs");

    Trace::level = (QtMsgType)Settings::instance()->
        value("tracelevel", (int)DORIAN_DEFAULT_TRACE_LEVEL).toInt();
    qInstallMsgHandler(Trace::messageHandler);

    {
        MainWindow w;
        ret = a.exec();
    }

    // Release singletons
    Library::close();
    BookDb::close();
    Settings::close();
    Search::close();
#ifdef Q_OS_SYMBIAN
    MediaKeysObserver::close();
#endif

    // Re-start application if event loop exit code was 1000
    if (ret == 1000) {
        Platform::restart(argv);
    }
    return ret;
}
