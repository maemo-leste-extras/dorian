#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QMainWindow>

class TranslucentButton;

class FullScreenWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit FullScreenWindow(QWidget *child, QWidget *parent);
    void showFullScreen();

signals:
    void restore();

public slots:

protected:
#ifdef Q_WS_MAEMO_5
#   define MOUSE_ACTIVATE_EVENT mouseReleaseEvent
#else
#   define MOUSE_ACTIVATE_EVENT mousePressEvent
#endif
    virtual void MOUSE_ACTIVATE_EVENT(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    QRect fullScreenZone() const;
    TranslucentButton *restoreButton;
};

#endif // FULLSCREENWINDOW_H
