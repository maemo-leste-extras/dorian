#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QList>

class QListView;
class QString;
class QBoxLayout;
class QPushButton;
class QModelIndex;
class QItemSelection;
class QEvent;
class ListView;
class FlickCharm;

/** A window with a list and menu actions (Maemo) or buttons (non-Maemo). */
class ListWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0);

    /** Add a list view to the window. */
    void addList(ListView *list);

    /**
     * Add an action to the window: either a button, or, on Maemo, a top
     * level menu item.
     * Activating the action invokes the slot with no parameters.
     */
    void addAction(const QString &title, QObject *receiver, const char *slot,
        const QString &iconPath = QString(),
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::ActionRole);

    /**
     * Add an action to the selected item in the list: either a button which is
     * enabled when a list item is selected, or, on Maemo, a pop-up menu item
     * which is displayed when a list item is long-pressed.
     * Activating the action invokes the slot with no parameters.
     */
    void addItemAction(const QString &title, QObject *receiver,
                       const char *slot);

    /**
      * Add an action to the menu.
      */
    QAction *addMenuAction(const QString &title, QObject *receiver,
                           const char *slot);

public slots:
#ifdef Q_OS_SYMBIAN
    void show();
#endif

protected slots:
    void onSelectionChanged(const QItemSelection &selected,
                            const QItemSelection &deselected);
#ifdef Q_WS_MAEMO_5
    void onModelChanged();
#else
    void activateItemButtons();
#endif

protected:
#ifdef Q_WS_MAEMO_5
    bool eventFilter(QObject *obj, QEvent *event);
    void closeEvent(QCloseEvent *event);
    QMenu *popup;
#else
    QDialogButtonBox *buttonBox;
    QList<QPushButton *> itemButtons;
#endif // Q_WS_MAEMO_5
#ifdef Q_OS_SYMBIAN
    FlickCharm *charm;
#endif
    QBoxLayout *contentLayout;
    ListView *list;
};

#endif // LISTWINDOW_H
