#include <QtGui>

#include "settingswindow.h"
#include "settings.h"
#include "toolbuttonbox.h"
#include "platform.h"
#include "trace.h"

#ifdef Q_OS_SYMBIAN
#   include "flickcharm.h"
#endif

const int ZOOM_MIN = 75;
const int ZOOM_MAX = 250;
const int ZOOM_STEP = 25;

SettingsWindow::SettingsWindow(QWidget *parent):  AdopterWindow(parent)
{
#ifdef Q_WS_MAEMO_5
    setProperty("X-Maemo-StackedWindow", 1);
    //setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#endif
    setWindowTitle("Settings");

    Settings *settings = Settings::instance();
    Platform *platform = Platform::instance();

    QScrollArea *scroller = new QScrollArea(this);
#if defined(Q_WS_MAEMO_5)
    QScroller::grabGesture(scroller->viewport(), QScroller::LeftMouseButtonGesture);
#elif defined(Q_OS_SYMBIAN)
    FlickCharm *charm = new FlickCharm(this);
    charm->activateOn(scroller);
#else
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setFrameStyle(QFrame::NoFrame);

    QWidget *contents = new QWidget(scroller);
    QVBoxLayout *layout = new QVBoxLayout(contents);
    contents->setLayout(layout);

#if defined(Q_WS_MAEMO_5)
    QCheckBox *backlight = new QCheckBox(tr("Keep backlight on"), contents);
    layout->addWidget(backlight);
    backlight->setChecked(settings->value("lightson", false).toBool());
#endif

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    QCheckBox *grabVolume =
            new QCheckBox(tr("Navigate with volume keys"), contents);
    layout->addWidget(grabVolume);
    grabVolume->setChecked(settings->value("usevolumekeys", false).toBool());
#endif

    int zoom = settings->value("zoom", platform->defaultZoom()).toInt();
    if (zoom < ZOOM_MIN) {
        zoom = ZOOM_MIN;
    } else if (zoom > ZOOM_MAX) {
        zoom = ZOOM_MAX;
    }
    zoomLabel = new QLabel(tr("Zoom level: %1%").arg(zoom), contents);
    layout->addWidget(zoomLabel);
    zoomSlider = new QSlider(Qt::Horizontal, contents);
    zoomSlider->setMinimum(ZOOM_MIN);
    zoomSlider->setMaximum(ZOOM_MAX);
    zoomSlider->setSingleStep(ZOOM_STEP);
    zoomSlider->setPageStep(ZOOM_STEP);
    zoomSlider->setValue(zoom);
    layout->addWidget(zoomSlider);

    QLabel *fontLabel = new QLabel(tr("Font:"), contents);
    layout->addWidget(fontLabel);
    QString family =
            settings->value("font", platform->defaultFont()).toString();
    fontButton = new QFontComboBox(contents);
    fontButton->setCurrentFont(QFont(family));
    fontButton->setEditable(false);
    layout->addWidget(fontButton);

    QLabel *colorLabel = new QLabel(tr("Color scheme:"), contents);
    layout->addWidget(colorLabel);
    ToolButtonBox *box = new ToolButtonBox(this);
    layout->addWidget(box);
    box->addButton(SchemeDay, tr("Day"),
                   Platform::instance()->icon("style-day"));
    box->addButton(SchemeNight, tr("Night"),
                   Platform::instance()->icon("style-night"));
    box->addButton(SchemeSand, tr("Sand"),
                   Platform::instance()->icon("style-sand"));
    box->addStretch();
    QString scheme = settings->value("scheme", "day").toString();
    if (scheme == "night") {
        box->toggle(SchemeNight);
    } else if (scheme == "sand") {
        box->toggle(SchemeSand);
    } else {
        box->toggle(SchemeDay);
    }

    layout->addStretch();
    scroller->setWidget(contents);
    contents->show();
    scroller->setWidgetResizable(true);

    setCentralWidget(scroller);

#if defined(Q_WS_MAEMO_5)
    connect(backlight, SIGNAL(toggled(bool)),
            this, SLOT(onLightsToggled(bool)));
#endif
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    connect(grabVolume, SIGNAL(toggled(bool)),
            this, SLOT(onGrabVolumeToggled(bool)));
#endif
    connect(zoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onSliderValueChanged(int)));
    connect(fontButton, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(onCurrentFontChanged(const QFont &)));
    connect(box, SIGNAL(buttonClicked(int)),
            this, SLOT(onSchemeButtonClicked(int)));

#ifdef Q_OS_SYMBIAN
    QAction *closeAction = new QAction(parent? tr("Back"): tr("Exit"), this);
    closeAction->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
    QMainWindow::addAction(closeAction);
#endif
}

void SettingsWindow::onSliderValueChanged(int value)
{
    int step = zoomSlider->singleStep();
    if (value % step) {
        zoomSlider->setValue((value + step / 2) / step * step);
        return;
    }
    zoomLabel->setText(tr("Zoom level: %1%").arg(value));
#if !defined(Q_WS_MAEMO_5) && !defined(Q_OS_SYMBIAN)
    Settings::instance()->setValue("zoom", zoomSlider->value());
#endif
}

void SettingsWindow::onCurrentFontChanged(const QFont &font)
{
#if !defined(Q_WS_MAEMO_5) && !defined(Q_OS_SYMBIAN)
    Settings::instance()->setValue("font", font.family());
#else
    Q_UNUSED(font);
#endif
}

void SettingsWindow::onSchemeButtonClicked(int id)
{
    QString scheme;
    switch (id) {
    case SchemeNight: scheme = "night"; break;
    case SchemeSand: scheme = "sand"; break;
    default: scheme = "day"; break;
    }
    Settings::instance()->setValue("scheme", scheme);
}

void SettingsWindow::onOrientationButtonClicked(int id)
{
    QString orientation;
    switch (id) {
    case OrientationLandscape:
        orientation = "landscape";
        break;
    default:
        orientation = "portrait";
        break;
    }
    Settings::instance()->setValue("orientation", orientation);
}

void SettingsWindow::closeEvent(QCloseEvent *e)
{
    TRACE;
    Settings *settings = Settings::instance();
    settings->setValue("zoom", zoomSlider->value());
    settings->setValue("font", fontButton->currentFont().family());
    e->accept();
}

void SettingsWindow::onLightsToggled(bool value)
{
    Settings::instance()->setValue("lightson", value);
}

void SettingsWindow::onGrabVolumeToggled(bool enable)
{
    Settings::instance()->setValue("usevolumekeys", enable);
}
