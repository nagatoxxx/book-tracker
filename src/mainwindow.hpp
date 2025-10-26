#include "booksmodel.hpp"

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>

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

private slots:
    void addBook();
    void removeSelectedBooks();
    void editBook();

    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void makeMenu();

    std::unique_ptr<Ui::MainWindow> _ui{nullptr};
    BooksModel* _model{nullptr};

    QMenu* _popup_menu{nullptr};
    QAction* _action_remove_book{nullptr};
    QAction* _action_add_book{nullptr};
    QAction* _action_edit_book{nullptr};
};
