#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QRect>
#include <QObject>

#include "bookwindow.h"

class QWidget;
class QMouseEvent;
class QResizeEvent;
class TranslucentButton;

/** A full screen window that can adopt a child widget from another window. */
class FullScreenWindow: public BookWindow
{
    Q_OBJECT

public:
    explicit FullScreenWindow(QWidget *parent);
    void showFullScreen();

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
};

#endif // FULLSCREENWINDOW_H
