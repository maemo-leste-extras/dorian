#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QRect>
#include <QObject>
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>

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
    explicit FullScreenWindow(QWidget *parent = 0);

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

    /** Handle show event. */
    void showEvent(QShowEvent *e);

    /** Handle close event. */
    void closeEvent(QCloseEvent *e);

protected slots:
    /** Re-align adopted child windows. */
    void placeChildren();

private:
    TranslucentButton *restoreButton;
};

#endif // FULLSCREENWINDOW_H
