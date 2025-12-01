#pragma once

#include <QSortFilterProxyModel>

class BooksProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BooksProxyModel(QObject* parent = nullptr);

    BooksProxyModel(const BooksProxyModel&) = delete;
    BooksProxyModel(BooksProxyModel&&) = delete;

    BooksProxyModel& operator=(const BooksProxyModel&) = delete;
    BooksProxyModel& operator=(BooksProxyModel&&) = delete;

    ~BooksProxyModel() override;

    [[nodiscard]] bool filterAcceptsRow(int source_row, const QModelIndex& source_idx) const override;

private:
};
