#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QVariantHash>
#include <QLineEdit>

#include "dyalog.h"
#include "search.h"

class QLineEdit;

/** Request author and title for searching books. */
class SearchDialog: public Dyalog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = 0);
    Search::Query query();

signals:

public slots:

protected:
    QLineEdit *author;
    QLineEdit *title;
};

#endif // SEARCHDIALOG_H
