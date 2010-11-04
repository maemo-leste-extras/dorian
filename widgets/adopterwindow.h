#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QMainWindow>
#include <QList>

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
    void grabZoomKeys(bool grab);
    void takeChildren(QWidget *main, const QList<QWidget *> &others);
    void leaveChildren();

    /**
     * Add action that is visible on the tool bar (except on Symbian, where
     * it is visible on the Options menu.
     */
    QAction *addToolBarAction(QObject *receiver, const char *slot,
                              const QString &iconName, const QString &text);

    /** Add space. */
    void addToolBarSpace();

    /** Show window. */
    void show();

signals:

public slots:

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
