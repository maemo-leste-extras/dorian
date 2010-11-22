#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QMainWindow>
#include <QList>

class QWidget;
class QToolBar;
class QAction;
class BookView;
class QVBoxLayout;

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

    /** Return true if a child is currently adopted. */
    bool hasChild(QWidget *child);

    /**
     * Add action that is visible on the tool bar.
     * @param   receiver    Object receiving "activated" signal.
     * @param   slot        Slot receiving "activated" signal.
     * @param   iconName    Base name of tool bar icon in resource file.
     * @param   text        Tool bar item text.
     * @param   important   On Symbian, only "important" actions are added to
     *                      the tool bar. All actions are added to the Options
     *                      menu though.
     */
    QAction *addToolBarAction(QObject *receiver, const char *slot,
                              const QString &iconName, const QString &text,
                              bool important = false);

    /** Add spacing to tool bar. */
    void addToolBarSpace();

    /** Show window. */
    void show();

    /** If grab is true, volume keys will generate pageUp/Down key events. */
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
    QToolBar *toolBar;
};

#endif // ADOPTERWINDOW_H
