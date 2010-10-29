#include <QString>
#include <QtGui>

#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString &label, QWidget *parent):
    QProgressDialog(label, "", 0, 0, parent)
{
}
