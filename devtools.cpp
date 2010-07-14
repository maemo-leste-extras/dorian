#include <QtGui>

#include "devtools.h"

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
                              "application will be restarted. Continue?")) {
        QSettings().clear();
        QApplication::exit(1000);
    }
}
