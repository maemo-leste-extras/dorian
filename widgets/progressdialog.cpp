#include <QString>
#include <QtGui>

#include "progressdialog.h"
#include "trace.h"

ProgressDialog::ProgressDialog(const QString &label, QWidget *parent):
    QProgressDialog(label, "", 0, 0, parent)
{
    TRACE;
    setMinimumDuration(0);
    setWindowModality(Qt::WindowModal);
    setCancelButton(0);
#ifdef Q_WS_S60
    setFixedWidth(QApplication::desktop()->availableGeometry().width());
    qDebug() << "Height" << height();
#endif
}

void ProgressDialog::showWait()
{
    setMinimum(0);
    setMaximum(0);
    setValue(0);
    show();
}
