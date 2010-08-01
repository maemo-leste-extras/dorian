#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>

class QPushButton;
class QFontComboBox;
class QFont;
class QSlider;
class QFontComboBox;
class QButtonGroup;

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

protected:
#ifdef Q_WS_MAEMO_5
    void closeEvent(QCloseEvent *e);
#endif

private:
    QSlider *zoomSlider;
    QFontComboBox *fontButton;
    QButtonGroup *orientationGroup;
};

#endif // SETTINGSWINDOW_H
