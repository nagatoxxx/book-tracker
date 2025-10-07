#pragma once

#include <QObject>
#include <QTableView>

class TableViewEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit TableViewEventFilter(QTableView* table_view, QObject* parent = nullptr);

    TableViewEventFilter(const TableViewEventFilter&) = delete;
    TableViewEventFilter(TableViewEventFilter&&) = delete;

    TableViewEventFilter& operator=(const TableViewEventFilter&) = delete;
    TableViewEventFilter& operator=(TableViewEventFilter&&) = delete;

    ~TableViewEventFilter() override;

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    QTableView* _table_view;
};
