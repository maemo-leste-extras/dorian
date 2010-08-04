#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>

#include "listwindow.h"

class QListView;
class QPushButton;
class QModelIndex;
class QCloseEvent;
class Book;
class InfoWindow;
class SortedLibrary;

class LibraryDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit LibraryDialog(QWidget *parent = 0);

public slots:
    void onAdd();
    void onShowFolders();
#ifndef Q_WS_MAEMO_5
    void onRemove();
    void onDetails();
    void onRead();
    void onItemSelectionChanged();
#endif // Q_WS_MAEMO_5
    void onBookAdded();
    void onItemActivated(const QModelIndex &index);
    void onCurrentBookChanged();

private:
    QString createItemText(const Book *book);
    void setSelected(const QModelIndex &index);
    QModelIndex selected() const;
    QListView *list;
    SortedLibrary *sortedLibrary;
};

#endif // LIBRARYDIALOG_H
