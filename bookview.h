#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QWebView>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QImage>

#include "book.h"

class QModelIndex;

/** Visual representation of a book. */
class BookView: public QWebView
{
    Q_OBJECT

public:
    explicit BookView(QWidget *parent = 0);
    virtual ~BookView();
    void setBook(Book *book);
    Book *book();
    void goToBookmark(const Book::Bookmark &bookmark);
    void addBookmark();
    void setLastBookmark();

signals:
    void chapterLoadStart(int index);
    void chapterLoadEnd(int index);

public slots:
    void goPrevious();
    void goNext();
    void onLoadFinished(bool ok);
    void onSettingsChanged(const QString &key);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);

private:
    void loadContent(int index);
    void addNavigationBar();
    void extractIcons();
    void removeIcons();
    QString tmpPath();

    int contentIndex;
    Book *mBook;
    bool restore;
    qreal positionAfterLoad;
    QImage bookmarkImage;
    bool loaded;
};

#endif // BOOKVIEW_H
