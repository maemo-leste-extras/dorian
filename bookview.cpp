#include <QDebug>
#include <QWebFrame>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#include <QTimer>

#include "book.h"
#include "bookview.h"
#include "library.h"
#include "selectionsuppressor.h"
#include "settings.h"
#include "trace.h"

#ifdef Q_WS_MAC
#   define ICON_PREFIX ":/icons/mac/"
#else
#   define ICON_PREFIX ":/icons/"
#endif

BookView::BookView(QWidget *parent):
    QWebView(parent), contentIndex(-1), mBook(0), restore(true),
    positionAfterLoad(0), loaded(false)
{
    Trace t("BookView::BookView");
    settings()->setAttribute(QWebSettings::AutoLoadImages, true);
    settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,
                             false);
    page()->setContentEditable(false);

#if defined(Q_WS_MAEMO_5)
    (void)new SelectionSuppressor(this);
#endif
    QWebFrame *frame = page()->mainFrame();
#if defined(Q_WS_MAEMO_5)
    frame->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
#endif
    frame->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);

    bookmarkImage = QImage(":/icons/bookmark.png");

    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
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
        emit chapterLoadStart(index);
        load(QUrl(contentFile));
    }
    contentIndex = index;
}

void BookView::setBook(Book *book)
{
    Trace t("BookView::setBook");
    setLastBookmark();
    if (book != mBook) {
        mBook = book;
        if (book) {
            contentIndex = -1;
            book->open();
            goToBookmark(book->lastBookmark());
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
    Trace t("BookView::saveLastBookmark");
    if (mBook) {
        int height = page()->mainFrame()->contentsSize().height();
        int pos = page()->mainFrame()->scrollPosition().y();
        mBook->setLastBookmark(contentIndex, (qreal)pos / (qreal)height);
    }
}

void BookView::goToBookmark(const Book::Bookmark &bookmark)
{
    Trace t("BookView::goToBookmark");
    if (mBook) {
        restore = true;
        positionAfterLoad = bookmark.pos;
        if (bookmark.chapter != contentIndex) {
            loadContent(bookmark.chapter);
        } else {
            onLoadFinished(true);
        }
    }
}

void BookView::onLoadFinished(bool ok)
{
    Trace t(QString("BookView::onLoadFinished: %1").arg(ok));
    loaded = true;
    if (ok) {
        addNavigationBar();
    }
    onSettingsChanged("scheme");
    emit chapterLoadEnd(contentIndex);
    if (restore) {
        restore = false;
        if (ok && mBook) {
            int height = page()->mainFrame()->contentsSize().height();
            int scrollPos = (qreal)height * positionAfterLoad;
            page()->mainFrame()->setScrollPosition(QPoint(0, scrollPos));
        }
    }
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
        int height = page()->mainFrame()->contentsSize().height();
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
    int y = page()->mainFrame()->scrollPosition().y();
    int height = page()->mainFrame()->contentsSize().height();
    t.trace(QString().setNum((qreal)y / (qreal)height));
    mBook->addBookmark(contentIndex, (qreal)y / (qreal)height);
    repaint();
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
    frame->addToJavaScriptWindowObject("bv", this);
    QString headerScript = "document.body.innerHTML = '" +
        naviPrev + naviNext + "<br />" + "' + document.body.innerHTML;";
    QString trailerScript = "document.body.innerHTML += '<br /><br />" +
        naviPrev + naviNext + "';";

    frame->evaluateJavaScript(headerScript);
    frame->evaluateJavaScript(trailerScript);
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
