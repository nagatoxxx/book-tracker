#include "mainwindow.hpp"

#include "query.hpp"
#include "ui_mainwindow.h"
#include "util.hpp"
#include "vieweventfilter.hpp"

/* explicit  */ MainWindow::MainWindow(QWidget* parent /* = nullptr */)
: QMainWindow(parent),
  _ui(std::make_unique<Ui::MainWindow>())
{
    _ui->setupUi(this);

    setMinimumSize(800, 600);

    // initialize database connection
    auto database = QSqlDatabase::addDatabase(utils::strviewToQString(BooksDatabase::QT_DRIVER));
    database.setDatabaseName(utils::strviewToQString(BooksDatabase::FILENAME));
    if (!database.open()) {
        throw std::runtime_error("cannot open database");
    }

    _model = new EditableModel(database);

    _ui->tv_books->setModel(_model);
    _ui->tv_books->installEventFilter(new TableViewEventFilter(_ui->tv_books, _ui->tv_books)); // to process widget resizes
    _ui->tv_books->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // prevents columns size changes
    _ui->tv_books->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows); // select rows, not cells
}

MainWindow::~MainWindow() /* override */ = default;
