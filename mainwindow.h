#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

class QString;
class QModelIndex;
class DevTools;
class BookView;
class Book;
class TranslucentButton;

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow() {}

public slots:
    void showLibrary();
    void showInfo();
    void showSettings();
    void showDevTools();
    void showBookmarks();
    void onCurrentBookChanged();
    void showNormal();
    void showFullScreen();
    void onSettingsChanged(const QString &key);
    void onChapterLoaded(int index);

protected:
#ifdef Q_WS_MAEMO5
#   define MOUSE_ACTIVATE_EVENT mouseReleaseEvent
#else
#   define MOUSE_ACTIVATE_EVENT mousePressEvent
#endif
    virtual void MOUSE_ACTIVATE_EVENT(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:
    void setCurrentBook(const QModelIndex &current);
    QAction *addToolBarAction(const QObject *receiver, const char *member,
                              const QString &name);
    QRect fullScreenZone() const;
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
    QToolBar *toolBar;
    QDialog *settings;
    DevTools *devTools;
    QModelIndex mCurrent;
    Qt::WindowFlags normalFlags;
    TranslucentButton *restoreButton;
    bool isFullscreen;
    QRect normalGeometry;
};

#endif // MAINWINDOW_H
