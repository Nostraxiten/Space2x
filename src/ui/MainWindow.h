#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <memory>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<core::Engine> engine, QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onSidebarRowChanged(int row);

private:
    void setupUi();

    std::unique_ptr<core::Engine> m_engine;
    QListWidget*                  m_sidebar{nullptr};
    QStackedWidget*               m_stack{nullptr};
};

} // namespace space2x::ui
