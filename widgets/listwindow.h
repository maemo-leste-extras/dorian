#ifndef LISTWINDOW_H
#define LISTWINDOW_H

#include <QMainWindow>
#include <QDialogButtonBox>

class QListView;
class QString;
class QHBoxLayout;

/** A window with a list and menu actions (Maemo) or buttons (non-Maemo). */
class ListWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit ListWindow(QWidget *parent = 0);
    void addList(QListView *list);
    void addAction(const QString &title, QObject *receiver, const char *slot,
        QDialogButtonBox::ButtonRole role = QDialogButtonBox::ActionRole);

protected:
#ifdef Q_WS_MAEMO_5
    void closeEvent(QCloseEvent *event);
#else
    QDialogButtonBox *buttonBox;
#endif
    QHBoxLayout *layout;
};

#endif // LISTWINDOW_H
