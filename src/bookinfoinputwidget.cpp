#include "bookinfoinputwidget.hpp"

#include "ui_bookinfoinputwidget.h"

#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>

#include <QCompleter>
#include <QInputDialog>
#include <qobject.h>

#include <algorithm>

/* explicit  */ BookInfoInputWidget::BookInfoInputWidget(QWidget* parent /* = nullptr */)
: QWidget(parent),
  _ui(std::make_unique<Ui::BookInfoInputWidget>()),
  _genres_string_model(new QStringListModel(this)),
  _genres_completer(new QCompleter(this)),
  _authors_string_model(new QStringListModel(this)),
  _authors_completer(new QCompleter(this))
{
    _ui->setupUi(this);

    // TODO make custom genres autocompletion
    // setup genres autocompletion
    _genres_completer->setCaseSensitivity(Qt::CaseInsensitive);
    _genres_completer->setCompletionMode(QCompleter::PopupCompletion);
    _ui->le_genres->setCompleter(_genres_completer);

    // setup authors autocompletion
    _authors_completer->setCaseSensitivity(Qt::CaseInsensitive);
    _authors_completer->setCompletionMode(QCompleter::PopupCompletion);
    _ui->le_author->setCompleter(_authors_completer);


    QObject::connect(_ui->pb_save, &QPushButton::clicked, this, &BookInfoInputWidget::onSaveButtonClicked);
}

BookInfoInputWidget::~BookInfoInputWidget() /* override */ = default;

void BookInfoInputWidget::setPriorities(const std::shared_ptr<std::vector<std::string>>& priorities)
{
    _ui->cb_priority->clear();

    auto idx{0};
    std::ranges::for_each(*priorities,
                          [this, &idx](const std::string& priority) -> void
                          {
                              _ui->cb_priority->insertItem(idx++, QString::fromStdString(priority));
                          }); // insert string with sql row id as user data
}

void BookInfoInputWidget::setAvaibilities(const std::shared_ptr<std::vector<std::string>>& avaibilities)
{
    _ui->cb_avaibility->clear();

    auto idx{0};
    std::ranges::for_each(*avaibilities,
                          [this, &idx](const std::string& avaibility) -> void
                          {
                              _ui->cb_avaibility->insertItem(idx++, QString::fromStdString(avaibility));
                          }); // insert string with sql row id as user data
}

void BookInfoInputWidget::setGenres(const std::shared_ptr<std::vector<std::string>>& genres)
{
    _genres = genres;

    // update genres autocompletion base
    QStringList list;

    std::ranges::for_each(*genres,
                          [&list](const std::string& genre) -> void { list.append(QString::fromStdString(genre)); });

    // update model
    _genres_string_model->setStringList(list);
    _genres_completer->setModel(_genres_string_model);
}

void BookInfoInputWidget::setAuthors(const std::shared_ptr<std::vector<std::string>>& authors)
{
    _authors = authors;

    // update authors autocompletion base
    QStringList list;

    std::ranges::for_each(*authors,
                          [&list](const std::string& genre) -> void { list.append(QString::fromStdString(genre)); });

    // update model
    _authors_string_model->setStringList(list);
    _authors_completer->setModel(_authors_string_model);
}

void BookInfoInputWidget::setBookInfo(const bd::Book& book)
{
    if (_ui->cb_avaibility->count() == 0) {
        throw std::runtime_error("avaibilities must be set before setBookInfo call");
    }

    if (_ui->cb_priority->count() == 0) {
        throw std::runtime_error("priorities must be set before setBookInfo call");
    }

    _ui->cb_priority->setCurrentText(QString::fromStdString(book.priority));
    _ui->cb_avaibility->setCurrentText(QString::fromStdString(book.avaibility));

    _ui->le_title->setText(QString::fromStdString(book.title));
    _ui->le_author->setText(QString::fromStdString(book.author));

    if (book.genres.size() == 0) {
        return;
    }

    std::string genres_str = book.genres[0];
    qDebug() << "initially" << genres_str.c_str();
    for (auto i = 1ULL; i < book.genres.size(); ++i) {
        qDebug() << i << "elem:" << book.genres[i].c_str();
        genres_str += ',';
        genres_str += book.genres[i];
    }

    qDebug() << genres_str.c_str();

    _ui->le_genres->setText(QString::fromStdString(genres_str));
}

void BookInfoInputWidget::onSaveButtonClicked()
{
    // get information from gui form
    auto title = _ui->le_title->text().toStdString();

    auto author = _ui->le_author->text().toStdString();

    auto priority = _ui->cb_priority->currentText().toStdString();

    auto avaibility = _ui->cb_avaibility->currentText().toStdString();

    // genres string in "genre1,genre2,genre3" format
    auto genres = _ui->le_genres->text().toStdString();

    // split by comma, semicolon and space
    boost::char_separator<char> separator(", ;");
    boost::tokenizer<boost::char_separator<char>> tokenizer(genres, separator);

    std::vector<std::string> separated_genres;

    std::ranges::for_each(tokenizer, [&separated_genres](const auto& str) { separated_genres.push_back(str); });

    qDebug() << "book saved" << title.c_str() << author.c_str() << avaibility.c_str() << priority.c_str()
             << genres.c_str();

    emit infoSaved(bd::Book{.title = std::move(title),
                            .author = std::move(author),
                            .avaibility = std::move(avaibility),
                            .priority = std::move(priority),
                            .genres = std::move(separated_genres)});

    close();
}
