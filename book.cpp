#include <QDir>
#include <QString>
#include <QDebug>
#include <QtXml>
#include <qtextdocument.h>  // Qt::escape is currently defined here...
#include <QDirIterator>
#include <QFileInfo>
#include <QtAlgorithms>

#include "book.h"
#include "opshandler.h"
#include "xmlerrorhandler.h"
#include "extractzip.h"
#include "library.h"
#include "containerhandler.h"
#include "ncxhandler.h"
#include "trace.h"

const int COVER_WIDTH = 53;
const int COVER_HEIGHT = 59;

Book::Book(const QString &p, QObject *parent): QObject(parent)
{
    mPath = "";
    if (p != "") {
        QFileInfo info(p);
        mPath = info.absoluteFilePath();
        title = info.baseName();
        cover = QImage(":/icons/book.png").scaled(COVER_WIDTH, COVER_HEIGHT,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}

QString Book::path() const
{
    return mPath;
}

void Book::open()
{
    Trace t("Book::open");
    t.trace(path());
    close();
    clear();
    if (path() == "") {
        title = "No book";
        fail("", "No book");
    }
    else if (!extract()) {
        fail("Could not extract content of " + path() + ".");
    }
    else if (!parse()) {
        fail("Could not parse content of " + path() + ".");
    }
    else {
        save();
        emit opened(path());
    }
}

void Book::close()
{
    Trace t("Book::close");
    content.clear();
    toc.clear();
    QDir::setCurrent(QDir::rootPath());
    clearDir(tmpDir());
}

QString Book::tmpDir() const
{
    return QDir::tempPath() + "/dorian/book";
}

void Book::fail(const QString &details, const QString &error)
{
    close();

    toc.append("error");
    QString errorPage = "<html><head><title>" + Qt::escape(error) +
        "</title></head><body><h1>" + Qt::escape(error) + "</h1><p>" +
        Qt::escape(details) + "</p></body></html>";
    content["error"].href = errorPage;
    content["error"].name = "Error";
}

bool Book::extract()
{
    Trace t("Book::extract");
    bool ret = false;
    QString tmp = tmpDir();
    t.trace("Extracting " + mPath + " to " + tmp);

    QDir::setCurrent(QDir::rootPath());
    if (!clearDir(tmp)) {
        qCritical() << "*** Book::extract: Failed to remove" << tmp;
        return false;
    }
    QDir d;
    if (!d.mkpath(tmp)) {
        qCritical() << "*** Book::extract: Could not create" << tmp;
        return false;
    }

    // If book comes from resource, copy it to the temporary directory first
    QString bookPath = path();
    if (bookPath.startsWith(":/books/")) {
        QFile src(bookPath);
        QString dst(tmp + "/book.epub");
        if (!src.copy(dst)) {
            qCritical() << "*** Book::extract: Failed to copy built-in book to"
                    << dst;
            return false;
        }
        bookPath = dst;
    }

    QString oldDir = QDir::currentPath();
    if (!QDir::setCurrent(tmp)) {
        qCritical() << "*** Book::extract: Could not change to" << tmp;
        return false;
    }
    ret = extractZip(bookPath);
    if (!ret) {
        qCritical() << "*** Book::extract: Extracting ZIP failed";
    }
    QDir::setCurrent(oldDir);
    return ret;
}

bool Book::parse()
{
    Trace t("Book::parse");

    // Parse OPS file
    bool ret = false;
    QString opsFileName = opsPath();
    t.trace("Parsing OPS file" + opsFileName);
    QFile opsFile(opsFileName);
    QXmlSimpleReader reader;
    QXmlInputSource *source = new QXmlInputSource(&opsFile);
    OpsHandler *opsHandler = new OpsHandler(*this);
    XmlErrorHandler *errorHandler = new XmlErrorHandler();
    reader.setContentHandler(opsHandler);
    reader.setErrorHandler(errorHandler);
    ret = reader.parse(source);
    delete errorHandler;
    delete opsHandler;
    delete source;

    // Load cover image
    if (content.contains("cover-image")) {
        t.trace("Loading cover image from " + content["cover-image"].href);
        cover = QImage(content["cover-image"].href).scaled(COVER_WIDTH,
            COVER_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    } else if (content.contains("img-cover-jpeg")) {
        t.trace("Loading cover image from " + content["img-cover-jpeg"].href);
        cover = QImage(content["img-cover-jpeg"].href).scaled(COVER_WIDTH,
            COVER_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    // If there is an "ncx" item in content, parse it: That's the real table of
    // contents
    if (content.contains("ncx")) {
        QString ncxFileName = content["ncx"].href;
        t.trace("Parsing NCX file " + ncxFileName);
        QFile ncxFile(ncxFileName);
        source = new QXmlInputSource(&ncxFile);
        NcxHandler *ncxHandler = new NcxHandler(*this);
        errorHandler = new XmlErrorHandler();
        reader.setContentHandler(ncxHandler);
        reader.setErrorHandler(errorHandler);
        ret = reader.parse(source);
        delete ncxHandler;
        delete errorHandler;
        delete source;
    }

    return ret;
}

bool Book::clearDir(const QString &dir)
{
    QDir d(dir);
    if (!d.exists()) {
        return true;
    }
    QDirIterator i(dir, QDirIterator::Subdirectories);
    while (i.hasNext()) {
        QString entry = i.next();
        if (entry.endsWith("/.") || entry.endsWith("/..")) {
            continue;
        }
        QFileInfo info(entry);
        if (info.isDir()) {
            if (!clearDir(entry)) {
                return false;
            }
        }
        else {
            if (!QFile::remove(entry)) {
                qCritical() << "*** Book::clearDir: Could not remove" << entry;
                // FIXME: To be investigated: This is happening too often
                // return false;
            }
        }
    }
    (void)d.rmpath(dir);
    return true;
}

void Book::clear()
{
    close();
    title = "";
    creators.clear();
    date = "";
    publisher = "";
    datePublished = "";
    subject = "";
    source = "";
    rights = "";
}

void Book::load()
{
    Trace t("Book::load");
    t.trace("path: " + path());
    QSettings settings;
    QString key = "book/" + path() + "/";
    t.trace("key: " + key);

    // Load book info
    title = settings.value(key + "title").toString();
    t.trace(title);
    creators = settings.value(key + "creators").toStringList();
    date = settings.value(key + "date").toString();
    publisher = settings.value(key + "publisher").toString();
    datePublished = settings.value(key + "datepublished").toString();
    subject = settings.value(key + "subject").toString();
    source = settings.value(key + "source").toString();
    rights = settings.value(key + "rights").toString();
    mLastBookmark.chapter = settings.value(key + "lastchapter").toInt();
    mLastBookmark.pos = settings.value(key + "lastpos").toReal();
    cover = settings.value(key + "cover").value<QImage>().scaled(COVER_WIDTH,
        COVER_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (cover.isNull()) {
        cover = QImage(":/icons/book.png").scaled(COVER_WIDTH, COVER_HEIGHT,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    // Load bookmarks
    int size = settings.value(key + "bookmarks").toInt();
    for (int i = 0; i < size; i++) {
        int chapter = settings.value(key + "bookmark" + QString::number(i) +
                                     "/chapter").toInt();
        qreal pos = settings.value(key + "bookmark" + QString::number(i) +
                                   "/pos").toReal();
        t.trace(QString("Bookmark %1 at chapter %2, %3").
                arg(i).arg(chapter).arg(pos));
        mBookmarks.append(Bookmark(chapter, pos));
    }
}

void Book::save()
{
    Trace t("Book::save");
    QSettings settings;
    QString key = "book/" + path() + "/";
    t.trace("key: " + key);

    // Save book info
    settings.setValue(key + "title", title);
    t.trace("title: " + title);
    settings.setValue(key + "creators", creators);
    settings.setValue(key + "date", date);
    settings.setValue(key + "publisher", publisher);
    settings.setValue(key + "datepublished", datePublished);
    settings.setValue(key + "subject", subject);
    settings.setValue(key + "source", source);
    settings.setValue(key + "rights", rights);
    settings.setValue(key + "lastchapter", mLastBookmark.chapter);
    settings.setValue(key + "lastpos", mLastBookmark.pos);
    settings.setValue(key + "cover", cover);

    // Save bookmarks
    settings.setValue(key + "bookmarks", mBookmarks.size());
    for (int i = 0; i < mBookmarks.size(); i++) {
        t.trace(QString("Bookmark %1 at %2, %3").
                arg(i).arg(mBookmarks[i].chapter).arg(mBookmarks[i].pos));
        settings.setValue(key + "bookmark" + QString::number(i) + "/chapter",
                          mBookmarks[i].chapter);
        settings.setValue(key + "bookmark" + QString::number(i) + "/pos",
                          mBookmarks[i].pos);
    }
}

void Book::setLastBookmark(int chapter, qreal position)
{
    mLastBookmark.chapter = chapter;
    mLastBookmark.pos = position;
    save();
}

Book::Bookmark Book::lastBookmark() const
{
    return Book::Bookmark(mLastBookmark);
}

void Book::addBookmark(int chapter, qreal position)
{
    mBookmarks.append(Bookmark(chapter, position));
    qSort(mBookmarks.begin(), mBookmarks.end());
    save();
}

void Book::deleteBookmark(int index)
{
    mBookmarks.removeAt(index);
    save();
}

QList<Book::Bookmark> Book::bookmarks() const
{
    return mBookmarks;
}

QString Book::opsPath()
{
    Trace t("Book::opsPath");
    QString ret;

    QFile container(tmpDir() + "/META-INF/container.xml");
    t.trace(container.fileName());
    QXmlSimpleReader reader;
    QXmlInputSource *source = new QXmlInputSource(&container);
    ContainerHandler *containerHandler = new ContainerHandler();
    XmlErrorHandler *errorHandler = new XmlErrorHandler();
    reader.setContentHandler(containerHandler);
    reader.setErrorHandler(errorHandler);
    if (reader.parse(source)) {
        ret = tmpDir() + "/" + containerHandler->rootFile;
        mRootPath = QFileInfo(ret).absoluteDir().absolutePath();
        t.trace("OSP path: " + ret);
        t.trace("Root dir: " + mRootPath);
    }
    delete errorHandler;
    delete containerHandler;
    delete source;
    return ret;
}

QString Book::rootPath() const
{
    return mRootPath;
}

QString Book::name() const
{
    if (title != "") {
        QString ret = title;
        if (creators.length()) {
            ret += "\nBy " + creators[0];
            for (int i = 1; i < creators.length(); i++) {
                ret += ", " + creators[i];
            }
        }
        return ret;
    } else {
        return path();
    }
}
