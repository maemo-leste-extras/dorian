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
    addItemAction(tr("Delete folder"), this, SLOT(onRemove()));
    addAction(tr("Re-scan folders"), this, SLOT(onRefresh()));
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
        onRefresh();
    } else {
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
            tr("This folder is already in the library"),
            QMaemo5InformationBox::DefaultTimeout);
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
    if (Library::instance()->removeFolder(path)) {
        model->removeRow(selected.row());
        onRefresh();
    }
}

void FoldersDialog::onRefresh()
{
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif

    BookFinder *bookFinder = new BookFinder(this);
    Library *library = Library::instance();
    connect(bookFinder, SIGNAL(add(const QString &)),
            library, SLOT(add(const QString &)));
    connect(bookFinder, SIGNAL(remove(const QString &)),
            library, SLOT(remove(const QString &)));
    connect(bookFinder, SIGNAL(done(int,int)),
            this, SLOT(onRefreshDone(int, int)));
    // bookFinder->moveToThread(&bookFinderThread);
    // bookFinderThread.start();

    (void)QMetaObject::invokeMethod(
        bookFinder,
        "find",
        Q_ARG(QStringList, model->stringList()),
        Q_ARG(QStringList, library->bookPaths()));

#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
#endif
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

    QString msg(tr("Scanning folders complete\n\n%1, %2.").
                arg(addedMsg).arg(removedMsg));
    Trace::trace(QString("FoldersDialog::onRefreshDone: " + msg));
#ifdef Q_WS_MAEMO_5
    QMaemo5InformationBox::
            information(this, msg, QMaemo5InformationBox::NoTimeout);
#else
    // FIXME
#endif
}
