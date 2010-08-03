#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QIcon>
#include <QMetaType>
#include <QObject>

/** A book. */
class Book: public QObject
{
    Q_OBJECT

public:

    /** Content item in the table of contents. */
    struct ContentItem
    {
        QString href;
        QString name;
    };

    /** Bookmark: a volume index and a relative position in volume. */
    struct Bookmark
    {
        Bookmark(int chapter_, qreal pos_): chapter(chapter_), pos(pos_) {}
        Bookmark() {chapter = pos = 0;}
        int chapter;
        qreal pos;
        bool operator<(const Bookmark&other) const {
            if (chapter != other.chapter) {
                return chapter < other.chapter;
            } else {
                return pos < other.pos;
            }
        }
    };

    /** Construct a book from an EPUB file in path. */
    Book(const QString &path, QObject *parent = 0);

    /** Default constructor. */
    Book();

    /** Load book from persistent storage. */
    void load();

    /** Save book to persistent storage. */
    void save();

    /** Extract and parse EPUB contents, fill in all members except mPath. */
    bool open();

    /** Clear toc and content members, remove extracted content files. */
    void close();

    /** Return path to EPUB. */
    QString path() const;

    /**
     * Return path to root directory of extracted EPUB.
     * Only valid after parsing Book::opsPath().
     */
    QString rootPath() const;

    /** Return temporary directory path for extracting EPUB file. */
    QString tmpDir() const;

    /** Clear directory. */
    bool clearDir(const QString &directory);

    /** Set last bookmark. */
    void setLastBookmark(int chapter, qreal position);

    /** Get last bookmark. */
    Bookmark lastBookmark() const;

    /** Add bookmark. */
    void addBookmark(int chapter, qreal position);

    /** Delete bookmark. */
    void deleteBookmark(int index);

    /** List bookmarks. */
    QList<Bookmark> bookmarks() const;

    /**
     * Get friendly name.
     * @return @see title or path name combined with author(s) name.
     */
    QString name() const;

    /** Get short friendly name: title or file name. */
    QString shortName() const;

    QString title;                          //< Book title from EPUB.
    QStringList toc;                        //< Table of contents from EPUB.
    QHash<QString, ContentItem> content;    //< Content items from EPUB.
    QImage cover;                           //< Cover image.
    QStringList creators;                   //< Creators.
    QString date;                           //< Date of creation.
    QString publisher;                      //< Publisher.
    QString datePublished;                  //< Date of publishing.
    QString subject;                        //< Subject.
    QString source;                         //< Source.
    QString rights;                         //< Rights.
    QString tocPath;                        //< Path to toc ncx.
    QString coverPath;                      //< Path to cover html.

signals:
    /** Emitted if @see open() succeeds. */
    void opened(const QString &bookPath);

protected:
    /** Extract EPUB as ZIP. */
    bool extract();

    /** Parse exteacted EPUB. */
    bool parse();

    /** Clear all book fields except path. */
    void clear();

    /** Get location of OPS file in EPUB archive. */
    QString opsPath();

    QString mPath;                          //< Path to EPUB file.
    Bookmark mLastBookmark;                 //< Last position read.
    QList<Bookmark> mBookmarks;             //< List of bookmarks.
    QString mRootPath;                      //< Path to root item in EPUB dir.
};

#endif // BOOK_H
