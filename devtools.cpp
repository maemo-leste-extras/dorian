#include <QtGui>
#include <QDebug>

#include "devtools.h"
#include "trace.h"
#include "settings.h"

DevTools::DevTools(QWidget *parent):
        QDialog(parent, Qt::Dialog | Qt::WindowTitleHint |
                Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle(tr("Developer"));
    QScrollArea *scroller = new QScrollArea(this);
    scroller->setFrameStyle(QFrame::NoFrame);
#ifdef Q_WS_MAEMO_5
    scroller->setProperty("FingerScrollable", true);
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
    scroller->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setSizeGripEnabled(true);
#endif // Q_WS_MAEMO_5
    scroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *contents = new QWidget(scroller);
    QVBoxLayout *layout = new QVBoxLayout(contents);

    QPushButton *clearSettings = new QPushButton("Clear persistent data", this);
    connect(clearSettings, SIGNAL(clicked()), this, SLOT(onClear()));
    layout->addWidget(clearSettings);

    QLabel *level = new QLabel(tr("Trace level:"), contents);
    layout->addWidget(level);

    QFrame *box = new QFrame(this);
    layout->addWidget(box);
    QHBoxLayout *boxLayout = new QHBoxLayout(box);
    boxLayout->setMargin(0);
    box->setLayout(boxLayout);
    QButtonGroup *group = new QButtonGroup(this);
    group->setExclusive(true);

    QToolButton *levelDebug = new QToolButton(box);
    levelDebug->setToolButtonStyle(Qt::ToolButtonTextOnly);
    levelDebug->setText(tr("Debug"));
    levelDebug->setCheckable(true);
    boxLayout->addWidget(levelDebug);
    group->addButton(levelDebug, QtDebugMsg);

    QToolButton *levelWarning = new QToolButton(box);
    levelWarning->setToolButtonStyle(Qt::ToolButtonTextOnly);
    levelWarning->setText(tr("Warning"));
    levelWarning->setCheckable(true);
    boxLayout->addWidget(levelWarning);
    group->addButton(levelWarning, QtWarningMsg);

    QToolButton *levelCritical = new QToolButton(box);
    levelCritical->setToolButtonStyle(Qt::ToolButtonTextOnly);
    levelCritical->setText(tr("Critical"));
    levelCritical->setCheckable(true);
    boxLayout->addWidget(levelCritical);
    group->addButton(levelCritical, QtCriticalMsg);

    QToolButton *levelFatal = new QToolButton(box);
    levelFatal->setToolButtonStyle(Qt::ToolButtonTextOnly);
    levelFatal->setText(tr("Fatal"));
    levelFatal->setCheckable(true);
    boxLayout->addWidget(levelFatal);
    group->addButton(levelFatal, QtFatalMsg);

    if (Trace::level == QtFatalMsg) {
        levelFatal->toggle();
    } else if (Trace::level == QtCriticalMsg) {
        levelCritical->toggle();
    } else if (Trace::level == QtWarningMsg) {
        levelWarning->toggle();
    } else {
        levelDebug->toggle();
    }

    connect(group, SIGNAL(buttonClicked(int)),
            this, SLOT(onLevelButtonClicked(int)));
    layout->addWidget(box);

    contents->setLayout(layout);
    scroller->setWidget(contents);

    QHBoxLayout *dialogLayout = new QHBoxLayout();
    dialogLayout->addWidget(scroller);
    setLayout(dialogLayout);
}

void DevTools::onClear()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "Clear persistent data?",
                              "Library and settings data will be cleared, "
                              "application will be restarted. Continue?",
                              QMessageBox::Yes
#ifndef Q_WS_MAEMO_5
                              , QMessageBox::No
#endif
                              )) {
        QSettings().clear();
        QApplication::exit(1000);
    }
}

void DevTools::onLevelButtonClicked(int level) {
    Trace::level = (QtMsgType)level;
    Settings::instance()->setValue("tracelevel", level);
}
