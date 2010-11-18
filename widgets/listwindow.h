#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QList>

class QString;
class QPushButton;
class FlickCharm;
class QAbstractItemModel;
class QListWidget;
class QModelIndex;

/** A window with a list and menu actions (Maemo) or buttons (non-Maemo). */
class ListWindow: public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * @param   model   Model for the list widget contents.
     * @param   parent  Parent widget.
     */
    explicit ListWindow(QWidget *parent = 0);

    /**
     * Set the model for the list.
     */
    void setModel(QAbstractItemModel *model);

    /**
     * Add an action button to the beginning of the list.
     */
    void addButton(const QString &title, QObject *receiver, const char *slot,
                   const QString &iconPath = QString());

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
    void onItemActivated(const QModelIndex &);
    void populateList();

protected:
    QListWidget *list;
    QAbstractItemModel *model;
    QList<QPushButton *> buttons;
#ifdef Q_OS_SYMBIAN
    FlickCharm *charm;
#endif
};

#endif // LISTWINDOW_H
