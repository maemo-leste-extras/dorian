#include <QFileInfo>

#include "bookdb.h"
#include "platform.h"
#include "trace.h"

BookDb *theInstance;

BookDb *BookDb::instance()
{
    if (!theInstance) {
        theInstance = new BookDb;
    }
    return theInstance;
}

void BookDb::close()
{
    if (theInstance) {
        delete theInstance;
        theInstance = 0;
    }
}

BookDb::BookDb()
{
    Trace t("BookDb::BookDb");
    bool shouldCreate = false;
    QFileInfo info(Platform::dbPath());
    if (!info.exists()) {
        QDir dbDir;
        dbDir.mkpath(info.absolutePath());
        shouldCreate = true;
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(Platform::dbPath());
    if (!db.open()) {
        qCritical() << "Could not open" << Platform::dbPath();
    }
    if (shouldCreate) {
        create();
    }
}

BookDb::~BookDb()
{
    db.close();
}

void BookDb::create()
{
    Trace t("BookDb::create");
    QSqlQuery query;
    if (!query.exec("create table book (name text primary key, content blob)")) {
        qCritical() << "Failed to create database";
    }
}

QVariantHash BookDb::load(const QString &book)
{
    Trace t("BookDb::load");
    qDebug() << book;
    QVariantHash ret;
    QSqlQuery query("select content from book where name = ?");
    query.bindValue(0, book);
    while (query.next()) {
        ret = query.value(0).toHash();
        break;
    }
    qDebug() << ret;
    return ret;
}

void BookDb::save(const QString &book, const QVariantHash &data)
{
    Trace t("BookDb::save");
    qDebug() << book;
    QSqlQuery query("insert or replace into book values (?, ?)");
    query.bindValue(0, book);
    query.bindValue(1, data);
    if (!query.exec()) {
        qCritical() << "Failed to insert or replace";
    }
}

void BookDb::remove(const QString &book)
{
    Q_UNUSED(book);
}

QStringList BookDb::books()
{
    Trace t("BookDb::books");
    QStringList ret;
    QSqlQuery query("select name from book");
    query.setForwardOnly(true);
    while (query.next()) {
        ret.append(query.value(0).toString());
    }
    qDebug() << ret;
    return ret;
}
