#include <QtGui>

#include "splash.h"
#include "platform.h"

Splash::Splash(): QSplashScreen()
{
    Platform *platform = Platform::instance();
#if defined(Q_OS_SYMBIAN)
    QRect geo = QApplication::desktop()->availableGeometry();
    if (geo.width() < geo.height()) {
        setPixmap(QPixmap(platform->icon("splash", ".jpg")).scaled(360, 518,
            Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        setPixmap(QPixmap(platform->icon("splash-landscape", ".jpg")).
            scaled(520, 330,
                   Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    }
#elif defined(Q_WS_MAEMO_5)
    platform->setOrientation(this, "landscape");
    setPixmap(QPixmap(platform->icon("splash-landscape", ".jpg")));
#endif
}
