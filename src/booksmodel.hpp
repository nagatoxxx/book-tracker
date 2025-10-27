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
    static constexpr auto BOOK_TITLE_COLUMN{1};

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


    [[nodiscard]] bd::Book book(int book_id) const;
    void insertBook(const bd::Book& book);
    void updateBook(int book_id, const bd::Book& book);
    void deleteBook(std::string_view title);
    // void deleteBook(int book_id);
    void deleteBooks(const std::vector<std::string>& titles);

    [[nodiscard]] std::vector<std::string> genres() const;
    [[nodiscard]] std::vector<std::string> priorities() const;
    [[nodiscard]] std::vector<std::string> avaibilities() const;
    [[nodiscard]] std::vector<std::string> authors() const;

private:
    void loadData(); // update _data with sql select query

    int ensureAuthorExists(std::string_view author);
    int ensureAvaibilityExists(std::string_view avaibility);
    int ensurePriorityExists(std::string_view priority);
    int ensureGenreExists(std::string_view genre);
    void linkBookGenre(int book_id, int genre_id);

    QSqlDatabase _database;
    QStringList _displayed_headers;
    QVector<QVector<QVariant>> _data;
    QVector<int> _ids; // id of rows retrieved during the last access to the database
};
