#include <QtGui>

#include "searchresultinfodialog.h"
#include "search.h"
#include "trace.h"

SearchResultInfoDialog::SearchResultInfoDialog(const Search::Result &result_,
                                               QWidget *parent):
    Dyalog(parent), result(result_)
{
    setWindowTitle(tr("Book Details"));

    QLabel *title = new QLabel(result.title, this);
    addWidget(title);
    if (result.authors.size()) {
        QLabel *creators = new QLabel(this);
        QString c = "By " + result.authors[0];
        for (int i = 1; i < result.authors.size(); i++) {
            c += ", " + result.authors[i];
        }
        creators->setText(c);
        creators->setWordWrap(true);
        addWidget(creators);
    }
    QLabel *source = new QLabel(tr("Source: %1").arg(result.source), this);
    addWidget(source);
    addStretch();
    addButton(tr("Download"), this, SLOT(accept()), QDialogButtonBox::ActionRole);
    addButton(tr("Close"), this, SLOT(reject()), QDialogButtonBox::DestructiveRole);
}
