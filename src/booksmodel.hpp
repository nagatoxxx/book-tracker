#pragma once

#include "defs.hpp"
#include "util.hpp"

#include <QAbstractTableModel>
#include <QSqlDatabase>

namespace bd = BooksDatabase;

// implementation of model with books, provides custom columns names,
// methods to add, delete, find books etc
class BooksModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BooksModel(const QString& filename, QObject* parent = nullptr);

    BooksModel(const BooksModel&) = delete;
    BooksModel(BooksModel&&) = delete;

    BooksModel& operator=(const BooksModel&) = delete;
    BooksModel& operator=(BooksModel&&) = delete;

    ~BooksModel() override;

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    // sql query wrappers
    void insertBook(const bd::Book& book);

    [[nodiscard]] std::vector<std::string> genres() const;
    [[nodiscard]] std::vector<std::string> priorities() const;
    [[nodiscard]] std::vector<std::string> avaibilities() const;
    [[nodiscard]] std::vector<std::string> authors() const;

private:
    void loadData(); // update _data with sql select query

    QSqlDatabase _database;
    QStringList _displayed_headers;
    QVector<QVector<QVariant>> _data;
    QVector<int> _ids; // id of rows retrieved during the last access to the database
};
