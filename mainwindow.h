#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QModelIndex>
#include <QEvent>

#include "adopterwindow.h"

class QString;
class QModelIndex;
class DevTools;
class BookView;
class Book;
class FullScreenWindow;
class Progress;
class ProgressDialog;
class TranslucentButton;

class MainWindow: public AdopterWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void initialize();

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
    void onAddBookmark(const QString &note);
    void onGoToBookmark(int index);
    void showChapters();
    void onGoToChapter(int index);
    void about();
    void goToNextPage();
    void goToPreviousPage();
    void onBeginUpgrade(int total);
    void onUpgrading(const QString &book);
    void onEndUpgrade();
    void onBeginLoad(int total);
    void onLoading(const QString &book);
    void onEndLoad();

protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    void setCurrentBook(const QModelIndex &current);
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
    QDialog *settings;
    DevTools *devTools;
    QModelIndex mCurrent;
    FullScreenWindow *fullScreenWindow;
    int preventBlankingTimer;
    Progress *prog;
    TranslucentButton *prev;
    TranslucentButton *next;
    ProgressDialog *libraryProgress;
};

#endif // MAINWINDOW_H
