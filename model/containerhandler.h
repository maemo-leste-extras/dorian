#ifndef CONTAINERHANDLER_H
#define CONTAINERHANDLER_H

#include <QXmlContentHandler>
#include <QString>

/** XML content handler for EPUB container format. */
class ContainerHandler: public QXmlContentHandler
{
public:
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
    bool characters(const QString &) {return true;}
    bool endElement(const QString &, const QString &, const QString &) {
        return true;
    }

    bool startElement(const QString &namespaceUri, const QString &name,
                      const QString &qName, const QXmlAttributes &attrs)
    {
        (void)namespaceUri;
        (void)qName;
        if (name == "rootfile") {
            rootFile = attrs.value("full-path");
        }
        return true;
    }

    QString rootFile;
};

#endif // CONTAINERHANDLER_H
