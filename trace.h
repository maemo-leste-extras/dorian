#ifndef TRACE_H
#define TRACE_H

#include <QtDebug>
#include <QString>
#include <QTime>
#include <QEvent>

/** Trace helper. */
class Trace
{
public:
    Trace(const QString &s);
    ~Trace();
    static QString event(QEvent::Type t);
    static void messageHandler(QtMsgType type, const char *msg);
    static QtMsgType level;

protected:
    static QString prefix();
    QString name;
    static int indent;
    typedef struct {int type; const char *name;} EventName;
    static EventName eventTab[];
};

#endif // TRACE_H
