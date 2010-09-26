#ifndef DYALOG_H
#define DYALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class QWidget;
class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QMenu;

/** Maemo- and Mac-friendly dialog box. */
class Dyalog: public QDialog
{
    Q_OBJECT

public:
    explicit Dyalog(QWidget *parent, bool showButtons = true);

    /** Add widget to the scrollable content pane. */
    void addWidget(QWidget *widget);

    /** Add stretch to the scrollable content pane. */
    void addStretch(int stretch = 0);

    /** Add button to the dialog button box. */
    void addButton(const QString &label, QObject *receiver, const char *slot,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::AcceptRole);

public slots:
#ifdef Q_OS_SYMBIAN
    void show();
    int exec();
#endif

protected:
    QScrollArea *scroller;
    QWidget *content;
    QVBoxLayout *contentLayout;
    QDialogButtonBox *buttonBox;
    bool showButtons;
#ifdef Q_OS_SYMBIAN
    QMenu *menu;
#endif
};

#endif // DYALOG_H
