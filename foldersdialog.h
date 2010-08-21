#ifndef FOLDERSDIALOG_H
#define FOLDERSDIALOG_H

class QStringListModel;
class QWidget;
class QProgressDialog;

#include "listwindow.h"

/** Dialog to manage folders monitored by the library. */
class FoldersDialog: public ListWindow
{
    Q_OBJECT

public:
    explicit FoldersDialog(QWidget *parent = 0);

public slots:
    void onAdd();
    void onRemove();
    void onRefresh();
    void onRefreshAll();
    void onRefreshDone(int added, int removed);
    void onAddBook(const QString &path);

protected:
    void refresh(const QStringList &folders);
    QStringListModel *model;
    QProgressDialog *progress;
};

#endif // FOLDERSDIALOG_H
