#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QMainWindow>

class TranslucentButton;

/** A full screen window that can adopt a child widget from another window. */
class FullScreenWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit FullScreenWindow(QWidget *parent);
    void showFullScreen();
    void takeChild(QWidget *child);
    void leaveChild();

signals:
    void restore();

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
    QWidget *child;
};

#endif // FULLSCREENWINDOW_H
