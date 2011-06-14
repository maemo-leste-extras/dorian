#include <QDir>
#include <QtGui>
#include <QWebFrame>

#if defined(Q_OS_SYMBIAN)
#   include "mediakeysobserver.h"
#   include "flickcharm.h"
#endif

#include "book.h"
#include "bookview.h"
#include "library.h"
#include "settings.h"
#include "trace.h"
#include "progress.h"
#include "progressdialog.h"
#include "platform.h"

BookView::BookView(QWidget *parent): QWebView(parent), contentIndex(-1),
    mBook(0), restorePositionAfterLoad(false), positionAfterLoad(0),
    restoreFragmentAfterLoad(false), loaded(false), grabbingVolumeKeys(false)
{
    TRACE;

    // Create timer for scheduling restores
    restoreTimer = new QTimer(this);

    // Set up web view defaults
    settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::JavaEnabled, false);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebSettings::PrivateBrowsingEnabled, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard,
                             false);
    settings()->setAttribute(QWebSettings::OfflineStorageDatabaseEnabled,
                             false);
    settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,
                             false);
    settings()->setAttribute(QWebSettings::LocalStorageEnabled, false);
    settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,
                             false);
    settings()->setDefaultTextEncoding("utf-8");
    page()->setContentEditable(false);
    QWebFrame *frame = page()->mainFrame();
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
#endif
    frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(onLoadFinished(bool)));
    connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObjects()));

    // Suppress unwanted text selections on Maemo and Symbian
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    installEventFilter(this);
#endif

    // Pre-load bookmark icon
    bookmarkImage = QImage(":/icons/bookmark.png");

    // Handle settings changes, force handling initial settings
    connect(Settings::instance(), SIGNAL(valueChanged(const QString &)),
            this, SLOT(onSettingsChanged(const QString &)));
    setBook(0);

    // Enable kinetic scrolling
#if defined(Q_WS_MAEMO_5)
    scrollerMonitor = 0;
    scroller = property("kineticScroller").value<QAbstractKineticScroller *>();
#elif defined(Q_OS_SYMBIAN)
    scrollerMonitor = 0;
    charm = new FlickCharm(this);
    charm->activateOn(this);
#endif

    // Observe media keys on Symbian
#ifdef Q_OS_SYMBIAN
    MediaKeysObserver *observer = MediaKeysObserver::instance();
    connect(observer, SIGNAL(mediaKeyPressed(MediaKeysObserver::MediaKeys)),
            this, SLOT(onMediaKeysPressed(MediaKeysObserver::MediaKeys)));
#endif
}

