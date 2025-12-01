#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    auto* window = new MainWindow;

    window->show();

    return app.exec();
}
