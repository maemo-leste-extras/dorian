#ifndef TRANSLUCENTBUTTON_H
#define TRANSLUCENTBUTTON_H

#include <QLabel>
#include <QString>

class QWidget;
class QPaintEvent;
class QMouseEvent;
class QTimer;

/**
 * A button that is transparent, but can temporarily made opaque by flashing it.
 */
class TranslucentButton: public QLabel
{
    Q_OBJECT

public:
    explicit TranslucentButton(const QString &iconName, QWidget *parent);
    ~TranslucentButton();
    static const int pixels;
    static const int elevatorInterval;

public slots:
    void flash(int duration = 700);
    void stopFlash();

signals:
    void triggered();

protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *e);

private:
    QString name;
    bool transparent;
    QTimer *timer;
    int elevatorTimer;
};

#endif // TRANSLUCENTBUTTON_H
