#include <QDir>
#include <QString>
#include <QDebug>
#include <QtXml>
#include <qtextdocument.h>  // Qt::escape is currently defined here...
#include <QDirIterator>
#include <QFileInfo>

#include "book.h"
#include "opshandler.h"
#include "opserrorhandler.h"
#include "extractzip.h"
#include "library.h"
#include "containerhandler.h"

Book::Book(const QString &path_)
{
    mPath = "";
    if (path_ != "") {
        QFileInfo info(path_);
        mPath = info.absoluteFilePath();
        title = info.baseName();
        cover = QIcon(":/icons/book.png");
    }
}

QString Book::path() const
{
    return mPath;
}

void Book::open()
{
    qDebug() << "Book::open" << path();
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
    }
}

void Book::close()
{
    qDebug() << "Book::close";
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
    content["error"].type = "text/html";
}

bool Book::extract()
{
    bool ret = false;
    QString tmp = tmpDir();
    qDebug() << "Book::extract: Extracting" << mPath << "to" << tmp;

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
    qDebug() << "Book::parse";

    bool ret = false;
    QFile bookFile(opsPath());
    QXmlSimpleReader reader;
    QXmlInputSource *source = new QXmlInputSource(&bookFile);
    OpsHandler *opsHandler = new OpsHandler(*this);
    OpsErrorHandler *opsErrorHandler = new OpsErrorHandler();
    reader.setContentHandler(opsHandler);
    reader.setErrorHandler(opsErrorHandler);

    ret = reader.parse(source);
    if (!ret) {
        qCritical() << "*** Book::parse: XML parsing failed";
    }

    delete opsHandler;
    delete source;
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
    qDebug() << "Book::load" << path();
    QSettings settings;
    QString key = "book/" + path() + "/";
    qDebug() << " key" << key;

    // Load book info
    title = settings.value(key + "title").toString();
    qDebug() << " title" << title;
    creators = settings.value(key + "creators").toStringList();
    date = settings.value(key + "date").toString();
    publisher = settings.value(key + "publisher").toString();
    datePublished = settings.value(key + "datepublished").toString();
    subject = settings.value(key + "subject").toString();
    source = settings.value(key + "source").toString();
    rights = settings.value(key + "rights").toString();
    mLastBookmark.chapter = settings.value(key + "lastchapter").toInt();
    mLastBookmark.pos = settings.value(key + "lastpos").toReal();

    // Load bookmarks
    int size = settings.value(key + "bookmarks").toInt();
    for (int i = 0; i < size; i++) {
        int chapter = settings.value(key + "bookmark" + QString::number(i) +
                                     "/chapter").toInt();
        qreal pos = settings.value(key + "bookmark" + QString::number(i) +
                                   "/pos").toReal();
        qDebug() << " Bookmark" << i << "at" << chapter << "," << pos;
        mBookmarks.append(Bookmark(chapter, pos));
    }
}

void Book::save()
{
    qDebug() << "Book::save";
    QSettings settings;
    QString key = "book/" + path() + "/";
    qDebug() << " key" << key;

    // Save book info
    settings.setValue(key + "title", title);
    qDebug() << " title" << title;
    settings.setValue(key + "creators", creators);
    settings.setValue(key + "date", date);
    settings.setValue(key + "publisher", publisher);
    settings.setValue(key + "datepublished", datePublished);
    settings.setValue(key + "subject", subject);
    settings.setValue(key + "source", source);
    settings.setValue(key + "rights", rights);
    settings.setValue(key + "lastchapter", mLastBookmark.chapter);
    settings.setValue(key + "lastpos", mLastBookmark.pos);

    // Save bookmarks
    settings.setValue(key + "bookmarks", mBookmarks.size());
    for (int i = 0; i < mBookmarks.size(); i++) {
        qDebug() << " Bookmark" << i << "at" << mBookmarks[i].chapter << ","
                << mBookmarks[i].pos;
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
    save();
}

QList<Book::Bookmark> Book::bookmarks() const
{
    return mBookmarks;
}

QString Book::opsPath()
{
    QString ret;

    QFile container(tmpDir() + "/META-INF/container.xml");
    qDebug() << "Book::opsPath" << container.fileName();
    QXmlSimpleReader reader;
    QXmlInputSource *source = new QXmlInputSource(&container);
    ContainerHandler *containerHandler = new ContainerHandler();
    OpsErrorHandler *opsErrorHandler = new OpsErrorHandler();
    reader.setContentHandler(containerHandler);
    reader.setErrorHandler(opsErrorHandler);
    if (reader.parse(source)) {
        ret = tmpDir() + "/" + containerHandler->rootFile;
        mRootPath = QFileInfo(ret).absoluteDir().absolutePath();
        qDebug() << " OSP path" << ret;
        qDebug() << " Root dir" << mRootPath;
    }
    delete containerHandler;
    delete source;
    return ret;
}

QString Book::rootPath() const
{
    return mRootPath;
}
