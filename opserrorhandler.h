#ifndef OPSERRORHANDLER_H
#define OPSERRORHANDLER_H

#include <QXmlErrorHandler>
#include <QDebug>

#include "book.h"

/**
  XML error handler for OPS format.
  */
class OpsErrorHandler: public QXmlErrorHandler
{
    bool error(const QXmlParseException &e) {
        qDebug() << "OpsErrorHandler::error" << e.message() << "at line"
                << e.lineNumber();
        return true;
    }
    virtual QString errorString() const {return QString();}
    virtual bool fatalError(const QXmlParseException &e) {
        qDebug() << "OpsErrorHandler::fatalError" << e.message() << "at line"
                <<  e.lineNumber();
        return true;
    }
    virtual bool warning(const QXmlParseException &e) {
        qDebug() << "OpsErrorHandler::warning" << e.message() << "at line"
                << e.lineNumber();
        return true;
    }
};

#endif // OPSERRORHANDLER_H
