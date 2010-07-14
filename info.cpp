#include <QtGui>
#include <QDebug>

#include "info.h"
#include "book.h"

Info::Info(Book *book, QWidget *parent): QScrollArea(parent)
{
#ifdef Q_WS_MAEMO_5
    setProperty("FingerScrollable", true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameStyle(QFrame::NoFrame);

    QWidget *contents = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(contents);
    layout->setMargin(0);
    contents->setLayout(layout);

    if (book) {
        QLabel *title = new QLabel(book->title, contents);
        layout->addWidget(title);
        if (book->subject != "") {
            QLabel *subject = new QLabel(book->subject, contents);
            layout->addWidget(subject);
        }
        if (book->creators.size()) {
            QLabel *creators = new QLabel(contents);
            QString c = "By " + book->creators[0];
            for (int i = 1; i < book->creators.size(); i++) {
                c += ", " + book->creators[i];
            }
            creators->setText(c);
            layout->addWidget(creators);
        }
        QLabel *path = new QLabel("File: " + book->path(), contents);
        layout->addWidget(path);
        if (book->publisher != "") {
            QLabel *publisher =
                    new QLabel("Published by " + book->publisher, contents);
            layout->addWidget(publisher);
        }
        if (book->source != "") {
            QLabel *source = new QLabel("Source: " + book->source, contents);
            layout->addWidget(source);
        }
        if (book->rights != "") {
            QLabel *rights = new QLabel(book->rights, contents);
            layout->addWidget(rights);
        }
    }

    layout->addStretch();
    setWidget(contents);
    contents->show();
    setWidgetResizable(true);
}
