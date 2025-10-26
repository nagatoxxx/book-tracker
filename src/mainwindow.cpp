#include "mainwindow.hpp"

#include "bookinfoinputwidget.hpp"
#include "query.hpp"
#include "ui_mainwindow.h"
#include "util.hpp"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSqlError>
#include <QSqlQuery>

/* explicit  */ MainWindow::MainWindow(QWidget* parent /* = nullptr */)
: QMainWindow(parent),
  _ui(std::make_unique<Ui::MainWindow>()),
  _model(new BooksModel(utils::strviewToQString(BooksDatabase::FILENAME))),
  _popup_menu(new QMenu(this))
{
    _ui->setupUi(this);

    makeMenu();

    // table view config
    _ui->tv_books->setModel(_model);
    // _ui->tv_books->installEventFilter(new TableViewEventFilter(_ui->tv_books, _ui->tv_books)); // to process widget resizes
    // _ui->tv_books->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed); // prevents columns size changes
    _ui->tv_books->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows); // select rows, not cells
    _ui->tv_books->setEditTriggers(QTableView::NoEditTriggers);

    setMinimumSize(800, 600);
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

void MainWindow::removeSelectedBooks()
{
    // TODO check this
    const auto selection = _ui->tv_books->selectionModel()->selectedRows();

    std::vector<std::string_view> titles;
    titles.reserve(selection.count());

    // multiple rows can be selected
    for (int i = 0; i < selection.count(); i++) {
        const auto index = selection.at(i);

        titles.emplace_back(_ui->tv_books->model()->data(index).toString().toStdString());
    }

    _model->deleteBooks(titles);
}

void MainWindow::editBook() {}

void MainWindow::contextMenuEvent(QContextMenuEvent* event) /* override  */
{
    const auto local_coords = mapFromGlobal(event->globalPos());
    const auto* widget = childAt(local_coords); // widget under cursor

    _action_remove_book->setEnabled(false);

    if (widget == _ui->tv_books->viewport()) {
        // enable "remove" only if selection is not empty
        if (_ui->tv_books->selectionModel()->selectedRows().count() != 0) {
            _action_remove_book->setEnabled(true);
        }

        _popup_menu->exec(event->globalPos());
    }
}


void MainWindow::makeMenu()
{
    _action_remove_book = new QAction(tr("Удалить"));
    _action_add_book = new QAction(tr("Добавить"));
    _action_edit_book = new QAction(tr("Изменить"));

    _popup_menu->addAction(_action_remove_book);
    _popup_menu->addAction(_action_add_book);
    _popup_menu->addAction(_action_edit_book);

    QObject::connect(_action_remove_book, &QAction::triggered, this, &MainWindow::removeSelectedBooks);
    QObject::connect(_action_add_book, &QAction::triggered, this, &MainWindow::addBook);
}
