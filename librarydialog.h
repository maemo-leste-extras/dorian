#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>

#include "listwindow.h"

class ListView;
class QPushButton;
class QModelIndex;
class QCloseEvent;
class QProgressDialog;
class Book;
class InfoWindow;
class SortedLibrary;
class SearchDialog;

/** Manage library. */
class LibraryDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit LibraryDialog(QWidget *parent = 0);

public slots:
    void onAdd();
    void onAddFolder();
#ifndef Q_WS_MAEMO_5
    void onRemove();
    void onDetails();
    void onRead();
#endif // Q_WS_MAEMO_5
    void onBookAdded();
    void onItemActivated(const QModelIndex &index);
    void onCurrentBookChanged();
    void onAddFromFolder(const QString &path);
    void onAddFromFolderDone(int added);
    void onSearch();
    void showSearchResults();

private:
    QString createItemText(const Book *book);
    void setSelected(const QModelIndex &index);
    QModelIndex selected() const;
    ListView *list;
    SortedLibrary *sortedLibrary;
    QProgressDialog *progress;
    SearchDialog *searchDialog;
};

#endif // LIBRARYDIALOG_H
