#include <QDebug>
#include <QWebFrame>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#include <QTimer>

#include "book.h"
#include "bookview.h"
#include "library.h"
#include "settings.h"
#include "trace.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

BookView::BookView(QWidget *parent):
    QWebView(parent), contentIndex(-1), mBook(0),
    restorePositionAfterLoad(false), positionAfterLoad(0), loaded(false),
    contentsHeight(0), decorated(false)
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

    extractIcons();
}

BookView::~BookView()
{
    Trace t("BookView::~BookView");
    removeIcons();
}

void BookView::loadContent(int index)
{
    Trace t("BookView::loadContent");
    if (!mBook) {
        return;
    }
    if ((index < 0) || (index >= mBook->toc.size())) {
        return;
    }

    QString contentFile(mBook->content[mBook->toc[index]].href);
    if (mBook->toc[index] == "error") {
        setHtml(contentFile);
    }
    else {
        loaded = false;
        decorated = false;
        emit chapterLoadStart(index);
        load(QUrl(contentFile));
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
    loadContent(contentIndex - 1);
}

void BookView::goNext()
{
    Trace t("BookView::goNext");
    loadContent(contentIndex + 1);
}

void BookView::setLastBookmark()
{
    Trace t("BookView::setLastBookmark");
    if (mBook) {
        int height = contentsHeight;
        int pos = page()->mainFrame()->scrollPosition().y();
        t.trace(QString("At %1 (%2%, height %3)").
                arg(pos).arg((qreal)pos / (qreal)height * 100).arg(height));
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
        if (bookmark.chapter != contentIndex) {
            t.trace(QString("Loading new chapter %1").arg(bookmark.chapter));
            mBook->setLastBookmark(bookmark.chapter, bookmark.pos);
            restorePositionAfterLoad = true;
            positionAfterLoad = bookmark.pos;
            loadContent(bookmark.chapter);
        } else {
            goToPosition(bookmark.pos);
        }
    }
}

void BookView::onLoadFinished(bool ok)
{
    Trace t("BookView::onLoadFinished");
    if (!ok) {
        t.trace("Not OK");
        return;
    }
    loaded = true;
    addNavigationBar();
    onSettingsChanged("scheme");
    emit chapterLoadEnd(contentIndex);
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
        if (b.chapter != contentIndex) {
            continue;
        }
        int height = contentsHeight;
        int bookmarkPos = (qreal)height * (qreal)b.pos;
        painter.drawPixmap(2, bookmarkPos - scrollPos.y(), bookmarkPixmap);
    }
}

void BookView::mousePressEvent(QMouseEvent *e)
{
    QWebView::mousePressEvent(e);
#ifndef Q_WS_MAEMO_5
    QWebFrame *frame = page()->mainFrame();
    if (frame->scrollBarGeometry(Qt::Vertical).contains(e->pos())) {
        e->accept();
        return;
    }
#endif // Q_WS_MAEMO_5
    e->ignore();
}

void BookView::addBookmark()
{
    Trace t("BookView::addBookmark");
    if (!mBook) {
        return;
    }
    int y = page()->mainFrame()->scrollPosition().y();
    int height = page()->mainFrame()->contentsSize().height();
    t.trace(QString().setNum((qreal)y / (qreal)height));
    mBook->addBookmark(contentIndex, (qreal)y / (qreal)height);
    update();
}

void BookView::addNavigationBar()
{
    Trace t("BookView::addNavigationBar");
    if (!mBook) {
        return;
    }

    QString naviPrev =
            "<a href=\"javascript:bv.goPrevious();\">"
            "<img width=\"95\" height=\"95\" style=\"float:left;clear:none;\" "
            "src=\"file://"
            + tmpPath() +
            "/previous.png\" />"
            "</a>";
    QString naviNext =
            "<a href=\"javascript:bv.goNext();\">"
            "<img width=\"95\" height=\"95\" style=\"float:right;clear:none;\" "
            "src=\"file://"
            + tmpPath() +
            "/next.png\" />"
            "</a>";
    if (contentIndex == 0) {
        naviPrev = "";
    }
    if (contentIndex >= mBook->toc.size() - 1) {
        naviNext = "";
    }

    QWebFrame *frame = page()->currentFrame();
    QString headerScript = "document.body.innerHTML = '" +
        naviPrev + naviNext + "<br />" + "' + document.body.innerHTML;";
    QString trailerScript = "document.body.innerHTML += '<br /><br />" +
        naviPrev + naviNext + "';";

    frame->evaluateJavaScript(headerScript);
    frame->evaluateJavaScript(trailerScript);
    decorated = true;
}

QString BookView::tmpPath()
{
    return QDir::tempPath() + "/dorian";
}

void BookView::extractIcons()
{
    QFile next(ICON_PREFIX + QString("/next.png"));
    QFile prev(ICON_PREFIX + QString("/previous.png"));

    QDir().mkpath(tmpPath());
    next.copy(tmpPath() + "/next.png");
    prev.copy(tmpPath() + "/previous.png");
}

void BookView::removeIcons()
{
    QFile(ICON_PREFIX + QString("/next.png")).remove();
    QFile(ICON_PREFIX + QString("/previous.png")).remove();
    QDir().rmpath(tmpPath());
}

bool BookView::eventFilter(QObject *o, QEvent *e)
{
#if 0
    if (e->type() != QEvent::Paint && e->type() != QEvent::MouseMove) {
        if (e->type() == QEvent::Resize) {
            Trace::trace(QString("BookView::eventFilter QEvent::Resize to %1").
                         arg(page()->mainFrame()->contentsSize().height()));
        } else {
            Trace::trace(QString("BookView::eventFilter %1").
                         arg(Trace::event(e->type())));
        }
    }
#endif

    switch (e->type()) {
    case QEvent::MouseButtonPress:
        emit suppressedMouseButtonPress();
        mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
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
    if (decorated) {
        if (restorePositionAfterLoad) {
            Trace::trace("BookView::onContentSizeChanged: Time to restore");
            restorePositionAfterLoad = false;
            goToPosition(positionAfterLoad);
        }
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
    int scrollPos = (qreal)contentsHeight * position;
    page()->mainFrame()->setScrollPosition(QPoint(0, scrollPos));
    // FIXME: update();
    Trace::trace(QString("BookView::goToPosition: To %1 (%2%, height %3)").
            arg(scrollPos).arg(position * 100).arg(contentsHeight));
}
