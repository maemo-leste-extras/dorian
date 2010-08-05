#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QList>

class QListView;
class QString;
class QHBoxLayout;
class QPushButton;
class QModelIndex;
class QItemSelection;

/** A window with a list and menu actions (Maemo) or buttons (non-Maemo). */
class ListWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0);

    /** Add a list view to the window. */
    void addList(QListView *list);

    /**
     * Add an action to the window: either a button, or, on Maemo, a top
     * level menu item.
     * Activating the action invokes the slot with no parameters.
     */
    void addAction(const QString &title, QObject *receiver, const char *slot,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::ActionRole);

    /**
     * Add an action to the selected item in the list: either a button which is
     * enabled when a list item is selected, or, on Maemo, a pop-up menu item
     * which is displayed when a list item is long-pressed.
     * Activating the action invokes the slot with no parameters.
     */
    void addItemAction(const QString &title, QObject *receiver,
                       const char *slot);

protected slots:
    void onSelectionChanged(const QItemSelection &selected,
                            const QItemSelection &deselected);
#ifndef Q_WS_MAEMO_5
    void activateItemButtons();
#endif

protected:
#ifdef Q_WS_MAEMO_5
    void closeEvent(QCloseEvent *event);
#else
    QDialogButtonBox *buttonBox;
    QList<QPushButton *> itemButtons;
#endif
    QHBoxLayout *frameLayout;
    QListView *list;
};

#endif // LISTWINDOW_H
