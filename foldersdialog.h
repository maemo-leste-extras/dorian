#ifndef FOLDERSDIALOG_H
#define FOLDERSDIALOG_H

class QStringListModel;
class QWidget;

#include "listwindow.h"

/** Dialog to manage folders monitored by the library. */
class FoldersDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit FoldersDialog(QWidget *parent = 0);

signals:

public slots:
    void onAdd();
    void onRemove();
    void onRefresh();
    void onRefreshDone(int added, int removed);

protected:
    QStringListModel *model;
};

#endif // FOLDERSDIALOG_H
