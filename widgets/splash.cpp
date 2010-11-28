#include <QtGui>

#include "splash.h"

Splash::Splash(): QSplashScreen(QPixmap(":/icons/splash.jpg").scaled(360, 518,
                      Qt::IgnoreAspectRatio, Qt::SmoothTransformation))
{
}
