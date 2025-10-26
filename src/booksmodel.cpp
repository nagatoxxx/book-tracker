#include "booksmodel.hpp"

#include "query.hpp"
#include "util.hpp"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>

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
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    return _data[index.row()][index.column()];
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
                  // TODO check this
}

void BooksModel::loadData()
{
    beginResetModel();

    // clear existance data
    _data.clear();
    _ids.clear();

    QSqlQuery query(_database);

    // getting all the data from the tables, combining them into one table
    auto res = query.exec(QString::fromStdString(std::string(BooksDatabase::GET_ALL)));

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("select query failed: {}", error.text().toStdString()));
    }

    // process result line by line, add rows to data vector
    while (query.next()) {
        _ids.emplace_back(query.value(0).toInt());
        QVector<QVariant> row;
        row.reserve(BooksDatabase::COLUMNS_IN_JOINED - 1);
        for (int i = 0; i < BooksDatabase::COLUMNS_IN_JOINED - 1; ++i) {
            row.emplace_back(query.value(i));
        }

        _data.emplace_back(std::move(row));
    }

    endResetModel();
}

void BooksModel::insertBook(const bd::Book& book)
{
    namespace bd = BooksDatabase;

    QSqlQuery query(_database);

    // insert author if non-exists
    query.prepare(utils::strviewToQString(bd::INSERT_AUTHOR_NE));
    query.bindValue(":a", QString::fromStdString(book.author));
    query.exec();

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    // insert avaibility if non-exists
    query.prepare(utils::strviewToQString(bd::INSERT_AVAIBILITY_NE));
    query.bindValue(":v", QString::fromStdString(book.avaibility));
    query.exec();

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    // insert priority if non-exists
    query.prepare(utils::strviewToQString(bd::INSERT_PRIORITY_NE));
    query.bindValue(":p", QString::fromStdString(book.priority));
    query.exec();

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }
    // insert book if non-exists
    query.prepare(utils::strviewToQString(BooksDatabase::INSERT_BOOK));
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", QString::fromStdString(book.author));
    query.bindValue(":avaibility", QString::fromStdString(book.avaibility));
    query.bindValue(":priority", QString::fromStdString(book.priority));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("insert query failed: {}", error.text().toStdString()));
    }

    auto book_id = query.lastInsertId().toInt();

    for (const std::string& genre : book.genres) {
        // insert genre if non-exists
        query.prepare(utils::strviewToQString(bd::INSERT_GENRE_NE));
        query.bindValue(":g", QString::fromStdString(genre));
        query.exec();

        // found genre id
        query.prepare("SELECT genre_id FROM Genres WHERE genre_name = :g LIMIT 1;");
        query.bindValue(":g", QString::fromStdString(genre));
        query.exec();

        qint64 genre_id = -1;
        if (query.next()) {
            genre_id = query.value(0).toLongLong();
        }

        // create reference if non-exists
        query.prepare(utils::strviewToQString(bd::INSERT_BOOK_GENRE_NE));
        query.bindValue(":b", book_id);
        query.bindValue(":g", genre_id);
        query.exec();
    }

    loadData();

    qDebug() << "book" << book.title << "inserted";
}

void BooksModel::deleteBook(std::string_view title)
{
    namespace bd = BooksDatabase;

    QSqlQuery query(_database);

    query.prepare(utils::strviewToQString(bd::DELETE_BOOK_BY_TITLE));
    query.bindValue(":title", utils::strviewToQString(title));

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("delete book query failed: {}", error.text().toStdString()));
    }

    qDebug() << "book" << title << "deleted";

    loadData();
}

void BooksModel::deleteBooks(const std::vector<std::string_view>& titles)
{
    for (const auto& title : titles) {
        deleteBook(title);
    }
}

/* [[nodiscard]] */ std::vector<std::string> BooksModel::genres() const
{
    QSqlQuery query(_database);

    auto res = query.exec(utils::strviewToQString(bd::GET_GENRES));

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

    auto res = query.exec(utils::strviewToQString(bd::GET_PRIORITIES));

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

    auto res = query.exec(utils::strviewToQString(bd::GET_AVAIBILITIES));

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

    auto res = query.exec(utils::strviewToQString(bd::GET_AUTHORS));

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
