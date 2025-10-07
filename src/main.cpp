#include "editablemodel.hpp"
#include "mainwindow.hpp"
#include "query.hpp"
#include "vieweventfilter.hpp"

#include <QApplication>
#include <QHeaderView>
#include <QTableView>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QSqlDatabase database = QSqlDatabase::addDatabase(QString::fromStdString(std::string(BooksDatabase::QT_DRIVER)));
    database.setDatabaseName("../books.db");
    if (!database.open()) {
        qWarning() << "error!";
        return -1;
    }

    auto* model = new EditableModel(database);
    model->loadData();

    auto* view = new QTableView();
    view->installEventFilter(new TableViewEventFilter(view, view));
    view->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    view->setModel(model);
    view->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    view->show();

    return app.exec();
}
