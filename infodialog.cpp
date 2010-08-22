#include <QtGui>

#include "infodialog.h"
#include "book.h"
#include "library.h"

InfoDialog::InfoDialog(Book *b, QWidget *parent): Dyalog(parent), book(b)
{
    setWindowTitle(tr("Book Details"));

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

    QPushButton *read = new QPushButton(tr("Read"), this);
    QPushButton *remove = new QPushButton(tr("Delete"), this);
    connect(read, SIGNAL(clicked()), this, SLOT(onReadBook()));
    connect(remove, SIGNAL(clicked()), this, SLOT(onRemoveBook()));
    addButton(read, QDialogButtonBox::ActionRole);
    addButton(remove, QDialogButtonBox::DestructiveRole);
}

void InfoDialog::onReadBook()
{
    Library::instance()->setNowReading(Library::instance()->find(book));
    close();
}

void InfoDialog::onRemoveBook()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this, tr("Delete book"),
            tr("Delete book \"%1\" from library?").arg(book->shortName()),
            QMessageBox::Yes | QMessageBox::No)) {
        Library::instance()->remove(Library::instance()->find(book));
        close();
    }
}
