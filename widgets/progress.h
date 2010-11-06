#ifndef PROGRESS_H
#define PROGRESS_H

#include <QWidget>
#include <QLabel>

/** Subtle progress bar. */
class Progress: public QLabel
{
    Q_OBJECT

public:
    explicit Progress(QWidget *parent = 0);

signals:

public slots:
    void setProgress(qreal p);
    void flash();

protected:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);

private:
    qreal progress;
    int timer;
};

#endif // PROGRESS_H
