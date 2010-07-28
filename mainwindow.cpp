#include <QtGui>
#include <QtDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#ifdef Q_WS_MAEMO_5
#   include <QtMaemo5/QMaemo5InformationBox>
#endif

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

#ifdef DORIAN_TEST_MODEL
#include "modeltest.h"
#endif

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

const Qt::WindowFlags WIN_BIG_FLAGS =
        Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
const int WIN_BIG_TIMER = 3000;

MainWindow::MainWindow(QWidget *parent):
        QMainWindow(parent), view(0), fullScreenWindow(0)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
#endif
    setWindowTitle("Dorian");

    // Book view
    view = new BookView(this);
    setCentralWidget(view);

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

    fullScreenAction = addToolBarAction(this, SLOT(showFullScreen()),
                                        "view-fullscreen");

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
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    Settings::instance()->setValue("orientation",
                                   Settings::instance()->value("orientation"));

    // Handle loading chapters
    connect(view, SIGNAL(chapterLoadStart(int)),
            this, SLOT(onChapterLoadStart()));
    connect(view, SIGNAL(chapterLoadEnd(int)),
            this, SLOT(onChapterLoadEnd(int)));

#ifdef DORIAN_TEST_MODEL
    (void)new ModelTest(Library::instance(), this);
#endif
}

void MainWindow::onCurrentBookChanged()
{
    setCurrentBook(Library::instance()->nowReading());
}

void MainWindow::showNormal()
{
    qDebug() << "MainWindow::showNormal";
    view->setParent(this);
    setCentralWidget(view);
    show();
    delete fullScreenWindow;
    fullScreenWindow = 0;
}

void MainWindow::showFullScreen()
{
    qDebug() << "MainWindow::showFullscreen";
    fullScreenWindow = new FullScreenWindow(view, this);
    fullScreenWindow->showFullScreen();
    connect(fullScreenWindow, SIGNAL(restore()), this, SLOT(showNormal()));
}

void MainWindow::setCurrentBook(const QModelIndex &current)
{
    mCurrent = current;
    Book *book = Library::instance()->book(current);
    view->setBook(book);
    setWindowTitle(book? book->name(): tr("Dorian"));
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
    LibraryDialog *dialog = new LibraryDialog(this);
    dialog->show();
}

void MainWindow::showSettings()
{
    SettingsWindow *settings = new SettingsWindow(this);
    settings->show();
}

void MainWindow::showInfo()
{
    if (mCurrent.isValid()) {
        InfoDialog *info =
            new InfoDialog(Library::instance()->book(mCurrent), this);
        info->exec();
    }
}

void MainWindow::showDevTools()
{
    DevTools *devTools = new DevTools();
    devTools->exec();
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
    qDebug() << "MainWindow::closeEvent";
    view->setLastBookmark();
    event->accept();
}

void MainWindow::onSettingsChanged(const QString &key)
{
#ifdef Q_WS_MAEMO_5
    if (key == "orientation") {
        QString value = Settings::instance()->value(key).toString();
        if (value == "portrait") {
            setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, false);
        }
        else {
            setAttribute(Qt::WA_Maemo5PortraitOrientation, false);
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
        }
    }
#else
    Q_UNUSED(key);
#endif // Q_WS_MAEMO_5
}

void MainWindow::onChapterLoadStart()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
}

void MainWindow::onChapterLoadEnd(int index)
{
    bool enablePrevious = false;
    bool enableNext = false;
    Book *book = Library::instance()->book(mCurrent);
    if (book) {
        if (index > 0) {
            enablePrevious = true;
        }
        if (index < (book->toc.size() - 1)) {
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
    view->addBookmark();
}

void MainWindow::onGoToBookmark(int index)
{
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
    view->goToBookmark(Book::Bookmark(index, 0));
}

