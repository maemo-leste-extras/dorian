#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QWebView>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QImage>
#include <QPoint>

#include "book.h"

class QModelIndex;
class Progress;
class QAbstractKineticScroller;

/** Visual representation of a book. */
class BookView: public QWebView
#ifdef Q_OS_SYMBIAN
        , public Flickable
#endif
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
    void partLoadStart(int index);
    void partLoadEnd(int index);

    /** Signal button press when the real event has been suppressed. */
    void suppressedMouseButtonPress();

    /** Signal progress in reading the book. */
    void progress(qreal p);

public slots:
    /** Go to next part. */
    void goPrevious();

    /** Go to previous part. */
    void goNext();

    void onLoadFinished(bool ok);
    void onSettingsChanged(const QString &key);

    /** Add QObjects to the main frame. */
    void addJavaScriptObjects();

    /** Handle main frame contents size changes. */
    void onContentsSizeChanged(const QSize &size);

    /** Go to previous page. */
    void goPreviousPage();

    /** Go to next page. */
    void goNextPage();

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *);
    bool eventFilter(QObject *o, QEvent *e);
    void leaveEvent(QEvent *e);
    void enterEvent(QEvent *e);
    void timerEvent(QTimerEvent *e);
    void keyPressEvent(QKeyEvent *e);

private:
    /** Load given part. */
    void loadContent(int index);

    /** Get temporary directory for extracting book contents. */
    QString tmpPath();

    /** Go to a given (relative) position in current part. */
    void goToPosition(qreal position);

    /** Show progress. */
    void showProgress();

    int contentIndex;   /**< Current part in book. */
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

#if def(Q_WS_MAEMO_5)
    int scrollerMonitor;
    QAbstractKineticScroller *scroller;
};

#endif // BOOKVIEW_H
