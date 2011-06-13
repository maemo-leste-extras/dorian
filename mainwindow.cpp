#include <QtGui>
#include <QEvent>

#ifdef Q_WS_MAEMO_5
#   include <QtDBus>
#   include <QtGui/QX11Info>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <mce/mode-names.h>
#   include <mce/dbus-names.h>
#endif // Q_WS_MAEMO_5

#include "bookview.h"
#include "book.h"
#include "library.h"
#include "infodialog.h"
#include "librarydialog.h"
#include "devtools.h"
#include "mainwindow.h"
#include "settingswindow.h"
#include "bookmarksdialog.h"
#include "settings.h"
#include "chaptersdialog.h"
#include "fullscreenwindow.h"
#include "trace.h"
#include "bookfinder.h"
#include "progress.h"
#include "dyalog.h"
#include "translucentbutton.h"
#include "platform.h"
#include "progressdialog.h"
#include "sortedlibrary.h"

#ifdef DORIAN_TEST_MODEL
#   include "modeltest.h"
#endif

MainWindow::MainWindow(QWidget *parent):
    AdopterWindow(parent), view(0), preventBlankingTimer(-1)
{
    TRACE;
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif

    // Central widget. Must be an intermediate, because the book view widget
    // might be re-parented later
    QFrame *central = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setMargin(0);
    central->setLayout(layout);
    setCentralWidget(central);

    // Book view
    view = new BookView(this);

    // Tool bar actions

    chaptersAction = addToolBarAction(this, SLOT(showChapters()),
                                      "chapters", tr("Chapters"), true);
    bookmarksAction = addToolBarAction(this, SLOT(showBookmarks()),
                                       "bookmarks", tr("Bookmarks"), true);
    libraryAction = addToolBarAction(this, SLOT(showLibrary()),
                                     "library", tr("Library"), false);

#ifdef Q_WS_MAEMO_5
    settingsAction = menuBar()->addAction(tr("Settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    devToolsAction = menuBar()->addAction(tr("Developer"));
    connect(devToolsAction, SIGNAL(triggered()), this, SLOT(showDevTools()));
#else
    settingsAction = addToolBarAction(this, SLOT(showSettings()),
                                      "preferences-system", tr("Settings"));
    devToolsAction = addToolBarAction(this, SLOT(showDevTools()),
                                      "developer", tr("Developer"));
#endif

    rotateAction = addToolBarAction(this, SLOT(rotate()),
                                    "rotate", tr("Rotate"), true);
    addToolBarSpace();
    fullScreenAction = addToolBarAction(this, SLOT(showBig()),
        "view-fullscreen", tr("Full screen"), true);

#if defined(Q_WS_MAEMO_5)
    QAction *aboutAction = menuBar()->addAction(tr("About"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
#else
    addToolBarAction(this, SLOT(about()), "about", tr("About"));
#endif // defined(Q_WS_MAEMO_5)

#if defined(Q_OS_SYMBIAN)
    (void)addToolBarAction(this, SLOT(close()), "", tr("Exit"), false);
#endif // defined(Q_OS_SYMBIAN)

    // Decorations
    prev = new TranslucentButton("back", this);
    next = new TranslucentButton("forward", this);
    prog = new Progress(this);

    // Handle model changes
    connect(Library::instance(), SIGNAL(nowReadingChanged()),
            this, SLOT(onCurrentBookChanged()));

    // Load library, upgrade it if needed
    libraryProgress = new ProgressDialog(tr("Upgrading library"), this);
    Library *library = Library::instance();
    connect(library, SIGNAL(beginUpgrade(int)),
            this, SLOT(onBeginUpgrade(int)));
    connect(library, SIGNAL(upgrading(const QString &)),
            this, SLOT(onUpgrading(const QString &)));
    connect(library, SIGNAL(endUpgrade()), this, SLOT(onEndUpgrade()));

    // Handle loading book parts
    connect(view, SIGNAL(partLoadStart(int)), this, SLOT(onPartLoadStart()));
    connect(view, SIGNAL(partLoadEnd(int)), this, SLOT(onPartLoadEnd(int)));

    // Handle progress
    connect(view, SIGNAL(progress(qreal)), prog, SLOT(setProgress(qreal)));

    // Shadow window for full screen reading
    fullScreenWindow = new FullScreenWindow();
    connect(fullScreenWindow, SIGNAL(restore()), this, SLOT(showRegular()));

    // Handle settings changes
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));

    // Handle book view buttons
    connect(next, SIGNAL(triggered()), this, SLOT(goToNextPage()));
    connect(prev, SIGNAL(triggered()), this, SLOT(goToPreviousPage()));

#ifdef DORIAN_TEST_MODEL
    (void)new ModelTest(Library::instance(), this);
#endif
}

MainWindow::~MainWindow()
{
    delete fullScreenWindow;
}

void MainWindow::initialize()
{
    TRACE;
    Library *library = Library::instance();

    // Show in regular (non full-screen) mode
    showRegular();

    // Upgrade library if needed, then load it
    library->upgrade();
    library->load();

    // Load book on command line, or load last read book, or load default book
    if (QCoreApplication::arguments().size() == 2) {
        QString path = QCoreApplication::arguments()[1];
        library->add(path);
        QModelIndex index = library->find(path);
        if (index.isValid()) {
            library->setNowReading(index);
        }
    } else {
        QModelIndex index = library->nowReading();
        if (index.isValid()) {
            library->setNowReading(index);
        } else {
            if (!library->rowCount()) {
                library->add(":/books/2BR02B.epub");
            }
            SortedLibrary sorted;
            library->setNowReading(sorted.mapToSource(sorted.index(0, 0)));
        }
    }
}

void MainWindow::onCurrentBookChanged()
{
    TRACE;
    setCurrentBook(Library::instance()->nowReading());
}

void MainWindow::showRegular()
{
    TRACE;

    view->setLastBookmark();
    fullScreenWindow->leaveBookView();
    takeBookView(view, prog, prev, next);

    fullScreenWindow->hide();
    show();
    view->scheduleRestoreLastBookmark();
}

void MainWindow::showBig()
{
    TRACE;

    view->setLastBookmark();
    leaveBookView();
    fullScreenWindow->takeBookView(view, prog, prev, next);

#if defined(Q_WS_MAEMO_5)
    fullScreenWindow->raise();
#else
    hide();
#endif
    fullScreenWindow->showFullScreen();
    view->scheduleRestoreLastBookmark();
}

void MainWindow::setCurrentBook(const QModelIndex &current)
{
    mCurrent = current;
    Book *book = Library::instance()->book(current);
    view->setBook(book);
    setWindowTitle(book? book->shortName(): tr("Dorian"));
}

void MainWindow::showLibrary()
{
    (new LibraryDialog(this))->show();
}

void MainWindow::showSettings()
{
    (new SettingsWindow(this))->show();
}

void MainWindow::rotate()
{
    QString current = Settings::instance()->value("orientation",
        Platform::instance()->defaultOrientation()).toString();
    QString target = (current == "landscape")? "portrait": "landscape";
    view->setLastBookmark();
    Settings::instance()->setValue("orientation", target);
}

void MainWindow::showDevTools()
{
    (new DevTools(this))->exec();
}

void MainWindow::showBookmarks()
{
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        BookmarksDialog *bookmarks = new BookmarksDialog(book, this);
        connect(bookmarks, SIGNAL(addBookmark(const QString &)),
                this, SLOT(onAddBookmark(const QString &)));
        connect(bookmarks, SIGNAL(goToBookmark(int)),
                this, SLOT(onGoToBookmark(int)));
        bookmarks->show();
    }
}

void MainWindow::onSettingsChanged(const QString &key)
{
    TRACE;
    qDebug() << "Key" << key;

    if (key == "orientation") {
        QString value = Settings::instance()->value(key,
            Platform::instance()->defaultOrientation()).toString();
        qDebug() << "Value: orientation" << value;
        Platform::instance()->setOrientation(this, value);
        Platform::instance()->setOrientation(fullScreenWindow, value);
    }

#if defined(Q_WS_MAEMO_5)
    else if (key == "lightson") {
        bool enable = Settings::instance()->value(key, false).toBool();
        killTimer(preventBlankingTimer);
        if (enable) {
            preventBlankingTimer = startTimer(29 * 1000);
        }
    }
#endif // defined(Q_WS_MAEMO_5)
}

void MainWindow::onPartLoadStart()
{
    TRACE;
    Platform::instance()->showBusy(this, true);
}

void MainWindow::onPartLoadEnd(int index)
{
    TRACE;
    bool enablePrevious = false;
    bool enableNext = false;
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        if (index > 0) {
            enablePrevious = true;
        }
        if (index < (book->parts.size() - 1)) {
            enableNext = true;
        }
    }
    Platform::instance()->showBusy(this, false);
}

