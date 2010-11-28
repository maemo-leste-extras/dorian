#include <QtGui>

#include "splash.h"

Splash::Splash(): QSplashScreen()
{
    QRect geo = QApplication::desktop()->availableGeometry();
    if (geo.width() < geo.height()) {
        setPixmap(QPixmap(":/icons/splash.jpg").scaled(360, 518,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        setPixmap(QPixmap(":/icons/splash-landscape.jpg").scaled(520, 330,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
}
