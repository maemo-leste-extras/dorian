#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QIcon>

/** A book. */
class Book
{
public:

    /** Content item in the table of contents. */
    struct ContentItem
    {
        QString href;
        QString type;
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
    explicit Book(const QString &path);

    /** Load book from persistent storage. */
    void load();

    /** Save book to persistent storage. */
    void save();

    /** Extract and parse EPUB contents, fill in all members except mPath. */
    void open();

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

    /** List bookmarks. */
    QList<Bookmark> bookmarks() const;

    QString title;                          //< Book title from EPUB.
    QStringList toc;                        //< Table of contents from EPUB.
    QHash<QString, ContentItem> content;    //< Content items from EPUB.
    QIcon cover;                            //< Cover image.
    QStringList creators;                   //< Creators.
    QString date;                           //< Date of creation.
    QString publisher;                      //< Publisher.
    QString datePublished;                  //< Date of publishing.
    QString subject;                        //< Subject.
    QString source;                         //< Source.
    QString rights;                         //< Rights.

protected:
    /** Indicate failure by creating a single "error" content item. */
    void fail(const QString &details,
              const QString &error = QString("Could not open book"));

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
