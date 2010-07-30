#include <QSettings>
#include <QDebug>
#include <QFileInfo>

#include "library.h"
#include "book.h"
#include "trace.h"

Library *Library::mInstance = 0;

Library::Library(QObject *parent): QAbstractListModel(parent)
{
    load();
}

Library::~Library()
{
    clear();
}

Library *Library::instance()
{
    if (!mInstance) {
        mInstance = new Library();
    }
    return mInstance;
}

int Library::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return mBooks.size();
    }
}

QVariant Library::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return mBooks[index.row()]->name();
    case Qt::DecorationRole:
        return QPixmap::fromImage(mBooks[index.row()]->cover);
    default:
        return QVariant();
    }
}

Book *Library::book(const QModelIndex &index)
{
    if (index.isValid()) {
        if ((index.row() >= 0) && (index.row() < mBooks.size())) {
            return mBooks[index.row()];
        } else {
            qCritical() << "*** Library::book: Bad index" << index.row();
        }
    }
    return 0;
}

void Library::close()
{
    delete mInstance;
    mInstance = 0;
}

void Library::load()
{
    QSettings settings;
    clear();
    int size = settings.value("lib/size").toInt();
    for (int i = 0; i < size; i++) {
        QString key = "lib/book" + QString::number(i);
        QString path = settings.value(key).toString();
        Book *book = new Book(path);
        connect(book, SIGNAL(opened(const QString &)),
                this, SLOT(onBookOpened(const QString &)));
        book->load();
        mBooks.append(book);
    }
    QString currentPath = settings.value("lib/nowreading").toString();
    mNowReading = find(currentPath);
}

void Library::save()
{
    QSettings settings;
    settings.setValue("lib/size", mBooks.size());
    for (int i = 0; i < mBooks.size(); i++) {
        QString key = "lib/book" + QString::number(i);
        settings.setValue(key, mBooks[i]->path());
    }
    Book *currentBook = book(mNowReading);
    settings.setValue("lib/nowreading",
                      currentBook? currentBook->path(): QString());
}

bool Library::add(QString path)
{
    Trace t("Library::add " + path);
    if (path == "") {
        qCritical() << "*** Library::add: Empty path";
        return false;
    }
    if (find(path).isValid()) {
        t.trace("Book already exists in library");
        return false;
    }
    int size = mBooks.size();
    Book *book = new Book(path);
    beginInsertRows(QModelIndex(), size, size);
    mBooks.append(book);
    save();
    endInsertRows();
    return true;
}

void Library::remove(const QModelIndex &index)
{
    Book *toRemove = book(index);
    if (!toRemove) {
        return;
    }
    int row = index.row();
    beginRemoveRows(QModelIndex(), row, row);
    mBooks.removeAt(row);
    save();
    endRemoveRows();
    if (index == mNowReading) {
        mNowReading = QModelIndex();
        emit nowReadingChanged();
    }
    delete toRemove;
}

QModelIndex Library::nowReading() const
{
    return mNowReading;
}

void Library::setNowReading(const QModelIndex &index)
{
    mNowReading = index;
    save();
    emit nowReadingChanged();
}

void Library::clear()
{
    for (int i = 0; i < mBooks.size(); i++) {
        delete mBooks[i];
    }
    mBooks.clear();
    mNowReading = QModelIndex();
}

QModelIndex Library::find(QString path) const
{
    if (path != "") {
        QString absolutePath = QFileInfo(path).absoluteFilePath();
        for (int i = 0; i < mBooks.size(); i++) {
            if (absolutePath == mBooks[i]->path()) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

QModelIndex Library::find(const Book *book) const
{
    if (book) {
        for (int i = 0; i < mBooks.size(); i++) {
            if (book == mBooks[i]) {
                return index(i);
            }
        }
    }
    return QModelIndex();
}

void Library::onBookOpened(const QString &path)
{
    Trace t("Library::onBookOpened " + path);
    QModelIndex index = find(path);
    if (index.isValid()) {
        emit dataChanged(index, index);
    }
}
