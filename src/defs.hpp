#pragma once

#include <string>
#include <vector>

namespace BooksDatabase
{
struct Book
{
    std::string title;
    std::string author;
    std::string avaibility;
    std::string priority;
    std::vector<std::string> genres;
};
} // namespace BooksDatabase
