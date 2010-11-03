#include <QtGui>

#include "settingswindow.h"
#include "settings.h"
#include "toolbuttonbox.h"
#include "platform.h"
#include "trace.h"

#ifdef Q_OS_SYMBIAN
#include "flickcharm.h"
#endif

#ifdef Q_OS_SYMBIAN
const char *DEFAULT_ORIENTATION = "portrait";
#else
const char *DEFAULT_ORIENTATION = "landscape";
#endif

const int ZOOM_MIN = 75;
const int ZOOM_MAX = 250;
const int ZOOM_STEP = 25;

SettingsWindow::SettingsWindow(QWidget *parent):  AdopterWindow(parent)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#endif
    setWindowTitle("Settings");

    Settings *settings = Settings::instance();
    QScrollArea *scroller = new QScrollArea(this);
#if defined(Q_WS_MAEMO_5)
    scroller->setProperty("FingerScrollable", true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

#ifndef Q_OS_SYMBIAN
    QCheckBox *backlight = new QCheckBox(tr("Keep backlight on"), contents);
    layout->addWidget(backlight);
    backlight->setChecked(settings->value("lightson", false).toBool());
#endif

#ifndef Q_OS_SYMBIAN
    QCheckBox *grabVolume =
            new QCheckBox(tr("Navigate with volume keys"), contents);
    layout->addWidget(grabVolume);
    grabVolume->setChecked(settings->value("usevolumekeys", false).toBool());
#endif

    int zoom = Settings::instance()->value("zoom").toInt();
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
    QString defaultFamily = fontLabel->fontInfo().family();
    QString family =
            Settings::instance()->value("font", defaultFamily).toString();
    fontButton = new QFontComboBox(contents);
    fontButton->setCurrentFont(QFont(family));
    fontButton->setEditable(false);
    layout->addWidget(fontButton);

    QLabel *colorLabel = new QLabel(tr("Color scheme:"), contents);
    layout->addWidget(colorLabel);
    ToolButtonBox *box = new ToolButtonBox(this);
    layout->addWidget(box);
    box->addButton(SchemeDefault, tr("Default"),
                   Platform::icon("style-default"));
    box->addButton(SchemeNight, tr("Night"), Platform::icon("style-night"));
    box->addButton(SchemeDay, tr("Day"), Platform::icon("style-day"));
    box->addButton(SchemeSand, tr("Sand"), Platform::icon("style-sand"));
    box->addStretch();
    QString scheme = settings->value("scheme", "default").toString();
    if (scheme == "night") {
        box->toggle(SchemeNight);
    } else if (scheme == "day") {
        box->toggle(SchemeDay);
    } else if (scheme == "sand") {
        box->toggle(SchemeSand);
    } else {
        box->toggle(SchemeDefault);
    }

#ifndef Q_OS_SYMBIAN
    QLabel *orientationLabel = new QLabel(tr("Orientation:"), contents);
    layout->addWidget(orientationLabel);
    orientationBox = new ToolButtonBox(this);
    layout->addWidget(orientationBox);
    orientationBox->addButton(OrientationPortrait, tr("Portrait"),
                              ":/icons/settings-portrait.png");
    orientationBox->addButton(OrientationLandscape, tr("Landscape"),
                              ":/icons/settings-landscape.png");
    orientationBox->addStretch();
    QString orientation =
        settings->value("orientation", DEFAULT_ORIENTATION).toString();
    if (orientation == "portrait") {
        orientationBox->toggle(OrientationPortrait);
    } else {
        orientationBox->toggle(OrientationLandscape);
    }
#endif // !Q_OS_SYMBIAN

    layout->addStretch();
    scroller->setWidget(contents);
    contents->show();
    scroller->setWidgetResizable(true);

    setCentralWidget(scroller);

#ifndef Q_OS_SYMBIAN
    connect(backlight, SIGNAL(toggled(bool)),
            this, SLOT(onLightsToggled(bool)));
    connect(grabVolume, SIGNAL(toggled(bool)),
            this, SLOT(onGrabVolumeToggled(bool)));
#endif
    connect(zoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onSliderValueChanged(int)));
    connect(fontButton, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(onCurrentFontChanged(const QFont &)));
    connect(box, SIGNAL(buttonClicked(int)),
            this, SLOT(onSchemeButtonClicked(int)));
#ifndef Q_OS_SYMBIAN
    connect(orientationBox, SIGNAL(buttonClicked(int)),
            this, SLOT(onOrientationButtonClicked(int)));
#endif

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
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    // Constant re-scaling of the book view is too much for mobiles
#else
    Settings::instance()->setValue("zoom", value);
#endif // Q_WS_MAEMO_5
}

void SettingsWindow::onCurrentFontChanged(const QFont &font)
{
#ifdef Q_WS_MAEMO_5
    Q_UNUSED(font);
#else
    Settings::instance()->setValue("font", font.family());
#endif // Q_WS_MAEMO_5
}

void SettingsWindow::onSchemeButtonClicked(int id)
{
    QString scheme;
    switch (id) {
    case SchemeDay: scheme = "day"; break;
    case SchemeNight: scheme = "night"; break;
    case SchemeSand: scheme = "sand"; break;
    default: scheme = "default"; break;
    }
    Settings::instance()->setValue("scheme", scheme);
}

void SettingsWindow::onOrientationButtonClicked(int id)
{
#ifdef Q_WS_MAEMO_5
    Q_UNUSED(id);
#else
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
#endif // Q_WS_MAEMO_5
}

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)

void SettingsWindow::closeEvent(QCloseEvent *e)
{
    TRACE;
    Settings *settings = Settings::instance();
    settings->setValue("zoom", zoomSlider->value());
    settings->setValue("font", fontButton->currentFont().family());
#ifndef Q_OS_SYMBIAN
    settings->setValue("orientation",
        (orientationBox->checkedId() == OrientationLandscape)?
        "landscape": "portrait");
#endif // Q_OS_SYMBIAN
    e->accept();
}

#endif // Q_WS_MAEMO_5 || Q_OS_SYMBIAN

void SettingsWindow::onLightsToggled(bool value)
{
    Settings::instance()->setValue("lightson", value);
}

void SettingsWindow::onGrabVolumeToggled(bool enable)
{
    Settings::instance()->setValue("usevolumekeys", enable);
}
