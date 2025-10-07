#pragma once

#include <string_view>

namespace BooksDatabase
{
static constexpr std::string_view FILENAME{"books.db"};
static constexpr std::string_view QT_DRIVER{"QSQLITE"};

static constexpr auto COLUMNS_IN_JOINED{6}; // без id

static constexpr std::string_view GET_ALL = R"(
SELECT 
Books.book_title,
Authors.author_name,
Priorities.priority_name, 
Avalibility.avaibility_name, 
GROUP_CONCAT(Genres.genre_name)
FROM BooksGenres
INNER JOIN Books on BooksGenres.book_id = Books.book_id
INNER JOIN Authors on Books.book_author = Authors.author_id
INNER JOIN Priorities on Books.book_priority = Priorities.priority_id
INNER JOIN Avalibility on Books.book_avaibility = Avalibility.avaibility_id
INNER JOIN Genres on BooksGenres.genre_id = Genres.genre_id
GROUP BY (book_title);
)";
} // namespace BooksDatabase
