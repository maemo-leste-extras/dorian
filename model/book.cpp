#include <QDir>
#include <QString>
#include <QDebug>
#include <QtXml>
#include <qtextdocument.h>  // Qt::escape is currently defined here...
#include <QDirIterator>
#include <QFileInfo>
#include <QtAlgorithms>
#include <QCryptographicHash>

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

static QImage makeCover(const QString &path)
{
    return QImage(path).scaled(COVER_WIDTH, COVER_HEIGHT,
        Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation).
        scaled(COVER_WIDTH, COVER_HEIGHT, Qt::KeepAspectRatio);
}

Book::Book(const QString &p, QObject *parent): QObject(parent)
{
    mPath = "";
    if (p.size()) {
        QFileInfo info(p);
        mPath = info.absoluteFilePath();
        title = info.baseName();
        cover = makeCover(":/icons/book.png");
        mTempFile.open();
    }
}

QString Book::path() const
{
    return mPath;
}

bool Book::open()
{
    Trace t("Book::open");
    qDebug() << path();
    close();
    clear();
    if (path().isEmpty()) {
        title = "No book";
        return false;
    }
    if (!extract()) {
        return false;
    }
    if (!parse()) {
        return false;
    }
    save();
    emit opened(path());
    return true;
}

void Book::peek()
{
    Trace t("Book::peek");
    qDebug() << path();
    close();
    clear();
    if (path().isEmpty()) {
        title = "No book";
        return;
    }
    if (!extractMetaData()) {
        return;
    }
    if (!parse()) {
        return;
    }
    save();
    close();
}

void Book::close()
{
    Trace t("Book::close");
    content.clear();
    parts.clear();
    QDir::setCurrent(QDir::rootPath());
    clearDir(tmpDir());
}

QString Book::tmpDir() const
{
    QString tmpName = QFileInfo(mTempFile.fileName()).fileName();
    return QDir::tempPath() + "/dorian/" + tmpName;
}

