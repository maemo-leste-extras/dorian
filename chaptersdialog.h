#ifndef CHAPTERSDIALOG_H
#define CHAPTERSDIALOG_H

#include <QMainWindow>

class QWidget;
class QListWidget;
class QListWidgetItem;
class Book;

/** Display book chapters. */
class ChaptersDialog: public QMainWindow
{
    Q_OBJECT

public:
    explicit ChaptersDialog(Book *book, QWidget *parent = 0);

signals:
    void goToChapter(int index);

public slots:
    void onItemActivated(QListWidgetItem *);
    void onClose();

protected:
    Book *book;
    QListWidget *list;
};

#endif // CHAPTERSDIALOG_H
