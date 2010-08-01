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
    void restoreLastBookmark();

signals:
    void chapterLoadStart(int index);
    void chapterLoadEnd(int index);

    /** Signal button press when the real event has been suppressed. */
    void suppressedMouseButtonPress();

public slots:
    void goPrevious();
    void goNext();
    void onLoadFinished(bool ok);
    void onSettingsChanged(const QString &key);

    /** Add QObjects to the main frame. */
    void addJavaScriptObjects();

    /** Handle main frame contents size changes. */
    void onContentsSizeChanged(const QSize &size);

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    virtual void leaveEvent(QEvent *);
    virtual void enterEvent(QEvent *);

private:
    /** Save navigation icons from resource to the file system. */
    void extractIcons();

    /** Remove extracted icons. */
    void removeIcons();

    /** Load given chapter. */
    void loadContent(int index);

    /** Decorate web page frame with navigation icons. */
    void addNavigationBar();

    /** Get temporary directory for extracting book contents. */
    QString tmpPath();

    /** Go to a given (relative) position in current chapter. */
    void goToPosition(qreal position);

    int contentIndex;   /**< Current chapter in book. */
    Book *mBook;        /**< Book to show. */
    bool restorePositionAfterLoad;
                        /**< If true, restoring position after load is needed. */
    qreal positionAfterLoad;
                        /**< Position to be restored after load. */
    QImage bookmarkImage;
                        /**< Bookmark icon pre-loaded. */
    bool loaded;        /**< True if content has been loaded. */
    bool mousePressed;
    int contentsHeight; /**< Last know height of the frame. */
    bool decorated;     /**< True after adding the arrows to the frame contents. */
};

#endif // BOOKVIEW_H
