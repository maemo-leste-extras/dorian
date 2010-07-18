#ifndef SORTEDLIBRARY_H
#define SORTEDLIBRARY_H

#include <QSortFilterProxyModel>
#include "library.h"

class SortedLibrary: public QSortFilterProxyModel
{
    Q_OBJECT

public:
    enum SortBy {SortByTitle, SortByAuthor};
    explicit SortedLibrary(QObject *parent = 0);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    void sortBy(SortBy key);

signals:

public slots:

protected:
    SortBy mSortBy;
};

#endif // SORTEDLIBRARY_H
