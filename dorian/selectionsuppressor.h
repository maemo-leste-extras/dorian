#ifndef SELECTIONSUPPRESSOR_H
#define SELECTIONSUPPRESSOR_H

#include <QWebView>
#include <QEvent>
#include <QMouseEvent>

class SelectionSuppressor: public QObject
{
    Q_OBJECT

public:
    SelectionSuppressor(QWebView *view): QObject(view), mousePressed(false)
    {
        view->installEventFilter(this);
    }

protected:
    inline bool eventFilter(QObject *, QEvent *e);

private:
    bool mousePressed;
};

bool SelectionSuppressor::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
        mousePressed = true;
        break;
    case QEvent::MouseButtonRelease:
        mousePressed = false;
        break;
    case QEvent::MouseMove:
        if (mousePressed) {
            return true;
        }
        break;
    case QEvent::MouseButtonDblClick:
        return true;
    default:
        break;
    }
    return false;
}

#endif // SELECTIONSUPPRESSOR_H