void BookView::loadContent(int index)
{
    TRACE;

    if (!mBook) {
        return;
    }
    if ((index < 0) || (index >= mBook->parts.size())) {
        return;
    }

    QString contentFile(mBook->content[mBook->parts[index]].href);
    if (mBook->parts[index] == "error") {
        setHtml(contentFile);
    } else {
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
    TRACE;

    // Bail out if new book is the same as current book
    if (book == mBook) {
        return;
    }

    // Save reading position of current book
    setLastBookmark();

    // Set new book as the current book
    mBook = book;

    // Bail out if new book is null
    if (!book) {
        contentIndex = 0;
        setHtml(tr("No book"));
        return;
    }

    // Open new book
    if (book->open()) {
        // Restore last reading position - this will force
        // a reload as well
        contentIndex = -1;
        restoreLastBookmark();
    } else {
        mBook = 0;
        contentIndex = 0;
        setHtml(tr("Failed to open book"));
    }
}

Book *BookView::book()
{
    return mBook;
}

void BookView::goPrevious()
{
    TRACE;
    if (mBook && (contentIndex > 0)) {
        mBook->setLastBookmark(contentIndex - 1, 0);
        loadContent(contentIndex - 1);
    }
}

void BookView::goNext()
{
    TRACE;
    if (mBook && (contentIndex < (mBook->parts.size() - 1))) {
        mBook->setLastBookmark(contentIndex + 1, 0);
        loadContent(contentIndex + 1);
    }
}

void BookView::setLastBookmark(bool fast)
{
    TRACE;
    if (mBook) {
        QWebFrame *frame = page()->mainFrame();
        int height = frame->contentsSize().height();
        int pos = frame->scrollPosition().y();
        qDebug() << QString("At %1 (%2%, height %3)").
                arg(pos).arg((qreal)pos / (qreal)height * 100).arg(height);
        mBook->setLastBookmark(contentIndex, (qreal)pos / (qreal)height, fast);
    } else {
        qDebug() << "(no book)";
    }
}

void BookView::restoreLastBookmark()
{
    TRACE;
    if (mBook) {
        goToBookmark(mBook->lastBookmark());
    }
}

void BookView::goToBookmark(const Book::Bookmark &bookmark)
{
    TRACE;
    if (mBook) {
        if (bookmark.part != contentIndex) {
            qDebug () << "Loading new part" << bookmark.part;
            mBook->setLastBookmark(bookmark.part, bookmark.pos);
            restorePositionAfterLoad = true;
            positionAfterLoad = bookmark.pos;
            loadContent(bookmark.part);
        } else {
            emit partLoadEnd(contentIndex);
            goToPosition(bookmark.pos);
        }
    }
}

void BookView::goToPart(int part, const QString &fragment)
{
    TRACE;
    if (mBook) {
        if (fragment.isEmpty()) {
            goToBookmark(Book::Bookmark(part, 0));
        } else {
            if (part != contentIndex) {
                qDebug() << "Loading new part" << part;
                restoreFragmentAfterLoad = true;
                fragmentAfterLoad = fragment;
                loadContent(part);
            } else {
                goToFragment(fragment);
                showProgress();
            }
        }
    }
}

void BookView::goToFragment(const QString &fragment)
{
    TRACE;
    if (!fragment.isEmpty()) {
        QVariant ret = page()->mainFrame()->evaluateJavaScript(
                QString("window.location='") + fragment + "'");
        qDebug() << ret;
    }
}

void BookView::onLoadFinished(bool ok)
{
    TRACE;
    if (!ok) {
        qDebug() << "Not OK";
        return;
    }
    loaded = true;
    onSettingsChanged("scheme");
    onSettingsChanged("zoom");
    onSettingsChanged("font");
    scheduleRestoreAfterLoad();
}

void BookView::scheduleRestoreAfterLoad()
{
    TRACE;
    if (restoreTimer->isActive()) {
        // Ignore request if a restore is already in progress
        return;
    }

    disconnect(restoreTimer, SIGNAL(timeout()), this, 0);
    connect(restoreTimer, SIGNAL(timeout()), this, SLOT(restoreAfterLoad()));
    restoreTimer->setSingleShot(true);
    restoreTimer->start(210);
}

void BookView::scheduleRestoreLastBookmark()
{
    TRACE;
    if (restoreTimer->isActive()) {
        // Ignore request if a restore is already in progress
        return;
    }

    disconnect(restoreTimer, SIGNAL(timeout()), this, 0);
    connect(restoreTimer, SIGNAL(timeout()), this,
            SLOT(restoreLastBookmark()));
    restoreTimer->setSingleShot(true);
    restoreTimer->start(210);
}

void BookView::restoreAfterLoad()
{
    TRACE;
    if (restoreFragmentAfterLoad) {
        qDebug() << "Restorint to fragment" << fragmentAfterLoad;
        goToFragment(fragmentAfterLoad);
        restoreFragmentAfterLoad = false;
    } else if (restorePositionAfterLoad) {
        qDebug() << "Restoring to position" << positionAfterLoad;
        goToPosition(positionAfterLoad);
        restorePositionAfterLoad = false;
    }

    emit partLoadEnd(contentIndex);
    showProgress();
}

void BookView::onSettingsChanged(const QString &key)
{
    Settings *s = Settings::instance();
    Platform *p = Platform::instance();

    if (key == "zoom") {
        int value = s->value(key, p->defaultZoom()).toInt();
        qDebug() << "BookView::onSettingsChanged: zoom" << value;
        setZoomFactor(value / 100.);
    }
    else if (key == "font") {
        QString face = s->value(key, p->defaultFont()).toString();
        qDebug() << "BookView::onSettingsChanged: font" << face;
        settings()->setFontFamily(QWebSettings::StandardFont, face);
    }
    else if (key == "scheme") {
        QWebFrame *frame = page()->mainFrame();
        QString scheme = Settings::instance()->value("scheme").toString();
        if ((scheme != "day") && (scheme != "night") && (scheme != "sand") &&
            (scheme != "default")) {
            scheme = "default";
        }
        qDebug() << "BookView::onSettingsChanged: scheme" << scheme;
        QFile script(":/styles/" + scheme + ".js");
        script.open(QFile::ReadOnly);
        QString scriptText = script.readAll();
        script.close();
        (void)frame->evaluateJavaScript(scriptText);
    }
    else if (key == "usevolumekeys") {
        bool grab = s->value(key, false).toBool();
        qDebug() << "BookView::onSettingsChanged: usevolumekeys" << grab;
        grabVolumeKeys(grab);
    }
}

void BookView::paintEvent(QPaintEvent *e)
{
    QWebView::paintEvent(e);
    if (!mBook || !loaded) {
        return;
    }

    // Paint bookmarks
    QWebFrame *frame = page()->mainFrame();
    int contentsHeight = frame->contentsSize().height();
    QPoint scrollPos = frame->scrollPosition();
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

    // Paint page separator(s)
    QPen pen(Qt::gray);
    pen.setStyle(Qt::DotLine);
    pen.setWidth(3);
    painter.setPen(pen);
    if (contentIndex > 0) {
        painter.drawLine(0, -scrollPos.y(), width(), -scrollPos.y());
    }
    if (contentIndex < (mBook->parts.size() - 1)) {
        int h = contentsHeight - scrollPos.y() - 1;
        painter.drawLine(0, h, width(), h);
    }
}

void BookView::mousePressEvent(QMouseEvent *e)
{
    QWebView::mousePressEvent(e);
#if defined(Q_WS_MAEMO_5)
    // Start monitoring kinetic scroll
    if (scrollerMonitor) {
        killTimer(scrollerMonitor);
        scrollerMonitor = 0;
    }
    if (scroller) {
        scrollerMonitor = startTimer(500);
    }
#elif defined(Q_OS_SYMBIAN)
    // Do nothing
#else
    // Handle mouse press on the scroll bar
    QWebFrame *frame = page()->mainFrame();
    if (frame->scrollBarGeometry(Qt::Vertical).contains(e->pos())) {
        e->accept();
        return;
    }
#endif
    e->ignore();
}

void BookView::wheelEvent(QWheelEvent *e)
{
    QWebView::wheelEvent(e);
    showProgress();
}

void BookView::addBookmark(const QString &note)
{
    TRACE;
    if (!mBook) {
        return;
    }
    int y = page()->mainFrame()->scrollPosition().y();
    int height = page()->mainFrame()->contentsSize().height();
    qDebug() << ((qreal)y / (qreal)height);
    mBook->addBookmark(contentIndex, (qreal)y / (qreal)height, note);
    update();
}

QString BookView::tmpPath()
{
    return QDir::tempPath() + "/dorian";
}

bool BookView::eventFilter(QObject *o, QEvent *e)
{
#if 0
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
#endif

    // Work around Qt bug that sometimes selects web view contents during swipe
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
#ifndef Q_OS_SYMBIAN // Too heavy on Symbian
        showProgress();
#endif
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
    TRACE;
    page()->mainFrame()->addToJavaScriptWindowObject("bv", this);
}

void BookView::goToPosition(qreal position)
{
    TRACE;
    int contentsHeight = page()->mainFrame()->contentsSize().height();
    int scrollPos = (int)((qreal)contentsHeight * position);
    page()->mainFrame()->setScrollPosition(QPoint(0, scrollPos));
    // FIXME: update();
    qDebug() << "BookView::goToPosition: To" << scrollPos << "("
            << (position * 100) << "%, height" << contentsHeight << ")";
}

void BookView::showProgress()
{
    TRACE;
    if (mBook) {
        int contentsHeight = page()->mainFrame()->contentsSize().height();
        qreal pos = (qreal)(page()->mainFrame()->scrollPosition().y()) /
                    (qreal)contentsHeight;
        emit progress(mBook->getProgress(contentIndex, pos));
    }
}

void BookView::timerEvent(QTimerEvent *e)
{
#if defined(Q_WS_MAEMO_5)
    if (e->timerId() == scrollerMonitor) {
        if (scroller &&
            ((scroller->state() == QAbstractKineticScroller::AutoScrolling) ||
             (scroller->state() == QAbstractKineticScroller::Pushing))) {
            showProgress();
        } else {
            killTimer(scrollerMonitor);
            scrollerMonitor = -1;
        }
    }
#endif // Q_WS_MAEMO_5

    QWebView::timerEvent(e);
}

void BookView::hideEvent(QHideEvent *e)
{
    Trace t("BookView::hideEvent");

#if defined(Q_OS_SYMBIAN)
    setLastBookmark();
#endif

    QWebView::hideEvent(e);
}

void BookView::goPreviousPage()
{
    QWebFrame *frame = page()->mainFrame();
    const int pos = frame->scrollPosition().y();
    frame->scroll(0, -1);
    if (pos == frame->scrollPosition().y()) {
        if (contentIndex > 0) {
            Book::Bookmark bookmark(contentIndex - 1, 1.0);
            mBook->setLastBookmark(contentIndex - 1, 1.0);
            goToBookmark(bookmark);
        }
    } else {
        showProgress();
        QPropertyAnimation *slide =
                new QPropertyAnimation(frame, "scrollPosition");
        const QPoint *offset = new QPoint(0, height() - 18);
        slide->setDuration(400);
        slide->setStartValue(frame->scrollPosition());
        slide->setEndValue(frame->scrollPosition() - *offset);
        slide->setEasingCurve(QEasingCurve::OutQuad);
        slide->start(QAbstractAnimation::DeleteWhenStopped);
        delete offset;
    }
}

void BookView::goNextPage()
{
    TRACE;
    QWebFrame *frame = page()->mainFrame();
    const int pos = frame->scrollPosition().y();
    frame->scroll(0, 1);
    if (pos == frame->scrollPosition().y()) {
        goNext();
    } else {
        showProgress();
        QPropertyAnimation *slide =
                new QPropertyAnimation(frame, "scrollPosition");
        const QPoint *offset = new QPoint(0, (height() - 18));
        slide->setDuration(400);
        slide->setStartValue(frame->scrollPosition());
        slide->setEndValue(frame->scrollPosition() + *offset);
        slide->setEasingCurve(QEasingCurve::OutQuad);
        slide->start(QAbstractAnimation::DeleteWhenStopped);
        delete offset;
    }
}

void BookView::grabVolumeKeys(bool grab)
{
    TRACE;
    grabbingVolumeKeys = grab;
}

#ifdef Q_OS_SYMBIAN

void BookView::onMediaKeysPressed(MediaKeysObserver::MediaKeys key)
{
    TRACE;
    qDebug() << "Key" << (int)key;
    if (grabbingVolumeKeys) {
        if (key == MediaKeysObserver::EVolIncKey) {
            qDebug() << "Volume up";
            goPreviousPage();
        } else if (key == MediaKeysObserver::EVolDecKey){
            qDebug() << "Volume down";
            goNextPage();
        }
    }
}

#endif // Q_OS_SYMBIAN
