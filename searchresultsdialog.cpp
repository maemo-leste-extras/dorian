#include <stdio.h>

#include <QtGui>
#include <QStringListModel>
#include <QDir>
#include <QFile>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#endif

#include "listview.h"
#include "searchresultsdialog.h"
#include "searchresultinfodialog.h"
#include "trace.h"
#include "progressdialog.h"
#include "library.h"
#include "platform.h"

SearchResultsDialog::SearchResultsDialog(const QList<Search::Result> results_,
    QWidget *parent): ListWindow(parent), results(results_)
{
    setWindowTitle(tr("Search Results"));

    foreach (Search::Result result, results) {
        QString author;
        if (result.authors.length()) {
            author = result.authors[0];
        }
        data.append(result.title + "\n" + author);
    }

    QStringListModel *model = new QStringListModel(data, this);
    list = new ListView;
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setModel(model);
    list->setUniformItemSizes(true);
    addList(list);
    addItemAction(tr("Download book"), this, SLOT(onDownload()));
    connect(list, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
    Search *search = Search::instance();
    connect(search, SIGNAL(beginDownload(int)), this, SLOT(onBeginDownload(int)));
    connect(search,
            SIGNAL(endDownload(int, const Search::Result &, const QString &)),
            this,
            SLOT(onEndDownload(int, const Search::Result &, const QString &)));

    progress = new ProgressDialog(tr("Downloading Book"), this);
}

void SearchResultsDialog::onItemActivated(const QModelIndex &index)
{
    TRACE;
    Search::Result result = results[index.row()];
    qDebug() << "Book" << index.row() << ":" << result.title;
    SearchResultInfoDialog *d = new SearchResultInfoDialog(result, this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    int ret = d->exec();
    if (ret == QDialog::Accepted) {
        qDebug() << "Accepted -> Start download";
        QString fileName = downloadName();
        qDebug() << "Downloading to" << fileName;
        Search::instance()->download(result, fileName);
    }
}

void SearchResultsDialog::onDownload()
{
    onItemActivated(list->currentIndex());
}

QString SearchResultsDialog::downloadName() const
{
    TRACE;
    QString dir = Platform::downloadDir();
    QDir().mkpath(dir); // Not sure if this works. QDir API is quiet lame.
    unsigned i = 0;
    QString fileName;
    do {
        char tmp[9];
        snprintf(tmp, 8, "%8.8x", i++);
        tmp[8] = '\0';
        fileName = QDir(dir).absoluteFilePath(QString(tmp) + ".epub");
    } while (QFile(fileName).exists());
    qDebug() << fileName;
    return fileName;
}

void SearchResultsDialog::onBeginDownload(int size)
{
    Q_UNUSED(size);
    TRACE;
    progress->showWait();
}

void SearchResultsDialog::onEndDownload(int status, const Search::Result &result,
                                        const QString &fileName)
{
    Q_UNUSED(result);
    TRACE;
    progress->reset();
    if (Search::Ok == status) {
        Library::instance()->add(fileName);
        int row = results.indexOf(result);
        if (-1 != row) {
            list->model()->removeRow(row);
        }
#ifdef Q_WS_MAEMO_5
        QMaemo5InformationBox::information(this,
            tr("Downloaded \"%1\"\nand added to the library").arg(result.title),
            QMaemo5InformationBox::DefaultTimeout);
#endif
    }
}
