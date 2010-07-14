#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <QDialog>
#include <QString>
#include <QModelIndexList>

class QMainWindow;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class Book;
class InfoWindow;

class LibraryDialog: public QDialog
{
    Q_OBJECT

public:
    explicit LibraryDialog(QWidget *parent = 0);
    QListWidget *list;
#ifndef Q_WS_MAEMO_5
    QPushButton *detailsButton;
    QPushButton *removeButton;
    QPushButton *readButton;
#endif
    QPushButton *addButton;
    QString lastDir;

public slots:
    void onAdd();
#ifndef Q_WS_MAEMO_5
    void onRemove();
    void onDetails();
    void onRead();
    void onItemSelectionChanged();
#endif
    void onBookAdded();
    void onItemActivated(QListWidgetItem *item);
    void onCurrentBookChanged();

private:
    QString createItemText(const Book *book);
};

#endif // LIBRARYDIALOG_H
