#include <QtGui>

#include "splash.h"

Splash::Splash(QWidget *parent): QMainWindow(parent)
{
    QLabel *label = new QLabel(this);
    QRect geo = QApplication::desktop()->geometry();
    label->setGeometry(geo);
    label->setStyleSheet("background-color: black; color: black");
    label->setPixmap(QPixmap(":/icons/splash.jpg").scaled(geo.width(),
        geo.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setCentralWidget(label);
}
