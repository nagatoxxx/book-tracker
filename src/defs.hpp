#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

namespace BooksDatabase
{
using primary_key_t = int;
template <typename T>
using foreign_key_t = std::optional<T>;

// WARN id == -1 if unused

struct Author
{
    // static constexpr std::array<std::string_view, 2> names{"id", "name"};
    primary_key_t id;
    std::string name;
};

struct Priority
{
    // static constexpr std::array<std::string_view, 2> names{"id", "name"};
    primary_key_t id;
    std::string name;
};

struct Avaibility
{
    // static constexpr std::array<std::string_view, 2> names{"id", "name"};
    primary_key_t id;
    std::string name;
};

struct Genre
{
    // static constexpr std::array<std::string_view, 2> names{"id", "name"};
    primary_key_t id;
    std::string name;
};

struct Book
{
    // static constexpr std::array<std::string_view, 6> names{"id", "title", "author", "avaibility", "priority", "genres"};
    primary_key_t id;
    std::string title;
    foreign_key_t<decltype(Author::id)> author;
    foreign_key_t<decltype(Avaibility::id)> avaibility;
    foreign_key_t<decltype(Priority::id)> priority;
    std::vector<foreign_key_t<decltype(Genre::id)>> genres;
};
} // namespace BooksDatabase
