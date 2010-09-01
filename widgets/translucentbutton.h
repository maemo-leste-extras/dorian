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
    static const int pixels;
    explicit TranslucentButton(const QString &iconName, QWidget *parent);

public slots:
    void flash(int duration = 3000);
    void stopFlash();

signals:
    void triggered();

protected:
    void paintEvent(QPaintEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    QString name;
    bool transparent;
    QTimer *timer;
};

#endif // TRANSLUCENTBUTTON_H
