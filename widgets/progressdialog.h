#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QString>
#include <QProgressDialog>

/** Simple wrapper for QProgressDialog. */
class ProgressDialog: public QProgressDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QString &label, QWidget *parent = 0);

    /** Show wait animation instead of progress bar. */
    void showWait();
};

#endif // PROGRESSDIALOG_H
