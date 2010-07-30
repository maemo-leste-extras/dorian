#ifndef TRACE_H
#define TRACE_H

#include <QtDebug>

class Trace
{
public:
    Trace(const QString &s): name(s) {
        qDebug() << QString(" ").repeated(indent) + ">" + name;
        indent++;
    }
    ~Trace() {
        if (--indent < 0) {
            indent = 0;
        }
        qDebug() << QString(" ").repeated(indent) + "<" + name;
    }
    void trace(const QString &s) const {
        qDebug() << QString(" ").repeated(indent)  + name + ": " + s;
    }

protected:
    QString name;
    static int indent;
};

#endif // TRACE_H
