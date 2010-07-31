#ifndef TRACE_H
#define TRACE_H

#include <QtDebug>
#include <QString>
#include <QTime>
#include <QEvent>

class Trace
{
public:
    Trace(const QString &s): name(s) {
        qDebug() << prefix() + ">" + name;
        indent++;
    }
    ~Trace() {
        if (--indent < 0) {
            indent = 0;
        }
        qDebug() << prefix() + "<" + name;
    }
    void trace(const QString &s) const {
        qDebug() << prefix()  + name + ": " + s;
    }

    static void debug(const QString &s) {
        qDebug() << prefix()  + s;
    }

    static QString event(QEvent::Type t) {
        for (int i = 0; eventTab[i].name; i++) {
            if (eventTab[i].type == t) {
                return eventTab[i].name;
            }
        }
        if (t >= QEvent::User) {
            return QString("QEvent::User+%1").arg(t - QEvent::User);
        } else {
            return QString("Unknown event %1").arg(t);
        }
    }

protected:
    static QString prefix() {
        return QTime::currentTime().toString("hh:mm:ss.zzz ") +
            QString(" ").repeated(indent);
    }
    QString name;
    static int indent;
    typedef struct {int type; const char *name;} EventName;
    static EventName eventTab[];
};

#endif // TRACE_H
