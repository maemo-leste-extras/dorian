#ifndef NCXHANDLER_H
#define NCXHANDLER_H

#include "xmlhandler.h"
#include "book.h"
#include "trace.h"

/** XML content handler for NCX format. */
class NcxHandler: public XmlHandler
{
public:
    NcxHandler(Book &b): book(b) {
        book.chapters.clear();
    }

    bool endElement(const QString &namespaceUri, const QString &name,
                    const QString &qName) {
        (void)namespaceUri;
        (void)qName;
        if (name == "text") {
            contentTitle = currentText;
        } else if (name == "navPoint") {
            qDebug() << "NcxHander::endElement: url" << contentUrl << "title"
                    << contentTitle << "id" << contentId;
            Book::ContentItem item;
            item.href = contentUrl;
            item.name = contentTitle;
            book.content[contentId] = item;
            book.chapters.append(contentId);
        }
        return true;
    }

    bool startElement(const QString &namespaceUri, const QString &name,
                      const QString &qName, const QXmlAttributes &attrs) {
        (void)namespaceUri;
        (void)qName;
        currentText = "";
        if (name == "navPoint") {
            contentId = attrs.value("id");
        } else if (name == "content") {
            contentUrl = attrs.value("src");
        }
        return true;
    }

private:
    Book &book;
    QString contentId;
    QString contentUrl;
    QString contentTitle;
};

#endif // NCXHANDLER_H
