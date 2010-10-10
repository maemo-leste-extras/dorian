#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QVariantHash>

#include "dyalog.h"
#include "search.h"

class QLineEdit;

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
