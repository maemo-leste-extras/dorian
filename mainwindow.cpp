#include <QtGui>
#include <QtDebug>
#include <QDir>
#include <QApplication>
#include <QFileInfo>
#include <QStringList>

#ifdef Q_WS_MAEMO_5
#   include <QtMaemo5/QMaemo5InformationBox>
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

#ifdef DORIAN_TEST_MODEL
#include "modeltest.h"
#endif

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

const int PROGRESS_HEIGHT = 17;
const char *DORIAN_VERSION =
#include "pkg/version.txt"
;

MainWindow::MainWindow(QWidget *parent):
    BookWindow(parent), view(0), preventBlankingTimer(-1)
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
    QAction *aboutAction = menuBar()->addAction(tr("About"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
#else
    infoAction = addToolBarAction(this, SLOT(showInfo()), "document-properties");
    libraryAction = addToolBarAction(this, SLOT(showLibrary()),
                                     "system-file-manager");
    settingsAction = addToolBarAction(this, SLOT(showSettings()),
                                      "preferences-system");
    devToolsAction = addToolBarAction(this, SLOT(showDevTools()), "developer");
    addToolBarAction(this, SLOT(about()), "about");
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

    // Handle loading book parts
    connect(view, SIGNAL(partLoadStart(int)), this, SLOT(onPartLoadStart()));
    connect(view, SIGNAL(partLoadEnd(int)), this, SLOT(onPartLoadEnd(int)));

    // Handle progress
    connect(view, SIGNAL(progress(qreal)), progress, SLOT(setProgress(qreal)));

    // Shadow window for full screen
    fullScreenWindow = new FullScreenWindow(this);
    connect(fullScreenWindow, SIGNAL(restore()), this, SLOT(showRegular()));

    // Handle settings changes
    Settings *settings = Settings::instance();
    connect(settings, SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    settings->setValue("orientation", settings->value("orientation"));
    settings->setValue("lightson", settings->value("lightson"));
    settings->setValue("usevolumekeys", settings->value("usevolumekeys"));

#ifdef DORIAN_TEST_MODEL
    (void)new ModelTest(Library::instance(), this);
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::onCurrentBookChanged()
{
    setCurrentBook(Library::instance()->nowReading());
}

void MainWindow::showRegular()
{
    Trace t("MainWindow::showRegular");
    fullScreenWindow->hide();
    fullScreenWindow->leaveChildren();
    progress->setGeometry(0, 0, geometry().width(), PROGRESS_HEIGHT);
    QList<QWidget *> otherChildren;
    otherChildren.append(progress);
    takeChildren(view, otherChildren);
    progress->flash();
}

void MainWindow::showBig()
{
    Trace t("MainWindow::showBig");
    leaveChildren();
    QList<QWidget *> otherChildren;
    otherChildren.append(progress);
    progress->setGeometry(0, 0, QApplication::desktop()->screenGeometry().width(),
                          PROGRESS_HEIGHT);
    fullScreenWindow->takeChildren(view, otherChildren);
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
        qDebug() << "MainWindow::onSettingsChanged: orientation" << value;
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
        qDebug() << "MainWindow::onSettingsChanged: lightson:" << enable;
        killTimer(preventBlankingTimer);
        if (enable) {
            preventBlankingTimer = startTimer(29 * 1000);
        }
    } else if (key == "usevolumekeys") {
        bool value = Settings::instance()->value(key).toBool();
        qDebug() << "MainWindow::onSettingsChanged: usevolumekeys" << value;
        grabZoomKeys(value);
        fullScreenWindow->grabZoomKeys(value);
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
        qDebug() << "MainWindow::timerEvent: Prevent display blanking";
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    progress->setGeometry(QRect(0, 0, e->size().width(), PROGRESS_HEIGHT));
    QMainWindow::resizeEvent(e);
}

void MainWindow::about()
{
    Dyalog *aboutDialog = new Dyalog(this);
    aboutDialog->setWindowTitle(tr("About Dorian"));
    QLabel *label = new QLabel(aboutDialog);
    label->setTextFormat(Qt::RichText);
    label->setOpenExternalLinks(true);
    label->setText(tr("<b>Dorian %1</b><br><br>Copyright &copy; 2010 by "
        "Akos Polster &lt;akos@pipacs.com&gt;<br>"
        "Licensed under GNU General Public License, Version 3<br>"
        "Source code: <a href='https://garage.maemo.org/projects/dorian/'>"
        "garage.maemo.org/projects/dorian</a>").arg(DORIAN_VERSION));
    aboutDialog->addWidget(label);
    aboutDialog->show();
}
