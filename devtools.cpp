#include <QtGui>

#include "devtools.h"
#include "trace.h"
#include "settings.h"
#include "toolbuttonbox.h"
#include "bookdb.h"
#include "platform.h"

DevTools::DevTools(QWidget *parent): Dyalog(parent, false)
{
    setWindowTitle(tr("Developer"));
    QPushButton *clearSettings = new QPushButton("Clear persistent data", this);
    connect(clearSettings, SIGNAL(clicked()), this, SLOT(onClear()));
    addWidget(clearSettings);

    QLabel *level = new QLabel(tr("Trace level:"), this);
    addWidget(level);

    ToolButtonBox *box = new ToolButtonBox(this);
    addWidget(box);
    box->addButton(QtDebugMsg, tr("Debug"));
    box->addButton(QtWarningMsg, tr("Warning"));
    box->addButton(QtCriticalMsg, tr("Critical"));
    box->addButton(QtFatalMsg, tr("Fatal"));
    box->toggle(Trace::level);
    connect(box, SIGNAL(buttonClicked(int)),
            this, SLOT(onLevelButtonClicked(int)));

    QCheckBox *traceToFile = new QCheckBox(tr("Trace to file"), this);
    addWidget(traceToFile);
    traceToFile->setChecked(!Trace::fileName().isEmpty());
    connect(traceToFile, SIGNAL(toggled(bool)),
            this, SLOT(onTraceToFileToggled(bool)));

    addStretch();
}

void DevTools::onClear()
{
    if (QMessageBox::Yes ==
        QMessageBox::question(this, tr("Clear persistent data"),
                              tr("Library and settings will be cleared, "
                                 "application will be restarted. Continue?"),
                              QMessageBox::Yes | QMessageBox::No)) {
        QSettings().clear();
        BookDb::instance()->removeAll();
        QApplication::exit(1000);
    }
}

void DevTools::onLevelButtonClicked(int level)
{
    Trace::level = (QtMsgType)level;
    Settings::instance()->setValue("tracelevel", level);
}

void DevTools::onTraceToFileToggled(bool enable)
{
    QString name = enable? Platform::traceFileName(): QString();
    Trace::setFileName(name);
    Settings::instance()->setValue("tracefilename", name);
}
