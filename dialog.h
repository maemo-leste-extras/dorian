#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

class QWidget;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

/** Maemo- and Mac-friendly dialog box. */
class Dialog: public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);

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

#endif // DIALOG_H
