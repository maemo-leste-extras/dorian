QT += webkit xml sql network

INCLUDEPATH += $$PWD \
    $$PWD/model \
    $$PWD/widgets

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    bookview.cpp \
    model/unzip/unzip.c \
    model/unzip/ioapi.c \
    model/extractzip.cpp \
    model/library.cpp \
    model/book.cpp \
    librarydialog.cpp \
    devtools.cpp \
    infodialog.cpp \
    widgets/translucentbutton.cpp \
    settingswindow.cpp \
    model/settings.cpp \
    bookmarksdialog.cpp \
    model/sortedlibrary.cpp \
    bookmarkinfodialog.cpp \
    widgets/dyalog.cpp \
    chaptersdialog.cpp \
    fullscreenwindow.cpp \
    trace.cpp \
    widgets/toolbuttonbox.cpp \
    model/bookfinder.cpp \
    widgets/listwindow.cpp \
    widgets/progress.cpp \
    adopterwindow.cpp \
    platform.cpp \
    model/bookdb.cpp \
    searchdialog.cpp \
    search.cpp \
    widgets/flickable.cpp \
    searchresultsdialog.cpp \
    searchresultinfodialog.cpp \
    widgets/progressdialog.cpp \
    widgets/splash.cpp \
    widgets/mainbase.cpp

HEADERS += \
    mainwindow.h \
    bookview.h \
    model/opshandler.h \
    model/unzip/unzip.h \
    model/unzip/ioapi.h \
    model/extractzip.h \
    model/library.h \
    model/book.h \
    librarydialog.h \
    devtools.h \
    infodialog.h \
    widgets/translucentbutton.h \
    settingswindow.h \
    model/settings.h \
    bookmarksdialog.h \
    model/xmlerrorhandler.h \
    model/containerhandler.h \
    model/sortedlibrary.h \
    model/ncxhandler.h \
    bookmarkinfodialog.h \
    widgets/dyalog.h \
    chaptersdialog.h \
    fullscreenwindow.h \
    trace.h \
    widgets/toolbuttonbox.h \
    model/bookfinder.h \
    widgets/listwindow.h \
    widgets/progress.h \
    adopterwindow.h \
    model/xmlhandler.h \
    platform.h \
    model/bookdb.h \
    searchdialog.h \
    search.h \
    widgets/flickable.h \
    searchresultsdialog.h \
    searchresultinfodialog.h \
    widgets/progressdialog.h \
    widgets/splash.h \
    widgets/mainbase.h

RESOURCES += \
    dorian.qrc

OTHER_FILES += \
    TODO.txt \
    pkg/acknowledgements.txt \
    pkg/maemo/postinst \
    pkg/maemo/dorian.desktop \
    pkg/maemo/control \
    pkg/maemo/changelog \
    pkg/maemo/build.sh \
    styles/night.css \
    pkg/changelog \
    pkg/maemo/build-scratchbox.sh \
    styles/sand.css \
    styles/default.css \
    pkg/version.txt \
    styles/sand.js \
    styles/night.js \
    styles/default.js \
    styles/day.js \
    www/index.html \
    pkg/maemo/autobuild.sh \
    pkg/maemo/autobuild-scratchbox.sh \
    LICENSE.txt \
    pkg/symbian/book.svg \
    pkg/symbian/clean.bat

DEFINES += \
    USE_FILE32API

!symbian {
    DEFINES += DORIAN_TEST_MODEL
    include(model/modeltest/modeltest.pri)
}

unix {
    symbian {
    } else {
        LIBS += -lz
    }
}

win32 {
    DEFINES += ZLIB_WINAPI
    INCLUDEPATH += $$PWD/model/zlib
    LIBS += pkg/win32/zlibstat.lib
}

symbian {
    TARGET = Dorian
    TARGET.UID3 = 0xA89FC85B
    TARGET.CAPABILITY = UserEnvironment NetworkServices ReadUserData \
        WriteUserData
    TARGET.EPOCHEAPSIZE = 0x080000 0x4000000
    ICON = $$PWD/pkg/symbian/book.svg

    # FIXME: Package header for Qt 4.6.2:
    packageheader = \
        "$${LITERAL_HASH}{\"Dorian\"},(0xA89FC85B),0,4,4" \
        "[0x101F7961],0,0,0,{\"S60ProductID\"}" \
        "[0x102032BE],0,0,0,{\"S60ProductID\"}" \
        "[0x102752AE],0,0,0,{\"S60ProductID\"}" \
        "[0x1028315F],0,0,0,{\"S60ProductID\"}"
    default_deployment.pkg_prerules = packageheader

    # FIXME: Package header for Qt 4.6.3 or later:
    # packageheader = "$${LITERAL_HASH}{\"Dorian\"},(0xA89FC85B),0,4,4"
    # my_deployment.pkg_prerules = packageheader
    # DEPLOYMENT += my_deployment

    INCLUDEPATH += $(QTDIR)/src/3rdparty/zlib
    HEADERS += \
        widgets/flickcharm.h \
        widgets/mediakeysobserver.h
    SOURCES += \
        widgets/flickcharm.cpp \
        widgets/mediakeysobserver.cpp
    LIBS += -lremconinterfacebase -lremconcoreapi
}

maemo5 {
    QT += maemo5 dbus
    isEmpty(PREFIX) {
        PREFIX = /usr
    }
    BINDIR = $$PREFIX/bin
    DATADIR =$$PREFIX/share
    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

    # For "make install"

    INSTALLS += target desktop icon48 iconscalable

    target.path = $$BINDIR

    desktop.path = $$DATADIR/applications/hildon
    desktop.files += pkg/maemo/dorian.desktop

    icon48.path = $$DATADIR/icons/hicolor/48x48/hildon
    icon48.files += pkg/maemo/icon-48/dorian.png

    iconscalable.path = $$DATADIR/icons/hicolor/scalable/hildon
    iconscalable.files += pkg/maemo/icon-scalable/dorian.png
}
