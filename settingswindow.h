#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QLabel>
#include <QSlider>

#include "adopterwindow.h"

class QPushButton;
class QFontComboBox;
class QFont;
class QSlider;
class QFontComboBox;
class QLabel;
class ToolButtonBox;

/** Display settings. */
class SettingsWindow: public AdopterWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    enum {SchemeDefault, SchemeDay, SchemeNight, SchemeSand};
    enum {OrientationPortrait, OrientationLandscape};

signals:

public slots:
    void onSliderValueChanged(int value);
    void onCurrentFontChanged(const QFont &);
    void onSchemeButtonClicked(int id);
    void onOrientationButtonClicked(int id);
    void onLightsToggled(bool enable);
    void onGrabVolumeToggled(bool enable);

protected:
    void closeEvent(QCloseEvent *e);

private:
    QSlider *zoomSlider;
    QLabel *zoomLabel;
    QFontComboBox *fontButton;
};

#endif // SETTINGSWINDOW_H
