#ifndef OPSHANDLER_H
#define OPSHANDLER_H

#include <QXmlContentHandler>

#include "book.h"

/** XML content handler for OPS format. */
class OpsHandler: public QXmlContentHandler
{
public:
    OpsHandler(Book &book): mBook(book) {}
    bool endDocument() {return true;}
    bool endPrefixMapping(const QString &) {return true;}
    QString errorString() const {return "";}
    bool ignorableWhitespace(const QString &) {return true;}
    bool processingInstruction(const QString &, const QString &) {
        return true;
    }
    void setDocumentLocator(QXmlLocator *) {}
    bool skippedEntity(const QString &) {return true;}
    bool startDocument() {return true;}
    bool startPrefixMapping(const QString &, const QString &) {return true;}

    bool characters(const QString &ch)
    {
        mCurrentText += ch;
        return true;
    }

    bool endElement(const QString &namespaceUri, const QString &name,
                    const QString &qName)
    {
        (void)namespaceUri;
        (void)qName;
        if (mCurrentText != "") {
            if (name == "title") {
                mBook.title = mCurrentText;
            }
            else if (name == "creator") {
                mBook.creators.append(mCurrentText);
            }
            else if (name == "publisher") {
                mBook.publisher = mCurrentText;
            }
            else if (name == "subject") {
                mBook.subject = mCurrentText;
            }
            else if (name == "source") {
                mBook.source = mCurrentText;
            }
            else if (name == "rights") {
                mBook.rights = mCurrentText;
            }
        }
        return true;
    }

    bool startElement(const QString &namespaceUri, const QString &name,
                      const QString &qName, const QXmlAttributes &attrs)
    {
        (void)namespaceUri;
        (void)name;
        (void)qName;
        (void)attrs;
        mCurrentText = "";

        if (name == "item") {
            Book::ContentItem item;
            item.href = mBook.rootPath() + "/" + attrs.value("href");
            item.type = attrs.value("media-type");
            QString key = attrs.value("id");
            mBook.content[key] = item;
            qDebug() << "OpsHandler::startElement: item" << key << "type"
                    << item.type << "href" << item.href;
        }
        else if (name == "itemref") {
            mBook.toc.append(attrs.value("idref"));
            qDebug() << "OpsHandler::startElement: itemref" << attrs.value("idref");
        }
        return true;
    }

private:
    Book &mBook;
    QString mCurrentText;
};

#endif // OPSHANDLER_H
