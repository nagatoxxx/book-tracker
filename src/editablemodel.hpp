#pragma once

#include <QAbstractTableModel>
#include <QSqlDatabase>

// TODO rename
//
// implementation of model with books, provides custom columns names
class EditableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EditableModel(const QSqlDatabase& database = QSqlDatabase::addDatabase("QSQLITE"), QObject* parent = nullptr);

    EditableModel(const EditableModel&) = delete;
    EditableModel(EditableModel&&) = delete;

    EditableModel& operator=(const EditableModel&) = delete;
    EditableModel& operator=(EditableModel&&) = delete;

    ~EditableModel() override;

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    [[nodiscard]] bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    void loadData();

private:
    QSqlDatabase _database;
    QStringList _displayed_headers;
    QVector<QVector<QVariant>> _data;
    QVector<int> _ids; // id of rows retrieved during the last access to the database
};
