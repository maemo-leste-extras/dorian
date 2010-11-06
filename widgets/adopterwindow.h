#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QMainWindow>
#include <QList>

#ifdef Q_OS_SYMBIAN
#   include "mediakeysobserver.h"
#endif

class QWidget;
class QToolBar;
class QAction;

/**
  * A main window that can adopt other windows' children, and grabs the
  * zoom (volume) keys on Maemo.
  */
class AdopterWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit AdopterWindow(QWidget *parent = 0);

    /* If true, zoom (volume) keys will generate Key_F7 and Key_F8 events. */
    void grabZoomKeys(bool grab);

    /** Adopt children "main" and "others". */
    void takeChildren(QWidget *main, const QList<QWidget *> &others);

    /** Release current children (adopted in @see takeChildren). */
    void leaveChildren();

    /**
     * Add action that is visible on the tool bar (except on Symbian, where
     * it is visible on the Options menu).
     */
    QAction *addToolBarAction(QObject *receiver, const char *slot,
                              const QString &iconName, const QString &text);

    /** Add space. */
    void addToolBarSpace();

    /** Show window. */
    void show();

signals:

protected slots:
#ifdef Q_OS_SYMBIAN
    void onMediaKeysPressed(MediaKeysObserver::MediaKeys key);
#endif

protected:
    void showEvent(QShowEvent *e);
    void doGrabZoomKeys(bool grab);
    bool grabbingZoomKeys;
    QWidget *mainChild;
#ifndef Q_OS_SYMBIAN
    QToolBar *toolBar;
#endif
};

#endif // ADOPTERWINDOW_H
