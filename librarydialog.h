#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>

#include "listwindow.h"

class QPushButton;
class QModelIndex;
class QCloseEvent;
class ProgressDialog;
class Book;
class InfoWindow;
class SortedLibrary;
class QAction;
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
    void onBookAdded();
    void onItemActivated(const QModelIndex &index);
    void onAddFromFolder(const QString &path);
    void onAddFromFolderDone(int added);
    void onSearch();
    void showSearchResults();
    void onSortByAuthor();
    void onSortByTitle();

private:
    QString createItemText(Book *book);
    void setSelected(const QModelIndex &index);
    SortedLibrary *sortedLibrary;
    ProgressDialog *progress;
    QAction *sortByAuthor;
    QAction *sortByTitle;
    SearchDialog *searchDialog;
};

#endif // LIBRARYDIALOG_H
