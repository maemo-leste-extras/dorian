#ifndef OPSHANDLER_H
#define OPSHANDLER_H

#include <QXmlContentHandler>

#include "book.h"
#include "trace.h"

/** XML content handler for OPS format. */
class OpsHandler: public QXmlContentHandler
{
public:
    OpsHandler(Book &b): book(b), partCount(0) {}
    bool endDocument() {return true;}
    bool endPrefixMapping(const QString &) {return true;}
    QString errorString() const {return QString();}
    bool ignorableWhitespace(const QString &) {return true;}
    bool processingInstruction(const QString &, const QString &) {
        return true;
    }
    void setDocumentLocator(QXmlLocator *) {}
    bool skippedEntity(const QString &) {return true;}
    bool startDocument() {return true;}
    bool startPrefixMapping(const QString &, const QString &) {return true;}

    bool characters(const QString &ch) {
        currentText += ch;
        return true;
    }

    bool endElement(const QString &namespaceUri, const QString &name,
                    const QString &qName) {
        (void)namespaceUri;
        (void)qName;
        if (currentText.size()) {
            if (name == "title") {
                book.title = currentText;
            } else if (name == "creator") {
                book.creators.append(currentText);
            } else if (name == "publisher") {
                book.publisher = currentText;
            } else if (name == "subject") {
                book.subject = currentText;
            } else if (name == "source") {
                book.source = currentText;
            } else if (name == "rights") {
                book.rights = currentText;
            }
        }
        return true;
    }

    bool startElement(const QString &namespaceUri, const QString &name,
                      const QString &qName, const QXmlAttributes &attrs) {
        Trace t("OpsHandler::startElement " + name);
        (void)namespaceUri;
        (void)qName;
        currentText = "";

        if (name == "item") {
            Book::ContentItem item;
            item.href = book.rootPath() + "/" + attrs.value("href");
            item.name = QString("Part %1").arg(partCount + 1);
            item.size = 0;
            QString key = attrs.value("id");
            book.content[key] = item;
            partCount++;
            t.trace(QString("name: ") + item.name);
            t.trace(QString("href: ") + attrs.value("href"));
            t.trace(QString("id: ") + key);
        } else if (name == "itemref") {
            t.trace(QString("id: ") + attrs.value("idref"));
            book.parts.append(attrs.value("idref"));
        }
        return true;
    }

private:
    Book &book;
    QString currentText;
    int partCount;
};

#endif // OPSHANDLER_H
