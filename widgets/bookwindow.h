#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QMainWindow>
#include <QList>

class QWidget;

class BookWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit BookWindow(QWidget *parent = 0);
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

#endif // BOOKWINDOW_H
