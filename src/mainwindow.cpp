#include "mainwindow.hpp"

#include "bookinfoinputwidget.hpp"
#include "query.hpp"
#include "ui_mainwindow.h"
#include "util.hpp"

#include <QSqlError>
#include <QSqlQuery>

/* explicit  */ MainWindow::MainWindow(QWidget* parent /* = nullptr */)
: QMainWindow(parent),
  _ui(std::make_unique<Ui::MainWindow>()),
  _model(new BooksModel(utils::strviewToQString(BooksDatabase::FILENAME)))
{
    _ui->setupUi(this);

    _ui->tv_books->setModel(_model);
    // _ui->tv_books->installEventFilter(new TableViewEventFilter(_ui->tv_books, _ui->tv_books)); // to process widget resizes
    // _ui->tv_books->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // prevents columns size changes
    _ui->tv_books->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows); // select rows, not cells
    _ui->tv_books->setEditTriggers(QTableView::NoEditTriggers);

    setMinimumSize(800, 600);

    QObject::connect(_ui->pb_add, &QPushButton::clicked, this, &MainWindow::addBook);
}

MainWindow::~MainWindow() /* override */ = default;

void MainWindow::addBook()
{
    auto* widget = new BookInfoInputWidget;
    widget->setWindowModality(Qt::WindowModal);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->setWindowFlags(Qt::Tool);
    widget->show();

    // set genres, priority, avaibility to autocompletion
    widget->setPriorities(std::make_shared<std::vector<std::string>>(_model->priorities()));
    widget->setAvaibilities(std::make_shared<std::vector<std::string>>(_model->avaibilities()));
    widget->setAuthors(std::make_shared<std::vector<std::string>>(_model->authors()));
    widget->setGenres(std::make_shared<std::vector<std::string>>(_model->genres()));

    auto on_info_saved = [this](const BooksDatabase::Book& book) { _model->insertBook(book); };

    QObject::connect(widget, &BookInfoInputWidget::infoSaved, this, on_info_saved);
}
