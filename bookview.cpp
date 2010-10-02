#include <QDebug>
#include <QWebFrame>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#include <QTimer>

#ifdef Q_WS_MAEMO_5
#   include <QAbstractKineticScroller>
#endif

#include "book.h"
#include "bookview.h"
#include "library.h"
#include "settings.h"
#include "trace.h"
#include "progress.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

BookView::BookView(QWidget *parent):
    QWebView(parent), contentIndex(-1), mBook(0),
    restorePositionAfterLoad(false), positionAfterLoad(0), loaded(false),
    contentsHeight(0)
{
    Trace t("BookView::BookView");
    settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::JavaEnabled, false);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, false);
    settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled, false);
    settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,
                             false);
    settings()->setAttribute(QWebSettings::LocalStorageEnabled, false);
    settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,
                             false);
    settings()->setDefaultTextEncoding("utf-8");
    page()->setContentEditable(false);

#if defined(Q_WS_MAEMO_5)
    // Suppress unwanted text selections on Maemo
    installEventFilter(this);
#endif
    QWebFrame *frame = page()->mainFrame();
#if defined(Q_WS_MAEMO_5)
    frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
#endif
    frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    bookmarkImage = QImage(":/icons/bookmark.png");

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObjects()));
    connect(frame, SIGNAL(contentsSizeChanged(const QSize &)),
            this, SLOT(onContentsSizeChanged(const QSize &)));
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    Settings *s = Settings::instance();
    s->setValue("zoom", s->value("zoom", 160));
    s->setValue("font", s->value("font",
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_X11)
                                 "Serif"
#elif defined(Q_WS_MAC)
                                 "Hoefler Text"
#else
                                 "Times New Roman"
#endif
                                 ));
    s->setValue("scheme", s->value("scheme", "default"));
    setBook(0);

#ifdef Q_WS_MAEMO_5
    scrollerMonitor = 0;
    scroller = property("kineticScroller").value<QAbstractKineticScroller *>();
#endif
}

BookView::~BookView()
{
    Trace t("BookView::~BookView");
}

void BookView::loadContent(int index)
{
    Trace t("BookView::loadContent");
    if (!mBook) {
        return;
    }
    if ((index < 0) || (index >= mBook->parts.size())) {
        return;
    }

    QString contentFile(mBook->content[mBook->parts[index]].href);
    if (mBook->parts[index] == "error") {
        setHtml(contentFile);
    }
    else {
        loaded = false;
        emit partLoadStart(index);
        QUrl u = QUrl::fromLocalFile(QDir(mBook->rootPath()).
                                     absoluteFilePath(contentFile));
        qDebug() << "Loading" << u;
        load(u);
    }
    contentIndex = index;
}

void BookView::setBook(Book *book)
{
    Trace t("BookView::setBook");

    // Save position in current book
    setLastBookmark();

    // Open new book, restore last position
    if (book != mBook) {
        mBook = book;
        if (book) {
            contentIndex = -1;
            if (book->open()) {
                restoreLastBookmark();
            } else {
                mBook = 0;
                contentIndex = 0;
                setHtml(tr("Failed to open book"));
            }
        }
        else {
            contentIndex = 0;
            setHtml(tr("No book"));
        }
    }
}

Book *BookView::book()
{
    return mBook;
}

void BookView::goPrevious()
{
    Trace t("BookView::goPrevious");
    if (mBook && (contentIndex > 0)) {
        mBook->setLastBookmark(contentIndex - 1, 0);
        loadContent(contentIndex - 1);
    }
}

void BookView::goNext()
{
    Trace t("BookView::goNext");
    if (mBook && (contentIndex < (mBook->parts.size() - 1))) {
        mBook->setLastBookmark(contentIndex + 1, 0);
        loadContent(contentIndex + 1);
    }
}

void BookView::setLastBookmark()
{
    Trace t("BookView::setLastBookmark");
    if (mBook) {
        int height = contentsHeight;
        int pos = page()->mainFrame()->scrollPosition().y();
        qDebug() << QString("At %1 (%2%, height %3)").
                arg(pos).arg((qreal)pos / (qreal)height * 100).arg(height);
        mBook->setLastBookmark(contentIndex, (qreal)pos / (qreal)height);
    }
}

