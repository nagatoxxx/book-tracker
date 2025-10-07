#include "mainwindow.hpp"

#include "ui_mainwindow.h"

/* explicit  */ MainWindow::MainWindow(QWidget* parent /* = nullptr */)
: QMainWindow(parent),
  _ui(std::make_unique<Ui::MainWindow>())
{
    setMinimumSize(800, 600);
}

MainWindow::~MainWindow() /* override */ = default;
