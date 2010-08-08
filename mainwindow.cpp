#include <QtGui>
#include <QtDebug>
#include <QDir>
#include <QApplication>
#include <QFileInfo>
#include <QStringList>

#ifdef Q_WS_MAEMO_5
#   include <QtMaemo5/QMaemo5InformationBox>
#   include <QtDBus>
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

#ifdef DORIAN_TEST_MODEL
#include "modeltest.h"
#endif

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

const int PROGRESS_HEIGHT = 17;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), view(0), preventBlankingTimer(-1)
{
    Trace t("MainWindow::MainWindow");
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif
    setWindowTitle("Dorian");

    // Central widget. Must be an intermediate, because the book view widget
    // can be re-parented later
    QFrame *central = new QFrame(this);
    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setMargin(0);
    central->setLayout(layout);
    setCentralWidget(central);

    // Book view
    view = new BookView(central);
    view->show();
    layout->addWidget(view);

    // Progress
    progress = new Progress(central);

    // Tool bar
    setUnifiedTitleAndToolBarOnMac(true);
    settings = new QDialog(this);
    toolBar = addToolBar("controls");
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->toggleViewAction()->setVisible(false);
#if defined(Q_WS_X11) && !defined(Q_WS_MAEMO_5)
    toolBar->setIconSize(QSize(42, 42));
#endif

    previousAction = addToolBarAction(view, SLOT(goPrevious()), "previous");
    nextAction = addToolBarAction(view, SLOT(goNext()), "next");
    chaptersAction = addToolBarAction(this, SLOT(showChapters()), "chapters");
    bookmarksAction = addToolBarAction(this, SLOT(showBookmarks()), "bookmarks");

#ifdef Q_WS_MAEMO_5
    infoAction = menuBar()->addAction(tr("Book details"));
    connect(infoAction, SIGNAL(triggered()), this, SLOT(showInfo()));
    libraryAction = menuBar()->addAction(tr("Library"));
    connect(libraryAction, SIGNAL(triggered()), this, SLOT(showLibrary()));
    settingsAction = menuBar()->addAction(tr("Settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    devToolsAction = menuBar()->addAction(tr("Developer"));
    connect(devToolsAction, SIGNAL(triggered()), this, SLOT(showDevTools()));
#else
    infoAction = addToolBarAction(this, SLOT(showInfo()), "document-properties");
    libraryAction = addToolBarAction(this, SLOT(showLibrary()),
                                     "system-file-manager");
    settingsAction = addToolBarAction(this, SLOT(showSettings()),
                                      "preferences-system");
    devToolsAction = addToolBarAction(this, SLOT(showDevTools()), "developer");
#endif // Q_WS_MAEMO_5

    QFrame *frame = new QFrame(toolBar);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(frame);

    fullScreenAction = addToolBarAction(this, SLOT(showBig()), "view-fullscreen");

    // Handle model changes
    connect(Library::instance(), SIGNAL(nowReadingChanged()),
            this, SLOT(onCurrentBookChanged()));

    // Load book on command line, or load last read book, or load default book
    Library *library = Library::instance();
    if (QCoreApplication::arguments().size() == 2) {
        QString path = QCoreApplication::arguments()[1];
        library->add(path);
        QModelIndex index = library->find(path);
        if (index.isValid()) {
            library->setNowReading(index);
        }
    }
    else {
        QModelIndex index = library->nowReading();
        if (index.isValid()) {
            library->setNowReading(index);
        }
        else {
            if (!library->rowCount()) {
                library->add(":/books/2 B R 0 2 B.epub");
            }
            library->setNowReading(library->index(0));
        }
    }

    // Handle settings changes
    Settings *settings = Settings::instance();
    connect(settings, SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    settings->setValue("orientation", settings->value("orientation"));
    settings->setValue("lightson", settings->value("lightson"));

    // Handle loading chapters
    connect(view, SIGNAL(chapterLoadStart(int)),
            this, SLOT(onChapterLoadStart()));
    connect(view, SIGNAL(chapterLoadEnd(int)),
            this, SLOT(onChapterLoadEnd(int)));

    // Handle progress
    connect(view, SIGNAL(progress(qreal)), progress, SLOT(setProgress(qreal)));

    // Shadow window for full screen
    fullScreenWindow = new FullScreenWindow(this);
    connect(fullScreenWindow, SIGNAL(restore()), this, SLOT(showRegular()));

    // Create thread for finding books in directories
    bookFinder = new BookFinder();
    connect(bookFinder, SIGNAL(add(const QString &)),
            library, SLOT(add(const QString &)));
    connect(bookFinder, SIGNAL(remove(const QString &)),
            library, SLOT(remove(const QString &)));
    bookFinder->moveToThread(&bookFinderThread);
    bookFinderThread.start();

#ifdef DORIAN_TEST_MODEL
    (void)new ModelTest(Library::instance(), this);
#endif
}

MainWindow::~MainWindow()
{
    bookFinderThread.quit();
    bookFinderThread.wait();
    delete bookFinder;
}

void MainWindow::onCurrentBookChanged()
{
    setCurrentBook(Library::instance()->nowReading());
}

void MainWindow::showRegular()
{
    Trace t("MainWindow::showRegular");
    fullScreenWindow->hide();
    fullScreenWindow->leaveChild();
    view->setParent(centralWidget());
    progress->setParent(centralWidget());
    progress->setGeometry(0, 0, geometry().width(), PROGRESS_HEIGHT);
    centralWidget()->layout()->addWidget(view);
    progress->flash();
}

void MainWindow::showBig()
{
    Trace t("MainWindow::showBig");
    centralWidget()->layout()->removeWidget(view);
    progress->setParent(fullScreenWindow);
    progress->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(),
                          PROGRESS_HEIGHT);
    fullScreenWindow->takeChild(view);
    fullScreenWindow->showFullScreen();
    progress->flash();
}

void MainWindow::setCurrentBook(const QModelIndex &current)
{
    mCurrent = current;
    Book *book = Library::instance()->book(current);
    view->setBook(book);
    setWindowTitle(book? book->shortName(): tr("Dorian"));
}

QAction *MainWindow::addToolBarAction(const QObject *receiver,
                                      const char *member,
                                      const QString &name)
{
    return toolBar->
        addAction(QIcon(ICON_PREFIX + name + ".png"), "", receiver, member);
}

void MainWindow::showLibrary()
{
    (new LibraryDialog(this))->show();
}

void MainWindow::showSettings()
{
    (new SettingsWindow(this))->show();
}

void MainWindow::showInfo()
{
    if (mCurrent.isValid()) {
        (new InfoDialog(Library::instance()->book(mCurrent), this))->exec();
    }
}

void MainWindow::showDevTools()
{
    (new DevTools())->exec();
}

void MainWindow::showBookmarks()
{
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        BookmarksDialog *bookmarks = new BookmarksDialog(book, this);
        bookmarks->setWindowModality(Qt::WindowModal);
        connect(bookmarks, SIGNAL(addBookmark()), this, SLOT(onAddBookmark()));
        connect(bookmarks, SIGNAL(goToBookmark(int)),
                this, SLOT(onGoToBookmark(int)));
        bookmarks->show();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Trace t("MainWindow::closeEvent");
    view->setLastBookmark();
    event->accept();
}

void MainWindow::onSettingsChanged(const QString &key)
{
#ifdef Q_WS_MAEMO_5
    if (key == "orientation") {
        QString value = Settings::instance()->value(key).toString();
        Trace::trace(QString("MainWindow::onSettingsChanged: orientation %1").
                     arg(value));
        if (value == "portrait") {
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, false);
            setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
        }
        else {
            setAttribute(Qt::WA_Maemo5PortraitOrientation, false);
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
        }
    } else if (key == "lightson") {
        bool enable = Settings::instance()->value(key, false).toBool();
        Trace::trace(QString("MainWindow::onSettingsChanged: lightson: %1").
                     arg(enable));
        killTimer(preventBlankingTimer);
        if (enable) {
            preventBlankingTimer = startTimer(29 * 1000);
        }
    }
#else
    Q_UNUSED(key);
#endif // Q_WS_MAEMO_5
}

void MainWindow::onPartLoadStart()
{
    Trace t("MainWindow::onPartLoadStart");
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}

void MainWindow::onPartLoadEnd(int index)
{
    Trace t("MainWindow::onPartLoadEnd");
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
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
    previousAction->setIcon(QIcon(enablePrevious?
        ":/icons/previous.png" : ":/icons/previous-disabled.png"));
    nextAction->setIcon(QIcon(enableNext?
        ":/icons/next.png": ":/icons/next-disabled.png"));
#endif // Q_WS_MAEMO_5
    previousAction->setEnabled(enablePrevious);
    nextAction->setEnabled(enableNext);
}

void MainWindow::onAddBookmark()
{
    Trace t("MainWindow:onAddBookmark");
    view->addBookmark();
}

void MainWindow::onGoToBookmark(int index)
{
    Trace t("MainWindow::onGoToBookmark");
    Book *book = Library::instance()->book(mCurrent);
    view->goToBookmark(book->bookmarks()[index]);
}

void MainWindow::showChapters()
{
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        ChaptersDialog *chapters = new ChaptersDialog(book, this);
        chapters->setWindowModality(Qt::WindowModal);
        connect(chapters, SIGNAL(goToChapter(int)),
                this, SLOT(onGoToChapter(int)));
        chapters->show();
    }
}

void MainWindow::onGoToChapter(int index)
{
    Trace t("MainWindow::onGoToChapter");

    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        int partIndex = book->partFromChapter(index);
        if (partIndex != -1) {
            view->goToBookmark(Book::Bookmark(partIndex, 0));
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
        Trace::trace("MainWindow::timerEvent: Prevent display blanking");
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    progress->setGeometry(QRect(0, 0, e->size().width(), PROGRESS_HEIGHT));
    QMainWindow::resizeEvent(e);
}
