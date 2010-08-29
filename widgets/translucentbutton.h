#ifndef TRANSLUCENTBUTTON_H
#define TRANSLUCENTBUTTON_H

#include <QtGui>
#include <QString>

class TranslucentButton: public QWidget
{
    Q_OBJECT

public:
    explicit TranslucentButton(const QString &name, QWidget *parent);

public slots:
    void flash(int duration = 3000);
    void stopFlash();

protected:
    virtual void paintEvent(QPaintEvent *);
    QString name;
    int opacity;
    QTimer *timer;
};

#endif // TRANSLUCENTBUTTON_H
