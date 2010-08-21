#include <QtGui>
#include <QStringListModel>
#include <QFileInfo>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "foldersdialog.h"
#include "library.h"
#include "settings.h"
#include "trace.h"
#include "bookfinder.h"

FoldersDialog::FoldersDialog(QWidget *parent): ListWindow(parent)
{
    setWindowTitle(tr("Folders"));
    model = new QStringListModel(Library::instance()->folders(), this);
    QListView *list = new QListView(this);
    list->setModel(model);
    list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list->setUniformItemSizes(true);
    addList(list);
    addAction(tr("Add folder"), this, SLOT(onAdd()));
    addItemAction(tr("Re-scan"), this, SLOT(onRefresh()));
    addItemAction(tr("Remove"), this, SLOT(onRemove()));
    addAction(tr("Re-scan all folders"), this, SLOT(onRefreshAll()));
    progress = new QProgressDialog(tr("Scanning for books"), "", 0, 0, this);
    progress->reset();
    progress->setMinimumDuration(0);
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(0);
}

void FoldersDialog::onAdd()
{
    Trace t("FoldersDialog::onAdd");

    // Get folder name
    Settings *settings = Settings::instance();
    QString last =
            settings->value("lastfolderadded", QDir::homePath()).toString();
    QString path =
            QFileDialog::getExistingDirectory(this, tr("Add Folder"), last);
    if (path == "") {
        return;
    }
    settings->setValue("lastfolderadded", QFileInfo(path).absolutePath());
    t.trace(path);

    // Add folder to model
    if (Library::instance()->addFolder(path)) {
        int rows = model->rowCount();
        model->insertRows(rows, 1);
        model->setData(model->index(rows), path);
        refresh(QStringList(path));
    } else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
            tr("This folder is already in the library"));
#else
        (void)QMessageBox::information(this, tr("Dorian"),
            tr("This folder is already in the library"), QMessageBox::Ok);
#endif // Q_WS_MAEMO_5
    }
}

void FoldersDialog::onRemove()
{
    Trace t("FoldersDialog::onRemove");

    QModelIndexList selection = list->selectionModel()->selectedIndexes();
    if (selection.size() != 1) {
        return;
    }

    QModelIndex selected = selection[0];
    QString path = list->model()->data(selected).toString();
    t.trace(path);

    if (QMessageBox::Yes ==
        QMessageBox::question(this, tr("Remove folder"),
            tr("Remove folder \"%1\" from library?").arg(path),
            QMessageBox::Yes | QMessageBox::No)) {
        if (Library::instance()->removeFolder(path)) {
            model->removeRow(selected.row());
        }
    }
}

void FoldersDialog::refresh(const QStringList &dirs)
{
    Trace t("FoldersDialog::refresh");

    progress->setWindowTitle((dirs.length() > 1)?
                             tr("Scanning all folders"): tr("Scanning folder"));
    BookFinder *bookFinder = new BookFinder(this);
    Library *library = Library::instance();
    connect(bookFinder, SIGNAL(beginAdd(int)), progress, SLOT(setMaximum(int)));
    connect(bookFinder, SIGNAL(add(const QString &)),
            this, SLOT(onAddBook(const QString &)));
    connect(bookFinder, SIGNAL(add(const QString &)),
            library, SLOT(add(const QString &)));
    connect(bookFinder, SIGNAL(remove(const QString &)),
            library, SLOT(remove(const QString &)));
    connect(bookFinder, SIGNAL(done(int,int)),
            this, SLOT(onRefreshDone(int, int)));
    bookFinder->find(dirs, library->bookPaths());
}

void FoldersDialog::onRefresh()
{
    Trace t("FoldersDialog::onRefresh");

    QModelIndexList selection = list->selectionModel()->selectedIndexes();
    if (selection.size() != 1) {
        return;
    }
    QModelIndex selected = selection[0];
    QString path = list->model()->data(selected).toString();
    refresh(QStringList(path));
}

void FoldersDialog::onRefreshAll()
{
    refresh(model->stringList());
}

void FoldersDialog::onRefreshDone(int added, int removed)
{
    QString addedMsg;
    QString removedMsg;

    switch (added) {
    case 0: addedMsg = tr("No books added"); break;
    case 1: addedMsg = tr("%1 book added").arg(1); break;
    default: addedMsg = tr("%1 books added").arg(added);
    }

    switch (removed) {
    case 0: removedMsg = tr("no books removed"); break;
    case 1: removedMsg = tr("%1 book removed").arg(1); break;
    default: removedMsg = tr("%1 books removed").arg(removed);
    }

    progress->reset();
    QString msg(tr("Scanning complete\n\n%1, %2.").
                arg(addedMsg).arg(removedMsg));
    Trace::trace(QString("FoldersDialog::onRefreshDone: " + msg));
#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::information(this, msg);
#else
    // FIXME
#endif
}

void FoldersDialog::onAddBook(const QString &path)
{
    progress->setLabelText(QFileInfo(path).fileName());
    progress->setValue(progress->value() + 1);
}
