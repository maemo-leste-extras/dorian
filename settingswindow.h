#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSlider>

class QPushButton;
class QFontComboBox;
class QFont;
class QSlider;
class QFontComboBox;
class QLabel;
class ToolButtonBox;

/** Display settings. */
class SettingsWindow: public QMainWindow
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
#ifdef Q_WS_MAEMO_5
    void closeEvent(QCloseEvent *e);
#endif

private:
    QSlider *zoomSlider;
    QLabel *zoomLabel;
    QFontComboBox *fontButton;
    ToolButtonBox *orientationBox;
};

#endif // SETTINGSWINDOW_H
