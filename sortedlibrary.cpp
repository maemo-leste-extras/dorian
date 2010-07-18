#include "sortedlibrary.h"
#include "book.h"

SortedLibrary::SortedLibrary(QObject *parent): QSortFilterProxyModel(parent)
{
    setSourceModel(Library::instance());
}

void SortedLibrary::sortBy(SortBy key)
{
    mSortBy = key;
}

bool SortedLibrary::lessThan(const QModelIndex &left,
                             const QModelIndex &right) const
{
    Book leftBook = sourceModel()->data(left, Library::BookRole).value<Book>();
    Book rightBook = sourceModel()->data(right, Library::BookRole).value<Book>();

    QString leftString;
    QString rightString;

    switch (mSortBy) {
    case SortByTitle:
        leftString = leftBook.title;
        rightString = rightBook.title;
        break;
    default:
        leftString = leftBook.creators[0];
        rightString = rightBook.creators[0];
        break;
    }

    return QString::localeAwareCompare(leftString, rightString);
}
