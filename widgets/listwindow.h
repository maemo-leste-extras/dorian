#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include "mainbase.h"

#include <QDialogButtonBox>
#include <QList>

class QString;
class QPushButton;
class FlickCharm;
class QAbstractItemModel;
class QListWidget;
class QModelIndex;
class QListWidgetItem;

/** A window with a list and menu actions (Maemo) or buttons (non-Maemo). */
class ListWindow: public MainBase
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param   noItems Text to display when the list has no items.
     * @param   parent  Parent widget.
     */
    explicit ListWindow(const QString &noItems, QWidget *parent = 0);

    /** Set the model for the list. */
    void setModel(QAbstractItemModel *model);

    /** Get model. */
    QAbstractItemModel *model() const;

    /**
     * Add an action button to the beginning of the list (Maemo) or to the
     * tool bar (non-Maemo).
     */
    void addButton(const QString &title, QObject *receiver, const char *slot,
                   const QString &iconPath = QString());

    /**
     * Add an action button to the tool bar, which is only active if a list
     * item is selected.
     */
    void addItemButton(const QString &title, QObject *receiver,
                       const char *slot, const QString &iconPath = QString());

    /** Add an action to the menu. */
    QAction *addMenuAction(const QString &title, QObject *receiver,
                           const char *slot);

    /** Get current (selected) item. */
    QModelIndex currentItem() const;

signals:
    /** Emitted when a list item is activated. */
    void activated(const QModelIndex &index);

    /** Emitted when selection has changed. */
    void itemSelectionChanged();

public slots:
    /** Set the current (selected) item. */
    void setCurrentItem(const QModelIndex &item);

protected slots:
    void onItemActivated(const QModelIndex &);
    void populateList();
    void onItemSelectionChanged();

protected:
    struct Button {
        QString title;
        QObject *receiver;
        const char *slot;
        QString iconName;
    };
    void insertButton(int row, const Button &button);
#ifdef Q_WS_MAEMO_5
    void closeEvent(QCloseEvent *event);
#endif

private:
    QListWidget *list;
    QAbstractItemModel *mModel;
    QList<Button> buttons;
    QString noItems;
#ifdef Q_OS_SYMBIAN
    FlickCharm *charm;
#endif
    QList<QAction *>itemActions;
};

#endif // LISTWINDOW_H
