#include <QtGui>

#include "searchdialog.h"
#include "search.h"
#include "trace.h"

SearchDialog::SearchDialog(QWidget *parent): Dyalog(parent)
{
    setWindowTitle(tr("SearchDialog"));

    QLabel *titleLabel = new QLabel(tr("Title:"), this);
    title = new QLineEdit(this);
    QLabel *authorLabel = new QLabel(tr("Author:"), this);
    author = new QLineEdit(this);

    addWidget(titleLabel);
    addWidget(title);
    addWidget(authorLabel);
    addWidget(author);
    addStretch();
    addButton(tr("Search"), this, SLOT(accept()));
}

Search::Query SearchDialog::query()
{
    TRACE;
    Search::Query ret;
    ret.title = title->text();
    ret.author = author->text();
    qDebug() << ret.title << ret.author;
    return ret;
}
