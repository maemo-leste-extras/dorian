#include <QSettings>
#include <QDebug>
#include <QFileInfo>

#include "library.h"
#include "book.h"

Library *Library::mInstance = 0;

Library::Library(QObject *parent): QAbstractListModel(parent), mCurrent(0)
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
    qDebug() << "Library::data, row" << index.row() << "role" << role;

    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return mBooks[index.row()]->name();
    case BookRole:
        return QVariant::fromValue<Book>(*mBooks[index.row()]);
    default:
        return QVariant();
    }
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
        book->load();
        qDebug() << "Library::load: Add" << book->title << "from" << path;
        mBooks.append(book);
    }
    QString currentPath = settings.value("lib/current").toString();
    QModelIndex index = find(currentPath);
    if (index.isValid()) {
        mCurrent = mBooks[index.row()];
        qDebug() << "Library::load: Current book is" << mCurrent->path();
    }
}

void Library::save()
{
    qDebug() << "Library::save";

    QSettings settings;
    settings.setValue("lib/size", mBooks.size());
    for (int i = 0; i < mBooks.size(); i++) {
        QString key = "lib/book" + QString::number(i);
        settings.setValue(key, mBooks[i]->path());
    }
    settings.setValue("lib/current", mCurrent? mCurrent->path(): QString());
}

bool Library::add(QString path)
{
    qDebug() << "Library::add" << path;
    if (path == "") {
        qWarning() << "Library::add: Empty path";
        return false;
    }
    if (find(path).isValid()) {
        qDebug() << " Book already exists in library";
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
    if (!index.isValid()) {
        return;
    }
    int row = index.row();
    if ((row < 0) || (row >= mBooks.size())) {
        return;
    }
    beginRemoveRows(QModelIndex(), row, row);
    Book *book = mBooks[row];
    mBooks.removeAt(row);
    save();
    endRemoveRows();
    if (book == mCurrent) {
        mCurrent = 0;
        emit currentBookChanged();
    }
    delete book;
}

Book *Library::current() const
{
    return mCurrent;
}

void Library::setCurrent(const QModelIndex index)
{
    int row = index.row();
    qDebug() << "Library::setCurrent" << row;
    if ((row >= 0) && (row < mBooks.size())) {
        mCurrent = mBooks[row];
        save();
        emit currentBookChanged();
    }
}

void Library::clear()
{
    for (int i = 0; i < mBooks.size(); i++) {
        delete mBooks[i];
    }
    mBooks.clear();
    mCurrent = 0;
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
    for (int i = 0; i < mBooks.size(); i++) {
        if (book == mBooks[i]) {
            return index(i);
        }
    }
    return QModelIndex();
}
