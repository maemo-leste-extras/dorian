#ifndef CHAPTERSDIALOG_H
#define CHAPTERSDIALOG_H

#include "listwindow.h"

class QWidget;
class QListWidget;
class QListWidgetItem;
class Book;

/** Display book chapters. */
class ChaptersDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit ChaptersDialog(Book *book, QWidget *parent = 0);

signals:
    void goToChapter(int index);

public slots:
    void onItemActivated(QListWidgetItem *);

protected:
    QListWidget *list;
};

#endif // CHAPTERSDIALOG_H
