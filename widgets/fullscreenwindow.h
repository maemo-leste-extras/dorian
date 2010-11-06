#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QRect>
#include <QObject>

#include "adopterwindow.h"

class QWidget;
class QMouseEvent;
class QResizeEvent;
class TranslucentButton;

/** A full screen window with a restore button. */
class FullScreenWindow: public AdopterWindow
{
    Q_OBJECT

public:
    explicit FullScreenWindow(QWidget *parent);
    void showFullScreen();

signals:
    /** Emitted when the restore button is pressed. */
    void restore();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    TranslucentButton *restoreButton;
};

#endif // FULLSCREENWINDOW_H
