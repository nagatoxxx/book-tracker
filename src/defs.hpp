#pragma once

#include <string>
#include <vector>

namespace BooksDatabase
{
inline constexpr std::string_view FILENAME{"books.db"};
inline constexpr std::string_view QT_DRIVER{"QSQLITE"};
inline constexpr auto COLUMNS_IN_JOINED{6}; // without id

struct Book
{
    std::string title;
    std::string author;
    std::string avaibility;
    std::string priority;
    std::vector<std::string> genres;
};
} // namespace BooksDatabase
