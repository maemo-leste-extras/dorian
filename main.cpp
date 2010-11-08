#include <QtGui/QApplication>

#include "mainwindow.h"
#include "trace.h"
#include "settings.h"
#include "library.h"
#include "settings.h"
#include "bookdb.h"
#include "search.h"
#include "platform.h"
#include "splash.h"

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
    int ret;

    // Set up application
    QApplication a(argc, argv);
    a.setApplicationName("Dorian");
    a.setApplicationVersion(DORIAN_VERSION);
    a.setOrganizationDomain("pipacs.com");
    a.setOrganizationName("Pipacs");

    // Initialize tracing
    Settings *settings = Settings::instance();
    Trace::level = (QtMsgType)settings->
        value("tracelevel", (int)DORIAN_DEFAULT_TRACE_LEVEL).toInt();
    Trace::setFileName(settings->value("tracefilename").toString());
    qInstallMsgHandler(Trace::messageHandler);

#ifdef Q_OS_SYMBIAN
    // Show splash screen
    Splash *splash = new Splash();
    splash->showFullScreen();
    a.processEvents();
#endif

    // Create main window, run event loop
    {
        MainWindow w;
#ifdef Q_OS_SYMBIAN
        splash->close();
        splash->deleteLater();
#endif
        ret = a.exec();
    }

    // Re-start application if event loop exit code was 1000
    if (ret == 1000) {
        Platform::instance()->restart(argv);
    }

    // Release singletons
    Library::close();
    BookDb::close();
    Settings::close();
    Search::close();
    Platform::close();
#ifdef Q_OS_SYMBIAN
    MediaKeysObserver::close();
#endif

    return ret;
}
