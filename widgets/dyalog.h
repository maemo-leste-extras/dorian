#ifndef DYALOG_H
#define DYALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class QWidget;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

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
    void addButton(QPushButton *button,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::AcceptRole);

    /** Add standard button to the dialog button box. */
    QPushButton *addButton(QDialogButtonBox::StandardButton button);

protected:
    QScrollArea *scroller;
    QWidget *content;
    QVBoxLayout *contentLayout;
    QDialogButtonBox *buttonBox;
};

#endif // DYALOG_H
