#include <QMainWindow>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
};
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;

    MainWindow& operator=(const MainWindow&) = delete;
    MainWindow& operator=(MainWindow&&) = delete;

    ~MainWindow() override;

private:
    std::unique_ptr<Ui::MainWindow> _ui;
};
