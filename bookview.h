#ifndef BOOKVIEW_H
#define BOOKVIEW_H

#include <QWebView>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QImage>
#include <QPoint>

#include "book.h"

#ifdef Q_OS_SYMBIAN
#   include "mediakeysobserver.h"
#endif

class QModelIndex;
class Progress;
class QAbstractKineticScroller;
class ProgressDialog;
class FlickCharm;

/** Visual representation of a book. */
class BookView: public QWebView
{
    Q_OBJECT

public:
    explicit BookView(QWidget *parent = 0);
    void setBook(Book *book);
    Book *book();
    void goToBookmark(const Book::Bookmark &bookmark);
    void addBookmark(const QString &note);
    void setLastBookmark();
    void restoreLastBookmark();
    void goToPart(int part, const QString &fragment);
    void goToFragment(const QString &fragment);

    /** If grab is true, volume keys will generate act as page up/down. */
    void grabVolumeKeys(bool grab);

signals:
    void partLoadStart(int index);
    void partLoadEnd(int index);

    /** Signal button press, when the real event has been suppressed. */
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

protected slots:
#ifdef Q_OS_SYMBIAN
    /** Observe media keys. */
    void onMediaKeysPressed(MediaKeysObserver::MediaKeys key);
#endif

protected:
    void paintEvent(QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *);
    bool eventFilter(QObject *o, QEvent *e);
    void timerEvent(QTimerEvent *e);
#ifdef Q_WS_MAEMO_5
    void leaveEvent(QEvent *e);
    void enterEvent(QEvent *e);
#endif // Q_WS_MAEMO_5

    /** Load given part. */
    void loadContent(int index);

    /** Get temporary directory for extracting book contents. */
    QString tmpPath();

    /** Go to a given (relative) position in current part. */
    void goToPosition(qreal position);

    /** Show reading progress. */
    void showProgress();

    int contentIndex;       /**< Current part in book. */
    Book *mBook;            /**< Book to show. */
    bool restorePositionAfterLoad;
                            /**< If true, restore current position after load. */
    qreal positionAfterLoad;/**< Position to be restored after load. */
    bool restoreFragmentAfterLoad;
                            /**< If true, restore fragment location after load. */
    QString fragmentAfterLoad;
                            /**< Fragment location to be restored after load. */
    QImage bookmarkImage;   /**< Bookmark icon pre-loaded. */
    bool loaded;            /**< True, if content has been loaded. */
    bool mousePressed;      /**< Event filter's mouse button state. */
    int contentsHeight;     /**< Last know height of the frame. */
    bool grabbingVolumeKeys;/**< True, if volume keys should be grabbed. */

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    int scrollerMonitor;    /**< ID of timer monitoring kinetic scroll. */
#endif
#if defined(Q_WS_MAEMO_5)
    QAbstractKineticScroller *scroller;
#elif defined(Q_OS_SYMBIAN)
    FlickCharm *charm;
#endif
};

#endif // BOOKVIEW_H
