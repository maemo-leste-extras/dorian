#include <QStringList>
#include <QDir>
#include <QFileInfoList>

#include "bookfinder.h"
#include "trace.h"

BookFinder::BookFinder(QObject *parent): QObject(parent)
{
}

void BookFinder::find(const QStringList &directories, const QStringList &books)
{
    Trace t("BookFinder::find");
    QStringList booksFound;

    foreach (QString path, directories) {
        QStringList filters(QString("*.epub"));
        QFileInfoList entries =
                QDir(path).entryInfoList(filters, QDir::Files | QDir::Readable);
        foreach (QFileInfo entry, entries) {
            booksFound.append(entry.absoluteFilePath());
        }
    }

    foreach (QString found, booksFound) {
        if (!books.contains(found)) {
            t.trace(QString("New book ") + found);
            emit add(found);
        }
    }

    foreach (QString book, books) {
        QFileInfo bookInfo = QFileInfo(book);
        QString bookDir = bookInfo.absolutePath();
        QString bookPath = bookInfo.absoluteFilePath();
        foreach (QString dirName, directories) {
            t.trace(bookDir + " vs. " + QDir(dirName).absolutePath());
            if (bookDir == QDir(dirName).absolutePath()) {
                if (!booksFound.contains(bookPath)) {
                    t.trace(QString("Deleted book ") + bookPath);
                    emit remove(bookPath);
                }
                break;
            }
        }
    }
}
