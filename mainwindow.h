#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "adopterwindow.h"

class QString;
class QModelIndex;
class DevTools;
class BookView;
class Book;
class FullScreenWindow;
class Progress;
class TranslucentButton;

class MainWindow: public AdopterWindow
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
    void about();
    void goToNextPage();
    void goToPreviousPage();

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
    QAction *chaptersAction;
    QToolBar *toolBar;
    QDialog *settings;
    DevTools *devTools;
    QModelIndex mCurrent;
    FullScreenWindow *fullScreenWindow;
    int preventBlankingTimer;
    Progress *progress;
    TranslucentButton *previousButton;
    TranslucentButton *nextButton;
};

#endif // MAINWINDOW_H
