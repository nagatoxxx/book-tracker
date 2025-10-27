#include "booksmodel.hpp"

#include "util.hpp"

#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <qnamespace.h>

/* explicit */ BooksModel::BooksModel(const QString& filename, QObject* parent /* = nullptr */)
: QAbstractTableModel(parent),
  _database(QSqlDatabase::addDatabase(utils::strviewToQString(BooksDatabase::QT_DRIVER))),
  _displayed_headers({"Название", "Автор", "Приоритет", "Наличие", "Жанры"})
{
    // initialize database connection
    _database.setDatabaseName(filename);
    if (!_database.open()) {
        throw std::runtime_error("cannot open database");
    }

    // initial data load
    loadData();
}

BooksModel::~BooksModel() /* override */
{
    _database.close();
}

/* [[nodiscard]] */ int BooksModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const /* override  */
{
    Q_UNUSED(parent);
    return static_cast<int>(_data.size());
}

/* [[nodiscard]] */ int BooksModel::columnCount(const QModelIndex& parent /* = QModelIndex() */) const /* override  */
{
    Q_UNUSED(parent);
    return static_cast<int>(_displayed_headers.size()); // columns in header
}

/* [[nodiscard]] */ QVariant BooksModel::data(const QModelIndex& index, int role) const /* override  */
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        return _data[index.row()][index.column()];
    }

    if (role == Qt::UserRole) {
        return _ids[index.row()];
    }

    return {};
}

/* [[nodiscard]] */ QVariant BooksModel::headerData(int section, Qt::Orientation orientation, int role) const /* override  */
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return _displayed_headers[section];
    }

    return {};
}

/* [[nodiscard]] */ Qt::ItemFlags BooksModel::flags(const QModelIndex& index) const /* override  */
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/* [[nodiscard]] */ bool BooksModel::setData(const QModelIndex& index [[maybe_unused]],
                                             const QVariant& value [[maybe_unused]],
                                             int role [[maybe_unused]]) /* override  */
{
    return false; // prevents editing of values
}

void BooksModel::loadData()
{
    beginResetModel();

    // clear existance data
    _data.clear();
    _ids.clear();

    QSqlQuery query(_database);

    // getting all the data from the tables, combining them into one table
    auto res = query.exec(R"(
    SELECT 
    Books.book_id as id,
    Books.book_title as title,
    Authors.author_name as author,
    Priorities.priority_name as priority, 
    Avaibilities.avaibility_name as avaibility, 
    GROUP_CONCAT(Genres.genre_name) as genres
    FROM Books
    FULL OUTER JOIN BooksGenres on BooksGenres.book_id = Books.book_id
    JOIN Authors on Books.book_author = Authors.author_id
    JOIN Priorities on Books.book_priority = Priorities.priority_id
    JOIN Avaibilities on Books.book_avaibility = Avaibilities.avaibility_id
    LEFT JOIN Genres on BooksGenres.genre_id = Genres.genre_id
    GROUP BY (title);
    )");


    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("select query failed: {}", error.text().toStdString()));
    }

    // process result line by line, add rows to data vector
    while (query.next()) {
        _ids.emplace_back(query.value(0).toInt());
        QVector<QVariant> row;
        row.reserve(BooksDatabase::COLUMNS_IN_JOINED);
        for (int i = 1; i < BooksDatabase::COLUMNS_IN_JOINED; ++i) {
            row.emplace_back(query.value(i));
        }

        _data.emplace_back(std::move(row));
    }

    qWarning() << "ids:" << _ids;

    endResetModel();
}

/* [[nodiscard]] */ bd::Book BooksModel::book(int book_id) const
{
    QSqlQuery query(_database);

    query.prepare(R"(
    SELECT 
    Books.book_id as id,
    Books.book_title as title,
    Authors.author_name as author,
    Priorities.priority_name as priority, 
    Avaibilities.avaibility_name as avaibility, 
    GROUP_CONCAT(Genres.genre_name) as genres
    FROM Books
    FULL OUTER JOIN BooksGenres on BooksGenres.book_id = Books.book_id
    JOIN Authors on Books.book_author = Authors.author_id
    JOIN Priorities on Books.book_priority = Priorities.priority_id
    JOIN Avaibilities on Books.book_avaibility = Avaibilities.avaibility_id
    LEFT JOIN Genres on BooksGenres.genre_id = Genres.genre_id
    WHERE Books.book_id = :id
    GROUP BY (title);
    )");
    query.bindValue(":id", book_id);

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("select query failed: {}", error.text().toStdString()));
    }

    if (!query.next()) {
        throw std::runtime_error("cannot find book");
    }

    bd::Book book;
    book.title = query.value(1).toString().toStdString();
    book.author = query.value(2).toString().toStdString();
    book.priority = query.value(3).toString().toStdString();
    book.avaibility = query.value(4).toString().toStdString();

    auto genres = query.value(5).toString().toStdString();
    qDebug() << "sql genres:" << genres.c_str();
    boost::char_separator<char> separator(", ;");
    boost::tokenizer<boost::char_separator<char>> tokenizer(genres, separator);

    std::vector<std::string> separated_genres;
    for (const auto& genre : tokenizer) {
        separated_genres.push_back(genre);
    }

    book.genres = std::move(separated_genres);

    return book;
}

