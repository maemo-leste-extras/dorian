#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QMainWindow>
#include <QList>

class QWidget;
class QToolBar;
class QAction;
class BookView;

/**
 * A toplevel window that can adopt a BookView and other children.
 * On Maemo, it can also grab the volume keys.
 */
class AdopterWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit AdopterWindow(QWidget *parent = 0);

    /** Adopt children "bookView" and "others". */
    void takeChildren(BookView *bookView, const QList<QWidget *> &others);

    /** Release current children (adopted in @see takeChildren). */
    void leaveChildren();

    /**
     * Add action that is visible on the tool bar (except on Symbian, where
     * it is visible on the Options menu).
     */
    QAction *addToolBarAction(QObject *receiver, const char *slot,
                              const QString &iconName, const QString &text);

    /** Add spacing to tool bar. */
    void addToolBarSpace();

    /** Show window. */
    void show();

    /** If grab is true, volume keys will generate pageUp/Down keys. */
    void grabVolumeKeys(bool grab);

public slots:
    /** Handle settings changes. */
    void onSettingsChanged(const QString &key);

protected:
    void keyPressEvent(QKeyEvent *event);
#ifdef Q_WS_MAEMO_5
    void showEvent(QShowEvent *event);
    void doGrabVolumeKeys(bool grab);
#endif
    BookView *bookView;
    bool grabbingVolumeKeys;/**< True, if volume keys should be grabbed. */
#ifndef Q_OS_SYMBIAN
    QToolBar *toolBar;
#endif
};

#endif // ADOPTERWINDOW_H
