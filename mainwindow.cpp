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

    // Dialogs
    progress = new Progress(this);

    // Tool bar actions

#ifdef Q_OS_SYMBIAN
    fullScreenAction = addToolBarAction(this, SLOT(showBig()),
                                        "view-fullscreen", tr("Full screen"));
#endif

    chaptersAction = addToolBarAction(this, SLOT(showChapters()),
                                      "chapters", tr("Chapters"));
    bookmarksAction = addToolBarAction(this, SLOT(showBookmarks()),
                                       "bookmarks", tr("Bookmarks"));
    infoAction = addToolBarAction(this, SLOT(showInfo()),
                                  "info", tr("Book info"));
    libraryAction = addToolBarAction(this, SLOT(showLibrary()),
                                     "library", tr("Library"));

#ifdef Q_WS_MAEMO_5
    settingsAction = menuBar()->addAction(tr("Settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    devToolsAction = menuBar()->addAction(tr("Developer"));
    connect(devToolsAction, SIGNAL(triggered()), this, SLOT(showDevTools()));
    QAction *aboutAction = menuBar()->addAction(tr("About"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
#else
    settingsAction = addToolBarAction(this, SLOT(showSettings()),
                                      "preferences-system", tr("Settings"));
    devToolsAction = addToolBarAction(this, SLOT(showDevTools()),
                                      "developer", tr("Developer"));
    addToolBarAction(this, SLOT(about()), "about", tr("About"));
#endif // Q_WS_MAEMO_5

#ifndef Q_OS_SYMBIAN
    addToolBarSpace();
    fullScreenAction = addToolBarAction(this, SLOT(showBig()),
                                        "view-fullscreen", tr("Full screen"));
#else
    (void)addToolBarAction(this, SLOT(close()), "", tr("Exit"));
#endif

    // Buttons on top of the book view
    previousButton = new TranslucentButton("back", this);
    nextButton = new TranslucentButton("forward", this);

    // Handle model changes
    connect(Library::instance(), SIGNAL(nowReadingChanged()),
            this, SLOT(onCurrentBookChanged()));

    // Load library, upgrade it if needed
    libraryProgress = new ProgressDialog(tr("Upgrading library"), this);
    Library *library = Library::instance();
    connect(library, SIGNAL(beginUpgrade(int)), this, SLOT(onBeginUpgrade(int)));
    connect(library, SIGNAL(upgrading(const QString &)),
            this, SLOT(onUpgrading(const QString &)));
    connect(library, SIGNAL(endUpgrade()), this, SLOT(onEndUpgrade()));

    // Handle loading book parts
    connect(view, SIGNAL(partLoadStart(int)), this, SLOT(onPartLoadStart()));
    connect(view, SIGNAL(partLoadEnd(int)), this, SLOT(onPartLoadEnd(int)));

    // Handle progress
    connect(view, SIGNAL(progress(qreal)), progress, SLOT(setProgress(qreal)));

    // Shadow window for full screen reading
    fullScreenWindow = new FullScreenWindow(this);
    connect(fullScreenWindow, SIGNAL(restore()), this, SLOT(showRegular()));

    // Handle settings changes
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));

    // Handle book view buttons
    connect(nextButton, SIGNAL(triggered()), this, SLOT(goToNextPage()));
    connect(previousButton, SIGNAL(triggered()), this, SLOT(goToPreviousPage()));

    // Adopt view, show window
    showRegular();

#ifdef DORIAN_TEST_MODEL
    (void)new ModelTest(Library::instance(), this);
#endif
}

void MainWindow::initialize()
{
    TRACE;
    Library *library = Library::instance();

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

    // Re-parent children
    fullScreenWindow->leaveChildren();
    QList<QWidget *> otherChildren;
    otherChildren << progress << previousButton << nextButton;
    takeChildren(view, otherChildren);

    // Adjust geometry of decorations

    QRect geo = geometry();
    int y = geo.height() - progress->thickness();
#if defined(Q_WS_MAEMO_5)
    y -= toolBar->height();
#endif
    progress->setGeometry(0, y, geo.width(), y + progress->thickness());

#if defined(Q_WS_MAEMO_5)
    previousButton->setGeometry(0,
        geo.height() - toolBar->height() - TranslucentButton::pixels,
        TranslucentButton::pixels, TranslucentButton::pixels);
    nextButton->setGeometry(geo.width() - TranslucentButton::pixels, 0,
        TranslucentButton::pixels, TranslucentButton::pixels);
#elif defined(Q_OS_SYMBIAN)
    previousButton->setGeometry(0, geo.height() - TranslucentButton::pixels,
        TranslucentButton::pixels, TranslucentButton::pixels);
    nextButton->setGeometry(geo.width() - TranslucentButton::pixels,
        0, TranslucentButton::pixels, TranslucentButton::pixels);
#else
    previousButton->setGeometry(0, geo.height() - TranslucentButton::pixels,
        TranslucentButton::pixels, TranslucentButton::pixels);
    nextButton->setGeometry(geo.width() - TranslucentButton::pixels - 25,
        toolBar->height(), TranslucentButton::pixels,
        TranslucentButton::pixels);
#endif // Q_WS_MAEMO_5
    qDebug() << "previousButton geometry" << previousButton->geometry();

    fullScreenWindow->hide();
    show();
#if defined(Q_OS_SYMBIAN)
    activateWindow();
#endif
    progress->flash();
    nextButton->flash();
    previousButton->flash();
}

void MainWindow::showBig()
{
    TRACE;

    // Re-parent children
    leaveChildren();
    QList<QWidget *> otherChildren;
    otherChildren << progress << nextButton << previousButton;
    fullScreenWindow->takeChildren(view, otherChildren);

    // Adjust geometry of decorations
    QRect screen = QApplication::desktop()->screenGeometry();
    int y = screen.height() - progress->thickness();
    progress->setGeometry(0, y, screen.width(), y + progress->thickness());
#if defined(Q_WS_MAEMO_5)
    nextButton->setGeometry(screen.width() - TranslucentButton::pixels, 0,
        TranslucentButton::pixels, TranslucentButton::pixels);
#else
    nextButton->setGeometry(screen.width() - TranslucentButton::pixels - 25, 0,
        TranslucentButton::pixels, TranslucentButton::pixels);
#endif // Q_WS_MAEMO_5
    previousButton->setGeometry(0, screen.height() - TranslucentButton::pixels,
        TranslucentButton::pixels, TranslucentButton::pixels);

#ifdef Q_OS_SYMBIAN
    hide();
#endif
    fullScreenWindow->showFullScreen();
#ifdef Q_OS_SYMBIAN
    fullScreenWindow->activateWindow();
#endif
    progress->flash();
    nextButton->flash();
    previousButton->flash();
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

void MainWindow::showInfo()
{
    if (mCurrent.isValid()) {
        (new InfoDialog(Library::instance()->book(mCurrent), this, false))->
                exec();
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
        connect(bookmarks, SIGNAL(addBookmark(const QString &)),
                this, SLOT(onAddBookmark(const QString &)));
        connect(bookmarks, SIGNAL(goToBookmark(int)),
                this, SLOT(onGoToBookmark(int)));
        bookmarks->show();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    TRACE;
    view->setLastBookmark();
    event->accept();
}

void MainWindow::onSettingsChanged(const QString &key)
{
#if defined(Q_WS_MAEMO_5)
    if (key == "orientation") {
        QString value = Settings::instance()->value(key,
            Platform::instance()->defaultOrientation()).toString();
        qDebug() << "MainWindow::onSettingsChanged: orientation" << value;
        if (value == "portrait") {
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, false);
            setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
            fullScreenWindow->setAttribute(Qt::WA_Maemo5LandscapeOrientation,
                                           false);
            fullScreenWindow->setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
        } else {
            setAttribute(Qt::WA_Maemo5PortraitOrientation, false);
            setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
            fullScreenWindow->setAttribute(Qt::WA_Maemo5PortraitOrientation,
                                           false);
            fullScreenWindow->setAttribute(Qt::WA_Maemo5LandscapeOrientation,
                                           true);
        }
    } else if (key == "lightson") {
        bool enable = Settings::instance()->value(key, false).toBool();
        qDebug() << "MainWindow::onSettingsChanged: lightson" << enable;
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

void MainWindow::resizeEvent(QResizeEvent *e)
{
    TRACE;

    if (hasChild(progress)) {
        qDebug() << "To" << e->size();
        int y = e->size().height() - progress->thickness();
#if defined(Q_WS_MAEMO_5)
        y -= toolBar->height();
#endif
        progress->setGeometry(0, y, e->size().width(), y + progress->thickness());

#if defined(Q_WS_MAEMO_5)
        previousButton->setGeometry(0,
            e->size().height() - toolBar->height() - TranslucentButton::pixels,
            TranslucentButton::pixels, TranslucentButton::pixels);
        nextButton->setGeometry(e->size().width() - TranslucentButton::pixels, 0,
            TranslucentButton::pixels, TranslucentButton::pixels);
#elif defined(Q_OS_SYMBIAN)
        previousButton->setGeometry(0, e->size().height() - TranslucentButton::pixels,
            TranslucentButton::pixels, TranslucentButton::pixels);
        nextButton->setGeometry(e->size().width() - TranslucentButton::pixels,
            0, TranslucentButton::pixels, TranslucentButton::pixels);
#else
        previousButton->setGeometry(0,
            e->size().height() - TranslucentButton::pixels,
            TranslucentButton::pixels, TranslucentButton::pixels);
        nextButton->setGeometry(e->size().width() - TranslucentButton::pixels - 25,
            toolBar->height(), TranslucentButton::pixels,
            TranslucentButton::pixels);
#endif // Q_WS_MAEMO_5

#ifdef Q_WS_MAEMO_5
        // This is needed on Maemo, in order not to lose current reading position
        // after orientation change
        QTimer::singleShot(250, view, SLOT(restoreLastBookmark()));
#endif
        previousButton->flash();
        nextButton->flash();
    }
    QMainWindow::resizeEvent(e);
}

void MainWindow::about()
{
    Dyalog *aboutDialog = new Dyalog(this, false);
    aboutDialog->setWindowTitle(tr("About Dorian"));
    QString version = Platform::instance()->version();
    QLabel *label = new QLabel(aboutDialog);
    label->setTextFormat(Qt::RichText);
    label->setOpenExternalLinks(true);
    label->setWordWrap(true);
    label->setText(tr("<b>Dorian %1</b><br><br>Copyright &copy; 2010 "
        "Akos Polster &lt;akos@pipacs.com&gt;<br>"
        "Licensed under GNU General Public License, Version 3<br>"
        "Source code:<br><a href='https://garage.maemo.org/projects/dorian/'>"
        "garage.maemo.org/projects/dorian</a>").arg(version));
    aboutDialog->addWidget(label);
    aboutDialog->addStretch();
    aboutDialog->show();
}

void MainWindow::goToNextPage()
{
    nextButton->flash();
    previousButton->flash();
    view->goNextPage();
}

void MainWindow::goToPreviousPage()
{
    nextButton->flash();
    previousButton->flash();
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
