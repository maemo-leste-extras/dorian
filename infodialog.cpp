#include <QtGui>

#include "infodialog.h"
#include "book.h"
#include "library.h"
#include "trace.h"

InfoDialog::InfoDialog(Book *b, QWidget *parent, bool showButtons):
        Dyalog(parent, showButtons), book(b)
{
    TRACE;

    setWindowTitle(tr("Book details"));

    if (book) {
        QLabel *title = new QLabel(book->title, this);
        addWidget(title);
        if (book->subject != "") {
            QLabel *subject = new QLabel(book->subject, this);
            addWidget(subject);
        }
        if (book->creators.size()) {
            QLabel *creators = new QLabel(this);
            QString c = "By " + book->creators[0];
            for (int i = 1; i < book->creators.size(); i++) {
                c += ", " + book->creators[i];
            }
            creators->setText(c);
            addWidget(creators);
        }
        QLabel *path = new QLabel("File: " + book->path(), this);
        addWidget(path);
        if (book->publisher != "") {
            QLabel *publisher =
                    new QLabel("Published by " + book->publisher, this);
            addWidget(publisher);
        }
        if (book->source != "") {
            QLabel *source = new QLabel("Source: " + book->source, this);
            addWidget(source);
        }
        if (book->rights != "") {
            QLabel *rights = new QLabel(book->rights, this);
            addWidget(rights);
        }
        addStretch();
    }

    addButton(tr("Read"), this, SLOT(onReadBook()),
              QDialogButtonBox::ActionRole);
    addButton(tr("Delete"), this, SLOT(onRemoveBook()),
              QDialogButtonBox::DestructiveRole);
}

void InfoDialog::onReadBook()
{
    done(InfoDialog::Read);
}

void InfoDialog::onRemoveBook()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this, tr("Delete book"),
            tr("Delete book \"%1\" from library?").arg(book->shortName()),
            QMessageBox::Yes | QMessageBox::No)) {
        done(InfoDialog::Delete);
    }
}
