#include "bookinfoinputwidget.hpp"

#include "ui_bookinfoinputwidget.h"

#include <QCompleter>

/* explicit  */ BookInfoInputWidget::BookInfoInputWidget(QWidget* parent /* = nullptr */)
: QWidget(parent),
  _ui(std::make_unique<Ui::BookInfoInputWidget>()),
  _genres_string_model(new QStringListModel(this)),
  _genres_completer(new QCompleter(this))
{
    _ui->setupUi(this);

    // setup genres autocompletion
    _genres_completer->setCaseSensitivity(Qt::CaseInsensitive);
    _genres_completer->setCompletionMode(QCompleter::PopupCompletion);
    _ui->le_genres->setCompleter(_genres_completer);

    QObject::connect(_ui->pb_save, &QPushButton::clicked, this, &BookInfoInputWidget::onSaveButtonClicked);
}

BookInfoInputWidget::~BookInfoInputWidget() /* override */ = default;

void BookInfoInputWidget::setPriorities(const std::shared_ptr<std::vector<bd::Priority>>& priorities)
{
    _ui->cb_priority->clear();

    auto idx{0};
    std::ranges::for_each(*priorities,
                          [this, &idx](const bd::Priority& priority) -> void
                          {
                              _ui->cb_priority->insertItem(idx++, QString::fromStdString(priority.name), priority.id);
                          }); // insert string with sql row id as user data
}

void BookInfoInputWidget::setAvaibilities(const std::shared_ptr<std::vector<bd::Avaibility>>& avaibilities)
{
    _ui->cb_avaibility->clear();

    auto idx{0};
    std::ranges::for_each(*avaibilities,
                          [this, &idx](const bd::Avaibility& avaibility) -> void
                          {
                              _ui->cb_avaibility->insertItem(idx++, QString::fromStdString(avaibility.name), avaibility.id);
                          }); // insert string with sql row id as user data
}

void BookInfoInputWidget::setGenres(const std::shared_ptr<std::vector<bd::Genre>>& genres)
{
    // update genres autocompletion base
    QStringList list;

    std::ranges::for_each(*genres,
                          [&list](const bd::Genre& genre) -> void { list.append(QString::fromStdString(genre.name)); });

    _genres_string_model->setStringList(list);
    _genres_completer->setModel(_genres_string_model);
}

void BookInfoInputWidget::setAuthors(const std::shared_ptr<std::vector<bd::Author>>& authors) {}

void BookInfoInputWidget::onSaveButtonClicked()
{
    // get information from gui form
    auto title = _ui->le_title->text().toStdString();
    // auto author = _ui->le_author->text().toStdString();
    auto priority = _ui->cb_priority->currentData().toInt();
    auto avaibility = _ui->cb_priority->currentData().toInt();
    // auto genres = _ui->le_genres->text().toStdString();

    emit infoSaved(
        bd::Book{.id = -1, .title = std::move(title), .author = 1, .avaibility = avaibility, .priority = priority, .genres = {}});

    close();
}
