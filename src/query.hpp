#pragma once

#include <string_view>

namespace BooksDatabase
{
static constexpr std::string_view FILENAME{"books.db"};
static constexpr std::string_view QT_DRIVER{"QSQLITE"};
static constexpr auto COLUMNS_IN_JOINED{6}; // without id

//#===============--------------- SELECTS IN TABLES ---------------===============#//

static constexpr std::string_view GET_ALL = R"(
SELECT 
Books.book_title as title,
Authors.author_name as author,
Priorities.priority_name as priority, 
Avaibilities.avaibility_name as avaibility, 
GROUP_CONCAT(Genres.genre_name) as genres
FROM Books
FULL OUTER JOIN BooksGenres on BooksGenres.book_id = Books.book_id
JOIN Authors on Books.book_author = Authors.author_id
JOIN Priorities on Books.book_priority = Priorities.priority_id
JOIN Avaibilities on Books.book_avaibility = Avaibilities.avaibility_id
LEFT JOIN Genres on BooksGenres.genre_id = Genres.genre_id
GROUP BY (title);
)";

static constexpr std::string_view GET_GENRES = R"(
SELECT
Genres.genre_id as id,
Genres.genre_name as name
FROM Genres;
)";

static constexpr std::string_view GET_PRIORITIES = R"(
SELECT 
Priorities.priority_id as id,
Priorities.priority_name as name,
Priorities.priority_value as value
FROM Priorities;
)";

static constexpr std::string_view GET_AVAIBILITIES = R"(
SELECT
Avaibilities.avaibility_id as id,
Avaibilities.avaibility_name as name
FROM Avaibilities;
)";

static constexpr std::string_view GET_AUTHORS = R"(
SELECT 
Authors.author_id as id,
Authors.author_name as name
FROM Authors;
)";

//#===============--------------- INSERTS INTO TABLES ---------------===============#//

// bindValue needs to be used (QSqlQuery)
static constexpr std::string_view INSERT_BOOK = R"(
INSERT INTO Books (book_title,book_author,book_avaibility,book_priority) VALUES (:title,:author,:avaibility,:priority);
)";
} // namespace BooksDatabase