void MainWindow::onAddBookmark(const QString &note)
{
    TRACE;
    view->addBookmark(note);
    Platform::instance()->information(tr("Bookmarked current position"), this);
}

void MainWindow::onGoToBookmark(int index)
{
    TRACE;
    Book *book = Library::instance()->book(mCurrent);
    view->goToBookmark(book->bookmarks()[index]);
}

void MainWindow::showChapters()
{
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        ChaptersDialog *chapters = new ChaptersDialog(book, this);
        connect(chapters, SIGNAL(goToChapter(int)),
                this, SLOT(onGoToChapter(int)));
        chapters->show();
    }
}

void MainWindow::onGoToChapter(int index)
{
    TRACE;

    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        QString fragment;
        int partIndex = book->partFromChapter(index, fragment);
        if (partIndex != -1) {
            view->goToPart(partIndex, fragment);
        }
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == preventBlankingTimer) {
#ifdef Q_WS_MAEMO_5
        QDBusInterface mce(MCE_SERVICE, MCE_REQUEST_PATH,
                           MCE_REQUEST_IF, QDBusConnection::systemBus());
        mce.call(MCE_PREVENT_BLANK_REQ);
#endif // Q_WS_MAEMO_5
        qDebug() << "MainWindow::timerEvent: Prevent display blanking";
    }
    AdopterWindow::timerEvent(event);
}

