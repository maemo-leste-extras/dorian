#include <QtGui>

#include "settingswindow.h"
#include "settings.h"

#ifdef Q_OS_SYMBIAN
#define DEFAULT_ORIENTATION "portrait"
#else
#define DEFAULT_ORIENTATION "landscape"
#endif

SettingsWindow::SettingsWindow(QWidget *parent):  QMainWindow(parent)
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5StackedWindow, true);
    setAttribute(Qt::WA_Maemo5AutoOrientation, true);
#endif
    setWindowTitle("Settings");

    QScrollArea *scroller = new QScrollArea(this);
#ifdef Q_WS_MAEMO_5
    scroller->setProperty("FingerScrollable", true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroller->setFrameStyle(QFrame::NoFrame);

    QWidget *contents = new QWidget(scroller);
    QVBoxLayout *layout = new QVBoxLayout(contents);
    contents->setLayout(layout);

    QLabel *zoomLabel = new QLabel(tr("Zoom level:"), contents);
    layout->addWidget(zoomLabel);
    QSlider *zoomSlider = new QSlider(Qt::Horizontal, contents);
    zoomSlider->setMinimum(50);
    zoomSlider->setMaximum(300);
    zoomSlider->setValue(Settings::instance()->value("zoom").toInt());
    layout->addWidget(zoomSlider);

    QLabel *fontLabel = new QLabel(tr("Font:"), contents);
    layout->addWidget(fontLabel);
    QString defaultFamily = fontLabel->fontInfo().family();
    QString family = Settings::instance()->value("font", defaultFamily).toString();
    QFontComboBox *fontButton = new QFontComboBox(contents);
    fontButton->setCurrentFont(QFont(family));
    fontButton->setEditable(false);
    layout->addWidget(fontButton);

    QLabel *colorLabel = new QLabel(tr("Color scheme:"), contents);
    layout->addWidget(colorLabel);
    QFrame *box = new QFrame(this);
    layout->addWidget(box);
    QHBoxLayout *boxLayout = new QHBoxLayout(box);
    boxLayout->setMargin(0);
    box->setLayout(boxLayout);
    QButtonGroup *group = new QButtonGroup(this);

    QToolButton *defaultSchemeButton = new QToolButton(box);
    defaultSchemeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    defaultSchemeButton->setText(tr("Default"));
    defaultSchemeButton->setIconSize(QSize(81, 81));
    defaultSchemeButton->setIcon(QIcon(":/icons/style-default.png"));
    defaultSchemeButton->setCheckable(true);
    boxLayout->addWidget(defaultSchemeButton);
    group->addButton(defaultSchemeButton);
    group->setId(defaultSchemeButton, SchemeDefault);

    QToolButton *nightSchemeButton = new QToolButton(box);
    nightSchemeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    nightSchemeButton->setText(tr("Night"));
    nightSchemeButton->setCheckable(true);
    nightSchemeButton->setIconSize(QSize(81, 81));
    nightSchemeButton->setIcon(QIcon(":/icons/style-night.png"));
    boxLayout->addWidget(nightSchemeButton);
    group->addButton(nightSchemeButton);
    group->setId(nightSchemeButton, SchemeNight);

    QToolButton *daySchemeButton = new QToolButton(box);
    daySchemeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    daySchemeButton->setText(tr("Day"));
    daySchemeButton->setCheckable(true);
    daySchemeButton->setIconSize(QSize(81, 81));
    daySchemeButton->setIcon(QIcon(":/icons/style-day.png"));
    boxLayout->addWidget(daySchemeButton);
    group->addButton(daySchemeButton);
    group->setId(daySchemeButton, SchemeDay);

    QToolButton *sandSchemeButton = new QToolButton(box);
    sandSchemeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    sandSchemeButton->setText(tr("Sand"));
    sandSchemeButton->setCheckable(true);
    sandSchemeButton->setIconSize(QSize(81, 81));
    sandSchemeButton->setIcon(QIcon(":/icons/style-sand.png"));
    boxLayout->addWidget(sandSchemeButton);
    group->addButton(sandSchemeButton);
    group->setId(sandSchemeButton, SchemeSand);

    QString scheme = Settings::instance()->value("scheme", "default").toString();
    if (scheme == "night") {
        nightSchemeButton->toggle();
    }
    else if (scheme == "day") {
        daySchemeButton->toggle();
    }
    else if (scheme == "sand") {
        sandSchemeButton->toggle();
    }
    else {
        defaultSchemeButton->toggle();
    }

    QLabel *orientationLabel = new QLabel(tr("Orientation:"), contents);
    layout->addWidget(orientationLabel);

    QFrame *orientationBox = new QFrame(this);
    layout->addWidget(orientationBox);
    QHBoxLayout *orientationLayout = new QHBoxLayout(orientationBox);
    orientationLayout->setMargin(0);
    orientationBox->setLayout(orientationLayout);
    QButtonGroup *orientationGroup = new QButtonGroup(this);

    QToolButton *portraitButton = new QToolButton(box);
    portraitButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    portraitButton->setText(tr("Portrait"));
    portraitButton->setIconSize(QSize(81, 81));
    portraitButton->setIcon(QIcon(":/icons/settings-portrait.png"));
    portraitButton->setCheckable(true);
    orientationLayout->addWidget(portraitButton);
    orientationGroup->addButton(portraitButton);
    orientationGroup->setId(portraitButton, OrientationPortrait);

    QToolButton *landscapeButton = new QToolButton(box);
    landscapeButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    landscapeButton->setText(tr("Landscape"));
    landscapeButton->setIconSize(QSize(81, 81));
    landscapeButton->setIcon(QIcon(":/icons/settings-landscape.png"));
    landscapeButton->setCheckable(true);
    orientationLayout->addWidget(landscapeButton);
    orientationGroup->addButton(landscapeButton);
    orientationGroup->setId(landscapeButton, OrientationLandscape);

    orientationLayout->addStretch();

    QString orientation =
        Settings::instance()->value("orientation", DEFAULT_ORIENTATION).toString();
    if (orientation == "portrait") {
        portraitButton->toggle();
    }
    else {
        landscapeButton->toggle();
    }

    boxLayout->addStretch();
    layout->addStretch();
    scroller->setWidget(contents);
    contents->show();
    scroller->setWidgetResizable(true);

    setCentralWidget(scroller);

    connect(zoomSlider, SIGNAL(valueChanged(int)),
            this, SLOT(onSliderValueChanged(int)));
    connect(fontButton, SIGNAL(currentFontChanged(const QFont &)),
            this, SLOT(onCurrentFontChanged(const QFont &)));
    connect(group, SIGNAL(buttonClicked(int)),
            this, SLOT(onSchemeButtonClicked(int)));
    connect(orientationGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(onOrientationButtonClicked(int)));
}

void SettingsWindow::onSliderValueChanged(int value)
{
    Settings::instance()->setValue("zoom", value);
}

void SettingsWindow::onCurrentFontChanged(const QFont &font)
{
    Settings::instance()->setValue("font", font.family());
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
    QString orientation;
    switch (id) {
    case OrientationLandscape: orientation = "landscape"; break;
    default: orientation = "portrait"; break;
    }
    Settings::instance()->setValue("orientation", orientation);
}
