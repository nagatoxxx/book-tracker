#pragma once

#include <string_view>

namespace BooksDatabase
{
inline constexpr std::string_view FILENAME{"books.db"};
inline constexpr std::string_view QT_DRIVER{"QSQLITE"};
inline constexpr auto COLUMNS_IN_JOINED{6}; // without id

//#===============--------------- SELECTS IN TABLES ---------------===============#//

inline constexpr std::string_view GET_ALL = R"(
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

inline constexpr std::string_view GET_GENRES = R"(
SELECT
Genres.genre_id as id,
Genres.genre_name as name
FROM Genres;
)";

inline constexpr std::string_view GET_PRIORITIES = R"(
SELECT 
Priorities.priority_id as id,
Priorities.priority_name as name,
Priorities.priority_value as value
FROM Priorities;
)";

inline constexpr std::string_view GET_AVAIBILITIES = R"(
SELECT
Avaibilities.avaibility_id as id,
Avaibilities.avaibility_name as name
FROM Avaibilities;
)";

inline constexpr std::string_view GET_AUTHORS = R"(
SELECT 
Authors.author_id as id,
Authors.author_name as name
FROM Authors;
)";

//#===============--------------- INSERTS INTO TABLES ---------------===============#//
inline constexpr std::string_view INSERT_AUTHOR_NE = R"(
INSERT INTO Authors (author_name) SELECT :a WHERE NOT EXISTS (SELECT 1 FROM Authors WHERE author_name = :a);
)";

inline constexpr std::string_view INSERT_AVAIBILITY_NE = R"(
INSERT INTO Avaibilities (avaibility_name) SELECT :v WHERE NOT EXISTS (SELECT 1 FROM Avaibilities WHERE avaibility_name = :v);
)";

inline constexpr std::string_view INSERT_PRIORITY_NE = R"(
INSERT INTO Priorities (priority_name) SELECT :p WHERE NOT EXISTS (SELECT 1 FROM Priorities WHERE priority_name = :p);
)";

inline constexpr std::string_view INSERT_GENRE_NE = R"(
INSERT INTO Genres (genre_name) SELECT :g WHERE NOT EXISTS (SELECT 1 FROM Genres WHERE genre_name = :g);
)";

inline constexpr std::string_view INSERT_BOOK_GENRE_NE = R"(
INSERT INTO BooksGenres (book_id, genre_id)
SELECT :b, :g
WHERE NOT EXISTS (
    SELECT 1 FROM BooksGenres WHERE book_id = :b AND genre_id = :g
);
)";

// bindValue needs to be used (QSqlQuery)
inline constexpr std::string_view INSERT_BOOK = R"(
INSERT INTO Books (book_title, book_author, book_avaibility, book_priority)
VALUES (:title,
(SELECT author_id FROM Authors where author_name = :author),
(SELECT avaibility_id FROM Avaibilities where avaibility_name = :avaibility),
(SELECT priority_id FROM Priorities where priority_name = :priority));
)";
} // namespace BooksDatabase
