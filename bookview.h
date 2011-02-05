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
class QTimer;

/** Visual representation of a book. */
class BookView: public QWebView
{
    Q_OBJECT

public:
    explicit BookView(QWidget *parent = 0);

    /** Set current book. */
    void setBook(Book *book);

    /** Get current book. */
    Book *book();

    /** Go to the position decribed by "bookmark". */
    void goToBookmark(const Book::Bookmark &bookmark);

    /** Add bookmark to book at the current position. */
    void addBookmark(const QString &note);

    /** Save current reading position into book. */
    void setLastBookmark(bool fast = false);

    /** Go to given part + part fragment URL. */
    void goToPart(int part, const QString &fragment);

    /** Go to given fragment URL in current part. */
    void goToFragment(const QString &fragment);

    /** If grab is true, volume keys will generate act as page up/down. */
    void grabVolumeKeys(bool grab);

    /** Schedule restoring last reading position after loading part. */
    void scheduleRestoreAfterLoad();

    /** Schedule restoring last reading position. */
    void scheduleRestoreLastBookmark();

signals:
    /** Part loading started. */
    void partLoadStart(int index);

    /** Part loading finished. */
    void partLoadEnd(int index);

    /** Signal progress in reading the book. */
    void progress(qreal p);

public slots:
    /** Go to next part. */
    void goPrevious();

    /** Go to previous part. */
    void goNext();

    /** Actions to perform after URL loading finished. */
    void onLoadFinished(bool ok);

    /** Handle settings changes. */
    void onSettingsChanged(const QString &key);

    /** Add QObjects to the main frame. */
    void addJavaScriptObjects();

    /** Go to previous page. */
    void goPreviousPage();

    /** Go to next page. */
    void goNextPage();

protected slots:
    /** Restore last reading position after part loading finished. */
    void restoreAfterLoad();

    /** Restore book's last reading position, load new part if needed. */
    void restoreLastBookmark();

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
    void hideEvent(QHideEvent *e);

    /** Load given part. */
    void loadContent(int index);

    /** Get temporary directory for extracting book contents. */
    QString tmpPath();

    /** Go to a given (relative) position in current part. */
    void goToPosition(qreal position);

    /** Show reading progress. */
    void showProgress();

private:
    int contentIndex;   /**< Current part in book. */
    Book *mBook;        /**< Book to show. */
    bool restorePositionAfterLoad;
                        /**< If true, restore reading position after load. */
    qreal positionAfterLoad;
                        /**< Position to be restored after load. */
    bool restoreFragmentAfterLoad;
                        /**< If true, restore fragment location after load. */
    QString fragmentAfterLoad;
                        /**< Fragment location to be restored after load. */
    QImage bookmarkImage;
                        /**< Bookmark icon pre-loaded. */
    bool loaded;        /**< True, if content has been loaded. */
    bool mousePressed;  /**< Event filter's mouse button state. */
    bool grabbingVolumeKeys;
                        /**< True, if volume keys should be grabbed. */
    QTimer *restoreTimer;
                        /**< Single timer for scheduling all restore ops. */

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    int scrollerMonitor;/**< ID of timer monitoring kinetic scroll. */
#endif

#if defined(Q_WS_MAEMO_5)
    QAbstractKineticScroller *scroller;
#endif

#if defined(Q_OS_SYMBIAN)
    FlickCharm *charm;  /**< Kinetic scroller. */
#endif
};

#endif // BOOKVIEW_H
