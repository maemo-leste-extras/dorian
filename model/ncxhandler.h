#ifndef NCXHANDLER_H
#define NCXHANDLER_H

#include <QXmlContentHandler>

#include "book.h"
#include "trace.h"

/** XML content handler for NCX format. */
class NcxHandler: public QXmlContentHandler
{
public:
    bool endDocument() {return true;}
    bool endPrefixMapping(const QString &) {return true;}
    QString errorString() const {return "";}
    bool ignorableWhitespace(const QString &) {return true;}
    bool processingInstruction(const QString &, const QString &) {return true;}
    void setDocumentLocator(QXmlLocator *) {}
    bool skippedEntity(const QString &) {return true;}
    bool startDocument() {return true;}
    bool startPrefixMapping(const QString &, const QString &) {return true;}

    NcxHandler(Book &b): book(b) {
        book.chapters.clear();
    }

    bool characters(const QString &ch) {
        currentText += ch;
        return true;
    }

    bool endElement(const QString &namespaceUri, const QString &name,
                    const QString &qName) {
        Trace t("NcxHandler::endElement" + name);
        (void)namespaceUri;
        (void)qName;
        if (name == "text") {
            contentTitle = currentText;
        } else if (name == "navPoint") {
            t.trace(QString("url: ") + contentUrl);
            t.trace(QString("title: ") + contentTitle);
            t.trace(QString("id:") + contentId);
            Book::ContentItem item;
            item.href = book.rootPath() + "/" + contentUrl;
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
    QString currentText;
    QString contentId;
    QString contentUrl;
    QString contentTitle;
};

#endif // NCXHANDLER_H