void BooksModel::insertBook(const bd::Book& book)
{
    auto author_id = ensureAuthorExists(book.author);
    auto avaibility_id = ensureAvaibilityExists(book.avaibility);
    auto priority_id = ensurePriorityExists(book.priority);

    QSqlQuery query(_database);

    query.prepare(R"(
    INSERT INTO Books (book_title, book_author, book_avaibility, book_priority)
    VALUES (:title, :author, :avaibility, :priority);)");
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", author_id);
    query.bindValue(":avaibility", avaibility_id);
    query.bindValue(":priority", priority_id);

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    auto book_id = query.lastInsertId().toInt();

    for (const std::string& genre : book.genres) {
        int genre_id = ensureGenreExists(genre);
        linkBookGenre(book_id, genre_id);
    }

    loadData();

    qDebug() << "book" << book.title << "inserted";
}

void BooksModel::updateBook(int book_id, const bd::Book& book)
{
    int author_id = ensureAuthorExists(book.author);
    int availability_id = ensureAvaibilityExists(book.avaibility);
    int priority_id = ensurePriorityExists(book.priority);

    QSqlQuery query(_database);
    query.prepare(R"(UPDATE Books
    SET
        book_title = :title,
        book_author = :author,
        book_avaibility = :avaibility,
        book_priority = :priority
    WHERE book_id = :id;)");
    query.bindValue(":id", book_id);
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", author_id);
    query.bindValue(":avaibility", availability_id);
    query.bindValue(":priority", priority_id);

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("update book failed: {}", error.text().toStdString()));
    }

    query.prepare("DELETE FROM BooksGenres WHERE book_id = :b;");
    query.bindValue(":b", book_id);

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("delete query failed: {}", error.text().toStdString()));
    }

    for (const std::string& genre : book.genres) {
        qDebug() << "genre:" << genre.c_str();
        int genre_id = ensureGenreExists(genre);
        qDebug() << "genre id" << genre_id;
        linkBookGenre(book_id, genre_id);
    }

    loadData();

    qDebug() << "book updated:" << QString::fromStdString(book.title);
}

void BooksModel::deleteBook(std::string_view title)
{
    QSqlQuery query(_database);

    query.prepare(R"(DELETE FROM BooksGenres WHERE book_id = (SELECT book_id FROM Books WHERE book_title=:title);)");
    query.bindValue(":title", utils::strviewToQString(title));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("delete book genres query failed: {}", error.text().toStdString()));
    }

    query.prepare(R"(
    DELETE FROM Books WHERE book_title = :title;
    )");
    query.bindValue(":title", utils::strviewToQString(title));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("delete book query failed: {}", error.text().toStdString()));
    }


    qDebug() << "book" << title << "deleted";

    loadData();
}

void BooksModel::deleteBooks(const std::vector<std::string>& titles)
{
    for (const auto& title : titles) {
        deleteBook(title);
    }
}

// void BooksModel::deleteBook(int book_id)
// {
//     QSqlQuery query(_database);
//
//     query.prepare(R"(
//     DELETE FROM Books WHERE book_id = :id;
//     )");
//     query.bindValue(":id", book_id);
//
//     if (!query.exec()) {
//         auto error = query.lastError();
//         throw std::runtime_error(std::format("delete book query failed: {}", error.text().toStdString()));
//     }
//
//     qDebug() << "book with id" << book_id << "deleted";
//
//     loadData();
// }

/* [[nodiscard]] */ std::vector<std::string> BooksModel::genres() const
{
    QSqlQuery query(_database);

    auto res = query.exec(R"(
    SELECT
    Genres.genre_id as id,
    Genres.genre_name as name
    FROM Genres;
    )");

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("get genres query failed: {}", error.text().toStdString()));
    }

    std::vector<std::string> genres;

    while (query.next()) {
        auto genre_name = query.value(1).toString().toStdString();

        genres.emplace_back(genre_name);
    }

    return genres;
}

