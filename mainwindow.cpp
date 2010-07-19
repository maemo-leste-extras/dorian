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
#include "translucentbutton.h"
#include "settingswindow.h"
#include "bookmarksdialog.h"
#include "settings.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

const Qt::WindowFlags WIN_BIG_FLAGS =
        Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint;
const int WIN_BIG_TIMER = 3000;

MainWindow::MainWindow(QWidget *parent):
        QMainWindow(parent), view(0), book(0), isFullscreen(false)
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
    bookmarksAction = addToolBarAction(this, SLOT(showBookmarks()),
                                       "bookmarks");
#ifdef Q_WS_MAEMO_5
    infoAction = new QAction(this);
#else
    infoAction = addToolBarAction(this, SLOT(showInfo()), "document-properties");
#endif
    libraryAction = addToolBarAction(this, SLOT(showLibrary()),
                                     "system-file-manager");
    settingsAction = addToolBarAction(this, SLOT(showSettings()),
                                      "preferences-system");
#ifdef Q_WS_MAEMO_5
    devToolsAction = new QAction(this);
#else
    devToolsAction = addToolBarAction(this, SLOT(showDevTools()), "developer");
    QFrame *frame = new QFrame(toolBar);
    frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolBar->addWidget(frame);
#endif
    fullScreenAction = addToolBarAction(this, SLOT(showFullScreen()),
                                        "view-fullscreen");

    // Handle model changes
    connect(Library::instance(), SIGNAL(currentBookChanged()),
            this, SLOT(onCurrentBookChanged()));

    normalFlags = windowFlags();
    restoreButton = new TranslucentButton("view-fullscreen", this);

    // Load book on command line, or load last read book, or load default book
    Library *library = Library::instance();
    if (QCoreApplication::arguments().size() == 2) {
        QString path = QCoreApplication::arguments()[1];
        library->add(path);
        QModelIndex index = library->find(path);
        if (index.isValid()) {
            library->setCurrent(index);
        }
    }
    else {
        Book *current = library->current();
        if (current) {
            setCurrentBook(current);
        }
        else {
            if (!library->rowCount()) {
                library->add(":/books/2 B R 0 2 B.epub");
            }
            library->setCurrent(library->index(0));
        }
    }

    // Handle settings changes
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    Settings::instance()->setValue("orientation",
                                   Settings::instance()->value("orientation"));

    // Handle loading chapters
    connect(view, SIGNAL(chapterLoaded(int)), this, SLOT(onChapterLoaded(int)));
}

void MainWindow::onCurrentBookChanged()
{
    setCurrentBook(Library::instance()->current());
}

void MainWindow::showNormal()
{
    qDebug() << "MainWindow::showNormal";
    isFullscreen = false;
    setWindowFlags(normalFlags);
    hide();
    setGeometry(normalGeometry);
    toolBar->show();
    restoreButton->hide();
    show();
}

void MainWindow::showFullScreen()
{
    qDebug() << "MainWindow::showFullscreen";
    normalGeometry = geometry();
    isFullscreen = true;
    toolBar->hide();
    setWindowFlags(normalFlags | WIN_BIG_FLAGS);
    showMaximized();
    restoreButton->flash();
}

void MainWindow::setCurrentBook(Book *current)
{
    book = current;
    view->setBook(current);
    setWindowTitle(current? current->title: "Dorian");
}

QAction *MainWindow::addToolBarAction(const QObject *receiver, const char *member,
                                      const QString &name)
{
    return toolBar->
        addAction(QIcon(ICON_PREFIX + name + ".png"), "", receiver, member);
}

void MainWindow::showLibrary()
{
    LibraryDialog *dialog = new LibraryDialog();
    dialog->exec();
}

void MainWindow::showSettings()
{
    SettingsWindow *settings = new SettingsWindow(this);
    settings->show();
}

void MainWindow::showInfo()
{
    if (book) {
        InfoDialog *info = new InfoDialog(book, this);
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
    if (book) {
        BookmarksDialog *bookmarks = new BookmarksDialog(book, this);
        int ret = bookmarks->exec();
        if (ret > 0) {
            int index = ret - 1;
            view->goToBookmark(book->bookmarks()[index]);
        }
        else if (ret < 0) {
            view->addBookmark();
        }
    }
}

void MainWindow::MOUSE_ACTIVATE_EVENT(QMouseEvent *event)
{
    qDebug() << "MainWindow::mousePress/ReleaseEvent at" << event->pos()
            << "against" << fullScreenZone();
    if (isFullscreen && fullScreenZone().contains(event->x(), event->y())) {
        qDebug() << " In fullScreenZone";
        showNormal();
    }
    QMainWindow::MOUSE_ACTIVATE_EVENT(event);
}

QRect MainWindow::fullScreenZone() const
{
    return QRect(width() / 2 - 45, height() - 104, 95, 95);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    (void)event;
    restoreButton->setGeometry(fullScreenZone());
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

void MainWindow::onChapterLoaded(int index)
{
    bool enablePrevious = false;
    bool enableNext = false;
    if (book) {
        if (index > 0) {
            enablePrevious = true;
        }
        if (index < (book->toc.size() - 1)) {
            enableNext = true;
        }
    }
#ifdef Q_WS_MAEMO_5
    previousAction->setIcon(QIcon(enablePrevious?
        ":/icons/previous.png" : ":/icons/previous-disabled.png"));
    nextAction->setIcon(QIcon(enableNext?
        ":/icons/next.png": ":/icons/next-disabled.png"));
#endif // Q_WS_MAEMO_5
    previousAction->setEnabled(enablePrevious);
    nextAction->setEnabled(enableNext);
}
