#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QList>

class QAction;

class DashboardView: public QWidget
{
    Q_OBJECT
public:
    explicit DashboardView(QWidget *parent = 0);
    QAction *viewAction;
    QList<QAction *> menuActions;

signals:

public slots:

};

#endif // DASHBOARDVIEW_H
