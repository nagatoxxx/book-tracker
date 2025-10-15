#pragma once

#include "defs.hpp"

#include <QCompleter>
#include <QStringListModel>
#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class BookInfoInputWidget;
};
QT_END_NAMESPACE

namespace bd = BooksDatabase;

class BookInfoInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BookInfoInputWidget(QWidget* parent = nullptr);

    BookInfoInputWidget(const BookInfoInputWidget&) = delete;
    BookInfoInputWidget(BookInfoInputWidget&&) = delete;

    BookInfoInputWidget& operator=(const BookInfoInputWidget&) = delete;
    BookInfoInputWidget& operator=(BookInfoInputWidget&&) = delete;

    ~BookInfoInputWidget() override;

    void setPriorities(const std::shared_ptr<std::vector<bd::Priority>>& priorities);
    void setAvaibilities(const std::shared_ptr<std::vector<bd::Avaibility>>& avaibilities);
    void setGenres(const std::shared_ptr<std::vector<bd::Genre>>& genres);
    void setAuthors(const std::shared_ptr<std::vector<bd::Author>>& authors);

signals:
    void infoSaved(const bd::Book&);

private slots:
    void onSaveButtonClicked();

private:
    std::unique_ptr<Ui::BookInfoInputWidget> _ui{};

    QStringListModel* _genres_string_model{nullptr}; // model to keep genres names
    QCompleter* _genres_completer{nullptr};
};
