#include <QApplication>
#include <QtGlobal>

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
    QApplication app(argc, argv);
    app.setApplicationName("Dorian");
    app.setApplicationVersion(DORIAN_VERSION);
    app.setOrganizationDomain("pipacs.com");
    app.setOrganizationName("Pipacs");

    // Initialize tracing
    Settings *settings = Settings::instance();
    //Trace::level = (QtMsgType)settings->
    //    value("tracelevel", (int)DORIAN_DEFAULT_TRACE_LEVEL).toInt();
    //Trace::setFileName(settings->value("tracefilename").toString());
    //qInstallMsgHandler(Trace::messageHandler);

    // Show splash screen
    Splash splash;
    splash.show();
    app.processEvents();

    // Initialize main window
    MainWindow *mainWindow = new MainWindow();
    mainWindow->initialize();

    // Hide splash screen
    splash.finish(mainWindow);

    // Apply settings (orientation, style etc.)
    settings->apply();

    // Run event loop, re-start application if event loop exit code was 1000
    ret = app.exec();
    if (ret == 1000) {
        Platform::instance()->restart(argv);
    }

    // Release singletons
    delete mainWindow;
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
