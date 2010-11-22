#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QRect>
#include <QObject>

#include "adopterwindow.h"

class QWidget;
class QMouseEvent;
class QResizeEvent;
class TranslucentButton;
class Progress;

/** A full screen window with a restore button. */
class FullScreenWindow: public AdopterWindow
{
    Q_OBJECT

public:
    explicit FullScreenWindow(QWidget *parent);

    /** Swith to full screen, and flash the restore button. */
    void showFullScreen();

    /**
     * Adopt children.
     * Same as @AdopterWindow::takeChildren(), but saves prog, previous
     * and next, before calling base class method.
     */
    void takeChildren(BookView *bookView, Progress *prog,
                      TranslucentButton *previous, TranslucentButton *next);

signals:
    /** Emitted when the restore button is pressed. */
    void restore();

protected:
    /** Handle size (and orientation) change. */
    void resizeEvent(QResizeEvent *e);

private:
    TranslucentButton *restoreButton;
    Progress *progress;
    TranslucentButton *previousButton;
    TranslucentButton *nextButton;
};

#endif // FULLSCREENWINDOW_H