bool Book::extract()
{
    Trace t("Book::extract");
    bool ret = false;
    QString tmp = tmpDir();
    qDebug() << "Extracting" << mPath << "to" << tmp;

    QDir::setCurrent(QDir::rootPath());
    if (!clearDir(tmp)) {
        qCritical() << "Book::extract: Failed to remove" << tmp;
        return false;
    }
    QDir d;
    if (!d.mkpath(tmp)) {
        qCritical() << "Book::extract: Could not create" << tmp;
        return false;
    }

    // If book comes from resource, copy it to the temporary directory first
    QString bookPath = path();
    if (bookPath.startsWith(":/books/")) {
        QFile src(bookPath);
        QString dst(tmp + "/book.epub");
        if (!src.copy(dst)) {
            qCritical() << "Book::extract: Failed to copy built-in book to"
                    << dst;
            return false;
        }
        bookPath = dst;
    }

    QString oldDir = QDir::currentPath();
    if (!QDir::setCurrent(tmp)) {
        qCritical() << "Book::extract: Could not change to" << tmp;
        return false;
    }
    ret = extractZip(bookPath);
    if (!ret) {
        qCritical() << "Book::extract: Extracting ZIP failed";
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
    qDebug() << "Parsing OPS file" << opsFileName;
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

    // Initially, put all content items in the chapter list.
    // This will be refined by parsing the NCX file later
    chapters = parts;

    // Load cover image
    QStringList coverKeys;
    coverKeys << "cover-image" << "img-cover-jpeg" << "cover";
    foreach (QString key, coverKeys) {
        if (content.contains(key)) {
            qDebug() << "Loading cover image from" << content[key].href;
            cover = makeCover(rootPath() + "/" + content[key].href);
            break;
        }
    }

    // If there is an "ncx" item in content, parse it: That's the real table of
    // contents
    QString ncxFileName;
    if (content.contains("ncx")) {
        ncxFileName = content["ncx"].href;
    } else if (content.contains("ncxtoc")) {
        ncxFileName = content["ncxtoc"].href;
    } else {
        qDebug() << "No NCX table of contents";
    }
    if (!ncxFileName.isEmpty()) {
        qDebug() << "Parsing NCX file" << ncxFileName;
        QFile ncxFile(rootPath() + "/" + ncxFileName);
        source = new QXmlInputSource(&ncxFile);
        NcxHandler *ncxHandler = new NcxHandler(*this);
        errorHandler = new XmlErrorHandler();
        reader.setContentHandler(ncxHandler);
        reader.setErrorHandler(errorHandler);
        ret = reader.parse(source);
        delete errorHandler;
        delete ncxHandler;
        delete source;
    }

    // Calculate book part sizes
    size = 0;
    foreach (QString part, parts) {
        QFileInfo info(content[part].href);
        content[part].size = info.size();
        size += content[part].size;
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
                qCritical() << "Book::clearDir: Could not remove" << entry;
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
    qDebug() << "path" << path();
    QSettings settings;
    QString key = "book/" + path() + "/";
    qDebug() << "key" << key;

    // Load book info
    title = settings.value(key + "title").toString();
    qDebug() << title;
    creators = settings.value(key + "creators").toStringList();
    date = settings.value(key + "date").toString();
    publisher = settings.value(key + "publisher").toString();
    datePublished = settings.value(key + "datepublished").toString();
    subject = settings.value(key + "subject").toString();
    source = settings.value(key + "source").toString();
    rights = settings.value(key + "rights").toString();
    mLastBookmark.part = settings.value(key + "lastpart").toInt();
    mLastBookmark.pos = settings.value(key + "lastpos").toReal();
    cover = settings.value(key + "cover").value<QImage>().scaled(COVER_WIDTH,
        COVER_HEIGHT, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (cover.isNull()) {
        cover = makeCover(":/icons/book.png");
    }

    // Load bookmarks
    int size = settings.value(key + "bookmarks").toInt();
    for (int i = 0; i < size; i++) {
        int part = settings.value(key + "bookmark" + QString::number(i) +
                                     "/part").toInt();
        qreal pos = settings.value(key + "bookmark" + QString::number(i) +
                                   "/pos").toReal();
        qDebug() << QString("Bookmark %1 at part %2, %3").
                arg(i).arg(part).arg(pos);
        mBookmarks.append(Bookmark(part, pos));
    }
}

void Book::save()
{
    Trace t("Book::save");
    QSettings settings;
    QString key = "book/" + path() + "/";
    qDebug() << "key" << key;

    // Save book info
    settings.setValue(key + "title", title);
    qDebug() << "title" << title;
    settings.setValue(key + "creators", creators);
    settings.setValue(key + "date", date);
    settings.setValue(key + "publisher", publisher);
    settings.setValue(key + "datepublished", datePublished);
    settings.setValue(key + "subject", subject);
    settings.setValue(key + "source", source);
    settings.setValue(key + "rights", rights);
    settings.setValue(key + "lastpart", mLastBookmark.part);
    settings.setValue(key + "lastpos", mLastBookmark.pos);
    settings.setValue(key + "cover", cover);

    // Save bookmarks
    settings.setValue(key + "bookmarks", mBookmarks.size());
    for (int i = 0; i < mBookmarks.size(); i++) {
        qDebug() << QString("Bookmark %1 at %2, %3").
                arg(i).arg(mBookmarks[i].part).arg(mBookmarks[i].pos);
        settings.setValue(key + "bookmark" + QString::number(i) + "/part",
                          mBookmarks[i].part);
        settings.setValue(key + "bookmark" + QString::number(i) + "/pos",
                          mBookmarks[i].pos);
    }
}

void Book::setLastBookmark(int part, qreal position)
{
    mLastBookmark.part = part;
    mLastBookmark.pos = position;
    save();
}

Book::Bookmark Book::lastBookmark() const
{
    return Book::Bookmark(mLastBookmark);
}

void Book::addBookmark(int part, qreal position)
{
    mBookmarks.append(Bookmark(part, position));
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
    qDebug() << container.fileName();
    QXmlSimpleReader reader;
    QXmlInputSource *source = new QXmlInputSource(&container);
    ContainerHandler *containerHandler = new ContainerHandler();
    XmlErrorHandler *errorHandler = new XmlErrorHandler();
    reader.setContentHandler(containerHandler);
    reader.setErrorHandler(errorHandler);
    if (reader.parse(source)) {
        ret = tmpDir() + "/" + containerHandler->rootFile;
        mRootPath = QFileInfo(ret).absoluteDir().absolutePath();
        qDebug() << "OSP path" << ret << "\nRoot dir" << mRootPath;
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
    if (title.size()) {
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

QString Book::shortName() const
{
    return (title.isEmpty())? QFileInfo(path()).baseName(): title;
}

int Book::chapterFromPart(int index)
{
    int ret = -1;

    QString partId = parts[index];
    QString partHref = content[partId].href;

    for (int i = 0; i < chapters.size(); i++) {
        QString id = chapters[i];
        QString href = content[id].href;
        QString baseRef(href);
        QUrl url(QString("file://") + href);
        if (url.hasFragment()) {
            QString fragment = url.fragment();
            baseRef.chop(fragment.length() + 1);
        }
        if (baseRef == partHref) {
            ret = i;
            // Don't break, keep looking
        }
    }

    return ret;
}

int Book::partFromChapter(int index)
{
    Trace t("Book::partFromChapter");
    QString id = chapters[index];
    QString href = content[id].href;
    QString baseRef(href);
    QUrl url(QString("file://") + href);
    if (url.hasFragment()) {
        QString fragment = url.fragment();
        baseRef.chop(fragment.length() + 1);
    }
    qDebug() << "Chapter" << index;
    qDebug() << " id" << id;
    qDebug() << " href" << href;
    qDebug() << " base href" << baseRef;

    for (int i = 0; i < parts.size(); i++) {
        QString partId = parts[i];
        if (content[partId].href == baseRef) {
            qDebug() << "Part index for" << baseRef << "is" << i;
            return i;
        }
    }

    qWarning() << "Book::partFromChapter: Could not find part index for"
            << baseRef;
    return -1;
}

qreal Book::getProgress(int part, qreal position)
{
    Q_ASSERT(part < parts.size());
    QString key;
    qreal partSize = 0;
    for (int i = 0; i < part; i++) {
        key = parts[i];
        partSize += content[key].size;
    }
    key = parts[part];
    partSize += content[key].size * position;
    return partSize / (qreal)size;
}

bool Book::extractMetaData()
{
    // FIXME
    return extract();
}
