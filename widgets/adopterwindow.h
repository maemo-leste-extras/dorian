#ifndef ADOPTERWINDOW_H
#define ADOPTERWINDOW_H

#include <QMainWindow>
#include <QList>

class QWidget;

/**
  * A main window that can adopt other windows' children, and grabs the
  * zoom (volume) keys on Maemo.
  */
class AdopterWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit AdopterWindow(QWidget *parent = 0);
    void grabZoomKeys(bool grab);
    void takeChildren(QWidget *main, const QList<QWidget *> &others);
    void leaveChildren();

signals:

public slots:

protected:
    void showEvent(QShowEvent *e);
    void doGrabZoomKeys(bool grab);
    bool grabbingZoomKeys;
    QWidget *mainChild;
};

#endif // ADOPTERWINDOW_H
