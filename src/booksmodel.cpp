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
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
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
    QSqlQuery query(_database);

    query.prepare(utils::strviewToQString(BooksDatabase::INSERT_BOOK));
    query.bindValue(":title", QString::fromStdString(book.title));
    query.bindValue(":author", *book.author);
    query.bindValue(":avaibility", *book.avaibility);
    query.bindValue(":priority", *book.priority);

    if (!query.exec()) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
    }

    beginResetModel();
    loadData();
    endResetModel();

    qWarning() << "book inserted";
}

/* [[nodiscard]] */ std::vector<bd::Genre> BooksModel::genres() const
{
    QSqlQuery query(_database);

    auto res = query.exec(utils::strviewToQString(bd::GET_GENRES));

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
    }

    std::vector<bd::Genre> genres;

    while (query.next()) {
        auto genre_id = query.value(0).toInt();
        auto genre_name = query.value(1).toString().toStdString();

        genres.emplace_back(genre_id, genre_name);
    }

    return genres;
}

/* [[nodiscard]] */ std::vector<bd::Priority> BooksModel::priorities() const
{
    QSqlQuery query(_database);

    auto res = query.exec(utils::strviewToQString(bd::GET_PRIORITIES));

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
    }

    std::vector<bd::Priority> priorities;

    while (query.next()) {
        auto priority_id = query.value(0).toInt();
        auto priority_name = query.value(1).toString().toStdString();

        priorities.emplace_back(priority_id, priority_name);
    }

    return priorities;
}

/* [[nodiscard]] */ std::vector<bd::Avaibility> BooksModel::avaibilities() const
{
    QSqlQuery query(_database);

    auto res = query.exec(utils::strviewToQString(bd::GET_AVAIBILITIES));

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
    }

    std::vector<bd::Avaibility> avaibilities;

    while (query.next()) {
        auto avaibility_id = query.value(0).toInt();
        auto avaibility_name = query.value(1).toString().toStdString();

        avaibilities.emplace_back(avaibility_id, avaibility_name);
    }

    return avaibilities;
}

/* [[nodiscard]] */ std::vector<bd::Author> BooksModel::authors() const
{
    QSqlQuery query(_database);

    auto res = query.exec(utils::strviewToQString(bd::GET_AUTHORS));

    if (!res) {
        auto error = query.lastError();
        throw std::runtime_error(std::format("query failed: {}", error.text().toStdString()));
    }

    std::vector<bd::Author> authors;

    while (query.next()) {
        auto author_id = query.value(0).toInt();
        auto author_name = query.value(1).toString().toStdString();

        authors.emplace_back(author_id, author_name);
    }

    return authors;
}