void MainWindow::about()
{
    Dyalog *aboutDialog = new Dyalog(this, false);
    aboutDialog->setWindowTitle(tr("About Dorian"));
    QString version = Platform::instance()->version();
#if !defined(Q_WS_MAEMO_5) && !defined(Q_OS_SYMBIAN)
    QLabel *icon = new QLabel(aboutDialog);
    QPixmap pixmap(Platform::instance()->icon("dorian"));
    icon->setPixmap(pixmap);
    aboutDialog->addWidget(icon);
#endif
    QLabel *label = new QLabel(aboutDialog);
    label->setTextFormat(Qt::RichText);
    label->setOpenExternalLinks(true);
    label->setWordWrap(true);
    label->setText(tr("<b>Dorian %1</b><br><br>"
        "Copyright &copy; 2010-2011 by "
        "Akos Polster &lt;akos@pipacs.com&gt;<br><br>"
        "Licensed under GNU General Public License, Version 3<br><br>"
        "<a href='http://dorian.garage.maemo.org/'>"
        "dorian.garage.maemo.org</a><br><br>"
        ).arg(version));
    aboutDialog->addWidget(label);
    aboutDialog->addStretch();
    aboutDialog->show();
}

void MainWindow::goToNextPage()
{
    next->flash();
    prev->flash();
    view->goNextPage();
}

void MainWindow::goToPreviousPage()
{
    next->flash();
    prev->flash();
    view->goPreviousPage();
}

void MainWindow::onBeginUpgrade(int total)
{
    libraryProgress->setVisible(total > 0);
    libraryProgress->setWindowTitle(tr("Upgrading library"));
    libraryProgress->setMaximum(total);
}

void MainWindow::onUpgrading(const QString &path)
{
    libraryProgress->setLabelText(tr("Upgrading %1").
                                  arg(QFileInfo(path).fileName()));
    libraryProgress->setValue(libraryProgress->value() + 1);
}

void MainWindow::onEndUpgrade()
{
    libraryProgress->hide();
    libraryProgress->reset();
}

void MainWindow::onBeginLoad(int total)
{
    libraryProgress->setVisible(total > 0);
    libraryProgress->setWindowTitle(tr("Loading library"));
    libraryProgress->setMaximum(total);
}

void MainWindow::onLoading(const QString &path)
{
    libraryProgress->setLabelText(tr("Loading %1").
                                  arg(QFileInfo(path).fileName()));
    libraryProgress->setValue(libraryProgress->value() + 1);
}

void MainWindow::onEndLoad()
{
    libraryProgress->hide();
    libraryProgress->reset();
}
