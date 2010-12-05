#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QToolBar;

/**
 * Main window with a toolbar.
 */
class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

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

signals:

public slots:

protected:
    /** Handle resize event: Manage tool bar visibility. */
    void resizeEvent(QResizeEvent *event);

    /** Handle show event: Manage tool bar visibility. */
    void showEvent(QShowEvent *event);

#ifdef Q_OS_SYMBIAN
    /** Update toolbar visibility. */
    void updateToolBar();

    /** Return true in portrait mode. */
    bool portrait();
#endif // Q_OS_SYMBIAN

    /** Hide tool bar if visible. */
    void hideToolBar();

private:
    QToolBar *toolBar;      /**< Tool bar. */
};

#endif // MAINWINDOW_H
