#ifndef CHAPTERSDIALOG_H
#define CHAPTERSDIALOG_H

#include <QStringList>

#include "listwindow.h"

class QWidget;
class QModelIndex;
class ListView;
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
    void onItemActivated(const QModelIndex &index);

protected:
    ListView *list;
    QStringList data;
};

#endif // CHAPTERSDIALOG_H
