#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>

class QMainWindow;
class QListView;
class QPushButton;
class QModelIndex;
class Book;
class InfoWindow;
class SortedLibrary;

class LibraryDialog: public QDialog
{
    Q_OBJECT

public:
    explicit LibraryDialog(QWidget *parent = 0);
    QListView *list;
    SortedLibrary *sortedLibrary;
#ifndef Q_WS_MAEMO_5
    QPushButton *detailsButton;
    QPushButton *removeButton;
    QPushButton *readButton;
#endif // Q_WS_MAEMO_5
    QPushButton *addButton;

public slots:
    void onAdd();
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
};

#endif // LIBRARYDIALOG_H
