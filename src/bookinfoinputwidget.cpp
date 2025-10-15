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

void BookInfoInputWidget::setPriorities(std::shared_ptr<std::vector<bd::Priority>> priorities)
{
    _ui->cb_priority->clear();

    auto idx{0};
    std::ranges::for_each(*priorities,
                          [this, &idx](const bd::Priority& priority) -> void
                          {
                              _ui->cb_priority->insertItem(idx++, QString::fromStdString(priority.name), priority.id);
                          }); // insert string with sql row id as user data
}

void BookInfoInputWidget::setAvaibilities(std::shared_ptr<std::vector<bd::Avaibility>> avaibilities) {}

void BookInfoInputWidget::setGenres(std::shared_ptr<std::vector<bd::Genre>> genres)
{
    // // _genres = genres;
    //
    // // update genres autocompletion base
    // QStringList list;
    // for (const auto& genre : *genres) {
    //     list.append(QString::fromStdString(genre.name));
    // }
    //
    // _genres_string_model->setStringList(list);
    // _genres_completer->setModel(_genres_string_model);
}

void BookInfoInputWidget::setAuthors(std::shared_ptr<std::vector<bd::Author>> authors) {}

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
}