/* [[nodiscard]] */ std::vector<std::string> BooksModel::priorities() const
{
    QSqlQuery query(_database);

    auto res = query.exec(R"(
    SELECT 
    Priorities.priority_id as id,
    Priorities.priority_name as name,
    Priorities.priority_value as value
    FROM Priorities;
    )");

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("get priorities query failed: {}", error.text().toStdString()));
    }

    std::vector<std::string> priorities;

    while (query.next()) {
        auto priority_name = query.value(1).toString().toStdString();

        priorities.emplace_back(priority_name);
    }

    return priorities;
}

/* [[nodiscard]] */ std::vector<std::string> BooksModel::avaibilities() const
{
    QSqlQuery query(_database);

    auto res = query.exec(R"(
    SELECT
    Avaibilities.avaibility_id as id,
    Avaibilities.avaibility_name as name
    FROM Avaibilities;
    )");

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("get avaibilities query failed: {}", error.text().toStdString()));
    }

    std::vector<std::string> avaibilities;

    while (query.next()) {
        auto avaibility_name = query.value(1).toString().toStdString();

        avaibilities.emplace_back(avaibility_name);
    }

    return avaibilities;
}

/* [[nodiscard]] */ std::vector<std::string> BooksModel::authors() const
{
    QSqlQuery query(_database);

    auto res = query.exec(R"(
    SELECT 
    Authors.author_id as id,
    Authors.author_name as name
    FROM Authors;
    )");

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("get authors query failed: {}", error.text().toStdString()));
    }

    std::vector<std::string> authors;

    while (query.next()) {
        auto author_name = query.value(1).toString().toStdString();

        authors.emplace_back(std::move(author_name));
    }

    return authors;
}

int BooksModel::ensureAuthorExists(std::string_view author)
{
    QSqlQuery query(_database);

    // insert author if non-exists
    query.prepare(R"(
    INSERT INTO Authors (author_name) SELECT :a WHERE NOT EXISTS (SELECT 1 FROM Authors WHERE author_name = :a);
    )");
    query.bindValue(":a", utils::strviewToQString(author));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    query.prepare("SELECT author_id FROM Authors WHERE author_name = :a LIMIT 1;");
    query.bindValue(":a", utils::strviewToQString(author));
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }

    throw std::runtime_error("failed to ensure author exists");
}

int BooksModel::ensureAvaibilityExists(std::string_view avaibility)
{
    QSqlQuery query(_database);

    // insert avaibility if non-exists
    query.prepare(R"(
    INSERT INTO Avaibilities (avaibility_name) SELECT :v WHERE NOT EXISTS (SELECT 1 FROM Avaibilities WHERE avaibility_name = :v);
    )");
    query.bindValue(":v", utils::strviewToQString(avaibility));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    query.prepare("SELECT avaibility_id FROM Avaibilities WHERE avaibility_name = :v LIMIT 1;");
    query.bindValue(":v", utils::strviewToQString(avaibility));
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }

    throw std::runtime_error("failed to ensure avaibility exists");
}

int BooksModel::ensurePriorityExists(std::string_view priority)
{
    QSqlQuery query(_database);

    // insert priority if non-exists
    query.prepare(R"(
    INSERT INTO Priorities (priority_name) SELECT :p WHERE NOT EXISTS (SELECT 1 FROM Priorities WHERE priority_name = :p);
    )");
    query.bindValue(":p", utils::strviewToQString(priority));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    query.prepare("SELECT priority_id FROM Priorities WHERE priority_name = :p LIMIT 1;");
    query.bindValue(":p", utils::strviewToQString(priority));
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }

    throw std::runtime_error("failed to ensure priority exists");
}

int BooksModel::ensureGenreExists(std::string_view genre)
{
    QSqlQuery query(_database);

    query.prepare(R"(
    INSERT INTO Genres (genre_name) SELECT :g WHERE NOT EXISTS (SELECT 1 FROM Genres WHERE genre_name = :g);
    )");
    query.bindValue(":g", utils::strviewToQString(genre));
    query.exec();

    query.prepare("SELECT genre_id FROM Genres WHERE genre_name = :g LIMIT 1;");
    query.bindValue(":g", utils::strviewToQString(genre));
    query.exec();

    if (query.next()) {
        return query.value(0).toInt();
    }

    throw std::runtime_error("failed to ensure genre exists");
}

void BooksModel::linkBookGenre(int book_id, int genre_id)
{
    QSqlQuery query(_database);

    query.prepare(R"(
    INSERT INTO BooksGenres (book_id, genre_id)
    SELECT :b, :g
    WHERE NOT EXISTS (
        SELECT 1 FROM BooksGenres WHERE book_id = :b AND genre_id = :g
    );
    )");
    query.bindValue(":b", book_id);
    query.bindValue(":g", genre_id);
    query.exec();
}
