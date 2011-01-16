#ifndef MAINBASE_H
#define MAINBASE_H

#include <QMainWindow>

class QToolBar;

/**
 * Main window with a toolbar.
 */
class MainBase: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainBase(QWidget *parent = 0);

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

    /** Add tool bar. */
    void addToolBar();

    /** Add spacing to tool bar. */
    void addToolBarSpace();

public slots:
    void show();

protected:
    /** Return the height of the tool bar (or 0 if there is no tool bar). */
    int toolBarHeight();

#if 0 // ifdef Q_OS_SYMBIAN
    /** Handle resize. */
    void resizeEvent(QResizeEvent *e);
#endif

private:
    QToolBar *toolBar;
};

#endif // MAINBASE_H