void BookView::restoreLastBookmark()
{
    Trace t("BookView::restoreLastBookmark");
    if (mBook) {
        goToBookmark(mBook->lastBookmark());
    }
}

void BookView::goToBookmark(const Book::Bookmark &bookmark)
{
    Trace t("BookView::goToBookmark");
    if (mBook) {
        if (bookmark.part != contentIndex) {
            qDebug () << "Loading new part" << bookmark.part;
            mBook->setLastBookmark(bookmark.part, bookmark.pos);
            restorePositionAfterLoad = true;
            positionAfterLoad = bookmark.pos;
            loadContent(bookmark.part);
        } else {
            goToPosition(bookmark.pos);
        }
    }
}

void BookView::onLoadFinished(bool ok)
{
    Trace t("BookView::onLoadFinished");
    if (!ok) {
        qDebug() << "Not OK";
        return;
    }
    loaded = true;
    onSettingsChanged("scheme");
    emit partLoadEnd(contentIndex);
    showProgress();
}

void BookView::onSettingsChanged(const QString &key)
{
    Trace t("BookView::onSettingsChanged " + key);
    if (key == "zoom") {
        setZoomFactor(Settings::instance()->value(key).toFloat() / 100.);
    }
    else if (key == "font") {
        QString face = Settings::instance()->value("font").toString();
        settings()->setFontFamily(QWebSettings::StandardFont, face);
    }
    else if (key == "scheme") {
        QWebFrame *frame = page()->mainFrame();
        QString scheme = Settings::instance()->value("scheme").toString();
        if ((scheme != "day") && (scheme != "night") && (scheme != "sand") &&
            (scheme != "default")) {
            scheme = "default";
        }
        QFile script(":/styles/" + scheme + ".js");
        script.open(QFile::ReadOnly);
        QString scriptText = script.readAll();
        script.close();
        QVariant ret = frame->evaluateJavaScript(scriptText);
    }
}

void BookView::paintEvent(QPaintEvent *e)
{
    QWebView::paintEvent(e);
    if (!mBook || !loaded) {
        return;
    }

    // Paint bookmarks
    QPoint scrollPos = page()->mainFrame()->scrollPosition();
    QPixmap bookmarkPixmap = QPixmap::fromImage(bookmarkImage);
    QPainter painter(this);
    foreach (Book::Bookmark b, mBook->bookmarks()) {
        if (b.part != contentIndex) {
            continue;
        }
        int height = contentsHeight;
        int bookmarkPos = (int)((qreal)height * (qreal)b.pos);
        painter.drawPixmap(2, bookmarkPos - scrollPos.y(), bookmarkPixmap);
    }
}

void BookView::mousePressEvent(QMouseEvent *e)
{
    QWebView::mousePressEvent(e);
#ifdef Q_WS_MAEMO_5
    // Start monitoring kinetic scroll
    if (scrollerMonitor) {
        killTimer(scrollerMonitor);
        scrollerMonitor = 0;
    }
    if (scroller) {
        scrollerMonitor = startTimer(500);
    }
#else
    // Handle mouse presses on the scroll bar
    QWebFrame *frame = page()->mainFrame();
    if (frame->scrollBarGeometry(Qt::Vertical).contains(e->pos())) {
        e->accept();
        return;
    }
#endif // Q_WS_MAEMO_5
    e->ignore();
}

void BookView::wheelEvent(QWheelEvent *e)
{
    QWebView::wheelEvent(e);
    showProgress();
}

void BookView::addBookmark()
{
    Trace t("BookView::addBookmark");
    if (!mBook) {
        return;
    }
    int y = page()->mainFrame()->scrollPosition().y();
    int height = page()->mainFrame()->contentsSize().height();
    qDebug() << ((qreal)y / (qreal)height);
    mBook->addBookmark(contentIndex, (qreal)y / (qreal)height);
    update();
}

QString BookView::tmpPath()
{
    return QDir::tempPath() + "/dorian";
}

