#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QList>
#include <QLayout>
#include "mainbase.h"

class QWidget;
class QAction;
class BookView;
class Progress;
class TranslucentButton;

/**
 * A toplevel window that can adopt a BookView and other children.
 * On some platforms, it can also grab the volume keys.
 */
class AdopterWindow: public MainBase
{
    Q_OBJECT

public:
    explicit AdopterWindow(QWidget *parent = 0);

    /** Adopt book view and decorations. */
    void takeBookView(BookView *bookView, Progress *prog,
                      TranslucentButton *prev, TranslucentButton *next);

    /** Release book view and decorations. */
    void leaveBookView();

    /** Return true if the book view is currently adopted. */
    bool hasBookView();

    /** If grab is true, volume keys will navigate the book view. */
    void grabVolumeKeys(bool grab);

public slots:
    /** Handle settings changes. */
    void onSettingsChanged(const QString &key);

signals:
    /** Emitted when Page Up or Volume Up pressed. */
    void pageUp();

    /** Emitted when Page Down or Volume Down pressed. */
    void pageDown();

protected:
    /** Handle key press events. */
    void keyPressEvent(QKeyEvent *event);

    /**
     * Handle show events.
     * On Symbian, volume keys can only be grabbed, when the window is shown.
     * So we do it from here.
     */
    void showEvent(QShowEvent *event);

    /** Handle resize event: Restore reading position. */
    void resizeEvent(QResizeEvent *event);

    /** Handle close event: Save reading position. */
    void closeEvent(QCloseEvent *event);

    /** Handle leave event: Save reading position. */
    void leaveEvent(QEvent *event);

#ifdef Q_WS_MAEMO_5
    /** Actually grab the volume keys. */
    void doGrabVolumeKeys(bool grab);
#endif // Q_WS_MAEMO_5

protected slots:
    void placeDecorations();
    void onPageDown();
    void onPageUp();

private:
    BookView *bookView;     /**< Book view widget. */
    bool grabbingVolumeKeys;/**< True, if volume keys should be grabbed. */
    Progress *progress;     /**< Reading progress indicator. */
    TranslucentButton *previousButton;  /**< Previous page indicator. */
    TranslucentButton *nextButton;      /**< Next page indicator. */
};

#endif // ADOPTERWINDOW_H
