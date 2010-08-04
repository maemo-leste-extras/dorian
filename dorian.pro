QT += webkit xml

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
    widgets/dialog.cpp \
    chaptersdialog.cpp \
    widgets/fullscreenwindow.cpp \
    trace.cpp \
    widgets/toolbuttonbox.cpp \
    model/bookfinder.cpp \
    widgets/listwindow.cpp \
    foldersdialog.cpp

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
    widgets/dialog.h \
    chaptersdialog.h \
    widgets/fullscreenwindow.h \
    trace.h \
    widgets/toolbuttonbox.h \
    model/bookfinder.h \
    widgets/listwindow.h \
    foldersdialog.h

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
    www/index.html

DEFINES += \
    USE_FILE32API \
    DORIAN_TEST_MODEL

include(model/modeltest/modeltest.pri)

unix {
    LIBS += -lz
}
windows, symbian {
    # FIXME: Build zlib, too
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
