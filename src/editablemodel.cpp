#include "editablemodel.hpp"

#include "query.hpp"

#include <QSqlError>
#include <QSqlQuery>
#include <QString>

/* explicit */ EditableModel::EditableModel(const QSqlDatabase& database /* = QSqlDatabase::addDatabase("QSQLITE") */,
                                            QObject* parent /* = nullptr */)
: QAbstractTableModel(parent),
  _database(database),
  _displayed_headers({"Название", "Автор", "Приоритет", "Наличие", "Жанры"})
{
    loadData();
}

EditableModel::~EditableModel() /* override */ = default;

/* [[nodiscard]] */ int EditableModel::rowCount(const QModelIndex& parent /* = QModelIndex() */) const /* override  */
{
    Q_UNUSED(parent);
    return static_cast<int>(_data.size());
}

/* [[nodiscard]] */ int EditableModel::columnCount(const QModelIndex& parent /* = QModelIndex() */) const /* override  */
{
    Q_UNUSED(parent);
    return static_cast<int>(_displayed_headers.size()); // columns in header
}

/* [[nodiscard]] */ QVariant EditableModel::data(const QModelIndex& index, int role) const /* override  */
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return {};
    }

    return _data[index.row()][index.column()];
}

/* [[nodiscard]] */ QVariant EditableModel::headerData(int section, Qt::Orientation orientation, int role) const /* override  */
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return _displayed_headers[section];
    }

    return {};
}

/* [[nodiscard]] */ Qt::ItemFlags EditableModel::flags(const QModelIndex& index) const /* override  */
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/* [[nodiscard]] */ bool EditableModel::setData(const QModelIndex& index [[maybe_unused]],
                                                const QVariant& value [[maybe_unused]],
                                                int role [[maybe_unused]]) /* override  */
{
    return false; // prevents editing of values
                  // TODO check this
}

void EditableModel::loadData()
{
    // clear existance data
    _data.clear();
    _ids.clear();

    QSqlQuery query;

    // getting all the data from the tables, combining them into one table
    auto res = query.exec(QString::fromStdString(std::string(BooksDatabase::GET_ALL)));

    if (!res) {
        throw std::runtime_error("query failed");
    }

    // process result line by line, add rows to data vector
    while (query.next()) {
        _ids.push_back(query.value(0).toInt());
        QVector<QVariant> row;
        row.reserve(BooksDatabase::COLUMNS_IN_JOINED - 1);
        for (int i = 0; i < BooksDatabase::COLUMNS_IN_JOINED - 1; ++i) {
            row.emplace_back(query.value(i));
        }

        _data.push_back(std::move(row));
    }
}