bool BookView::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::Paint && e->type() != QEvent::MouseMove) {
        if (e->type() == QEvent::Resize) {
            qDebug() << "BookView::eventFilter QEvent::Resize to"
                    << page()->mainFrame()->contentsSize().height();
        } else if (e->type() == QEvent::Timer) {
            qDebug() << "BookView::eventFilter" << "QEvent::Timer"
                    << ((QTimerEvent *)e)->timerId();
        } else {
            qDebug() << "BookView::eventFilter" << Trace::event(e->type());
        }
    }

    // Work around Qt bug that sometimes selects web view contents during swipe
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        emit suppressedMouseButtonPress();
        mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
        showProgress();
        mousePressed = false;
        break;
    case QEvent::MouseMove:
        if (mousePressed) {
            return true;
        }
        break;
    case QEvent::MouseButtonDblClick:
        return true;
    default:
        break;
    }

    return QObject::eventFilter(o, e);
}

void BookView::addJavaScriptObjects()
{
    page()->mainFrame()->addToJavaScriptWindowObject("bv", this);
}

void BookView::onContentsSizeChanged(const QSize &size)
{
    contentsHeight = size.height();
    if (restorePositionAfterLoad) {
        qDebug() << "BookView::onContentSizeChanged: Time to restore";
        restorePositionAfterLoad = false;
        goToPosition(positionAfterLoad);
    }
}

void BookView::leaveEvent(QEvent *e)
{
    Trace t("BookView::leaveEvent");
    // Save current position, to be restored later
    setLastBookmark();
    QWebView::leaveEvent(e);
}

void BookView::enterEvent(QEvent *e)
{
    Trace t("BookView::enterEvent");
    // Restore position saved at Leave event. This seems to be required,
    // after temporarily switching from portrait to landscape and back
    restoreLastBookmark();
    QWebView::enterEvent(e);
}

void BookView::goToPosition(qreal position)
{
    int scrollPos = (int)((qreal)contentsHeight * position);
    page()->mainFrame()->setScrollPosition(QPoint(0, scrollPos));
    // FIXME: update();
    qDebug() << "BookView::goToPosition: To" << scrollPos << "("
            << (position * 100) << "%, height" << contentsHeight << ")";
}

void BookView::showProgress()
{
    if (mBook) {
        qreal pos = (qreal)(page()->mainFrame()->scrollPosition().y()) /
                    (qreal)contentsHeight;
        emit progress(mBook->getProgress(contentIndex, pos));
    }
}

void BookView::timerEvent(QTimerEvent *e)
{
#ifdef Q_WS_MAEMO_5
    if (e->timerId() == scrollerMonitor) {
        if (scroller &&
            ((scroller->state() == QAbstractKineticScroller::AutoScrolling) ||
             (scroller->state() == QAbstractKineticScroller::Pushing))) {
            showProgress();
        } else {
            killTimer(scrollerMonitor);
        }
    }
#else
    Q_UNUSED(e);
#endif // Q_WS_MAEMO_5
}

void BookView::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_F7:
        goNextPage();
        event->accept();
        break;
    case Qt::Key_F8:
        goPreviousPage();
        event->accept();
        break;
    default:
        ;
    }
    QWebView::keyPressEvent(event);
}

void BookView::goPreviousPage()
{
    QWebFrame *frame = page()->mainFrame();
    int pos = frame->scrollPosition().y();
    frame->scroll(0, -height());
    if (pos == frame->scrollPosition().y()) {
        if (contentIndex > 0) {
            Book::Bookmark bookmark(contentIndex - 1, 1.0);
            mBook->setLastBookmark(contentIndex - 1, 1.0);
            goToBookmark(bookmark);
        }
    } else {
        showProgress();
    }
}

void BookView::goNextPage()
{
    Trace t("BookView::goNextPage");
    QWebFrame *frame = page()->mainFrame();
    int pos = frame->scrollPosition().y();
    frame->scroll(0, height());
    if (pos == frame->scrollPosition().y()) {
        goNext();
    } else {
        setLastBookmark();
        showProgress();
    }
}
