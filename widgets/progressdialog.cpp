#include <QString>
#include <QtGui>

#include "progressdialog.h"

ProgressDialog::ProgressDialog(const QString &label, QWidget *parent):
    QProgressDialog(label, "", 0, 0, parent)
{
    setMinimumDuration(0);
    setWindowModality(Qt::WindowModal);
    setCancelButton(0);
#ifdef Q_WS_S60
    setFixedWidth(QApplication::desktop()->availableGeometry().width());
#endif
}
