#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVariant>

class QString;

/**
 * Shallow wrapper for QSettings.
 * Emits valueChanged signals when a setting value has changed.
 */
class Settings: public QObject
{
    Q_OBJECT

public:
    explicit Settings(QObject *parent = 0);
    static Settings *instance();
    static void close();
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant())
        const;
    void setValue(const QString &key, const QVariant &value);

signals:
    void valueChanged(const QString &name);

public slots:
};

#endif // SETTINGS_H
