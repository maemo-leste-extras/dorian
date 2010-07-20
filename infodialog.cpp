#include <QtGui>

#include "infodialog.h"
#include "info.h"
#include "book.h"
#include "library.h"

InfoDialog::InfoDialog(Book *book_, QWidget *parent):
        QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint),
        book(book_)
{
    setWindowTitle(tr("Book Details"));
    Info *info = new Info(book);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Vertical, this);
    QPushButton *read = new QPushButton(tr("Read"), this);
    QPushButton *remove = new QPushButton(tr("Delete"), this);
    connect(read, SIGNAL(clicked()), this, SLOT(onReadBook()));
    connect(remove, SIGNAL(clicked()), this, SLOT(onRemoveBook()));
    buttonBox->addButton(read, QDialogButtonBox::ActionRole);
    buttonBox->addButton(remove, QDialogButtonBox::ActionRole);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(this);
    horizontalLayout->addWidget(info);
    horizontalLayout->addWidget(buttonBox);
    setLayout(horizontalLayout);
}

void InfoDialog::onReadBook()
{
    Library::instance()->setNowReading(Library::instance()->find(book));
    close();
}

void InfoDialog::onRemoveBook()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this,
                              tr("Delete book"),
                              "Delete book \"" + book->name() + "\"?",
                              QMessageBox::Yes | QMessageBox::No)) {
        Library::instance()->remove(Library::instance()->find(book));
        close();
    }
}
