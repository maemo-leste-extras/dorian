#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>
#include <QMainWindow>

class QListView;
class QPushButton;
class QModelIndex;
class QCloseEvent;
class Book;
class InfoWindow;
class SortedLibrary;

class LibraryDialog: public QMainWindow
{
    Q_OBJECT

public:
    explicit LibraryDialog(QWidget *parent = 0);

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

protected:
    void closeEvent(QCloseEvent *event);

private:
    QString createItemText(const Book *book);
    void setSelected(const QModelIndex &index);
    QModelIndex selected() const;
    QListView *list;
    SortedLibrary *sortedLibrary;
#ifndef Q_WS_MAEMO_5
    QPushButton *detailsButton;
    QPushButton *removeButton;
    QPushButton *readButton;
#endif // Q_WS_MAEMO_5
    QPushButton *addButton;
};

#endif // LIBRARYDIALOG_H
