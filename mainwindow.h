#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "bookfinder.h"

class QString;
class QModelIndex;
class DevTools;
class BookView;
class Book;
class FullScreenWindow;
class Progress;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showLibrary();
    void showInfo();
    void showSettings();
    void showDevTools();
    void showBookmarks();
    void onCurrentBookChanged();
    void showRegular();
    void showBig();
    void onSettingsChanged(const QString &key);
    void onPartLoadStart();
    void onPartLoadEnd(int index);
    void onAddBookmark();
    void onGoToBookmark(int index);
    void showChapters();
    void onGoToChapter(int index);

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    void setCurrentBook(const QModelIndex &current);
    QAction *addToolBarAction(const QObject *receiver, const char *member,
                              const QString &name);
    BookView *view;
    QAction *settingsAction;
    QAction *libraryAction;
    QAction *infoAction;
    QAction *devToolsAction;
    QAction *bookmarksAction;
    QAction *fullScreenAction;
    QAction *forwardAction;
    QAction *backwardAction;
    QAction *previousAction;
    QAction *nextAction;
    QAction *chaptersAction;
    QToolBar *toolBar;
    QDialog *settings;
    DevTools *devTools;
    QModelIndex mCurrent;
    FullScreenWindow *fullScreenWindow;
    int preventBlankingTimer;
    BookFinder *bookFinder;
    BookFinderThread bookFinderThread;
    Progress *progress;
};

#endif // MAINWINDOW_H
