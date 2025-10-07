#include "vieweventfilter.hpp"

#include <QEvent>
#include <QHeaderView>

/* explicit  */ TableViewEventFilter::TableViewEventFilter(QTableView* table_view, QObject* parent /* = nullptr */)
: QObject(parent),
  _table_view(table_view)
{
}

TableViewEventFilter::~TableViewEventFilter() /* override  */ = default;

bool TableViewEventFilter::eventFilter(QObject* object, QEvent* event) /* override  */
{
    if (object == _table_view && event->type() == QEvent::Resize) {
        // align column sizes when resizing
        auto full_width = _table_view->width();
        auto columns = _table_view->horizontalHeader()->count();

        // size of each column
        auto size = full_width / columns;

        // set size for each column
        for (auto i = 0; i < columns; ++i) {
            _table_view->setColumnWidth(i, size);
        }
    }

    return false;
}
