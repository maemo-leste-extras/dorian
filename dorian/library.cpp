#include <QSettings>
#include <QString>
#include <QDebug>
#include <QFileInfo>

#include "library.h"

Library *Library::mInstance = 0;

Library::Library(): mCurrent(0)
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
    int index = find(currentPath);
    if (-1 != index) {
        mCurrent = mBooks[index];
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
    if (path == "") {
        return false;
    }
    if (find(path) != -1) {
        return false;
    }
    Book *book = new Book(path);
    mBooks.append(book);
    save();
    emit bookAdded();
    return true;
}

void Library::remove(int index)
{
    if ((index < 0) || (index >= mBooks.size())) {
        return;
    }
    Book *book = mBooks[index];
    mBooks.removeAt(index);
    if (book == mCurrent) {
        mCurrent = 0;
        emit currentBookChanged();
    }
    emit bookRemoved(index);
    delete book;
    save();
}

int Library::size() const
{
    return mBooks.size();
}

Book *Library::at(int index) const
{
    return mBooks[index];
}

Book *Library::current() const
{
    return mCurrent;
}

void Library::setCurrent(int index)
{
    qDebug() << "Library::setCurrent" << index;
    if ((index >= 0) && (index < mBooks.size()))
    {
        mCurrent = mBooks[index];
        save();
        qDebug() << " Emit currentBookChanged()";
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

int Library::find(QString path) const
{
    if (path != "") {
        QString absolutePath = QFileInfo(path).absoluteFilePath();
        for (int i = 0; i < mBooks.size(); i++) {
            if (absolutePath == mBooks[i]->path()) {
                return i;
            }
        }
    }
    return -1;
}

int Library::find(const Book *book) const
{
    for (int i = 0; i < mBooks.size(); i++) {
        if (book == mBooks[i]) {
            return i;
        }
    }
    return -1;
}
