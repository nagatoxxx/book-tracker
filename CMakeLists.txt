cmake_minimum_required(VERSION 3.16)

project(books VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-g -Wall -Wpedantic -Wextra)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Sql)

set(PROJECT_SRC
    src/main.cpp
    src/mainwindow.hpp
    src/mainwindow.cpp
    src/mainwindow.ui
    src/booksmodel.hpp
    src/booksmodel.cpp
    src/bookinfoinputwidget.ui
    src/bookinfoinputwidget.hpp
    src/bookinfoinputwidget.cpp
    src/booksproxymodel.hpp
    src/booksproxymodel.cpp

    src/util.hpp
)

add_executable(books ${PROJECT_SRC})
target_link_libraries(books PRIVATE Qt6::Widgets Qt6::Sql)
target_include_directories(books PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/books_autogen/include)
target_include_directories(books PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
