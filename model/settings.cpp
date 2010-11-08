#include <QSettings>

#include "settings.h"

static Settings *theInstance;

Settings::Settings(QObject *parent) :
    QObject(parent)
{
}

Settings *Settings::instance()
{
    if (!theInstance) {
        theInstance = new Settings();
    }
    return theInstance;
}

void Settings::close()
{
    delete theInstance;
    theInstance = 0;
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    QSettings s;
    s.setValue(QString("settings/") + key, value);
    emit valueChanged(key);
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    QSettings s;
    return s.value(QString("settings/") + key, defaultValue);
}
