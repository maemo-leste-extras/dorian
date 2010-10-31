#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QString>
#include <QProgressDialog>

class ProgressDialog: public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QString &label, QWidget *parent = 0);
    void showWait();

signals:

public slots:

};

#endif // PROGRESSDIALOG_H
