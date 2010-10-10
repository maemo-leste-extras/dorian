#include "sortedlibrary.h"
#include "book.h"
#include "trace.h"

SortedLibrary::SortedLibrary(QObject *parent):
        QSortFilterProxyModel(parent), mSortBy(SortByTitle)
{
    setSourceModel(Library::instance());
    sort(0);
}

void SortedLibrary::sortBy(SortBy key)
{
    mSortBy = key;
}

bool SortedLibrary::lessThan(const QModelIndex &left,
                             const QModelIndex &right) const
{
    Book *leftBook = Library::instance()->book(left);
    Book *rightBook = Library::instance()->book(right);

    QString leftString;
    QString rightString;

    switch (mSortBy) {
    case SortByTitle:
        leftString = leftBook->shortName();
        rightString = rightBook->shortName();
        break;
    default:
        leftString = leftBook->creators[0];
        rightString = rightBook->creators[0];
        break;
    }

    return QString::localeAwareCompare(leftString, rightString) < 0;
}
