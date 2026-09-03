#pragma once

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QWidget>
#include <memory>
#include <space2x/core/Engine.h>

namespace space2x::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<core::Engine> engine, QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSidebarRowChanged(int row);
    void onToggleTheme();
    void applyTheme();

private:
    void setupUi();

    std::unique_ptr<core::Engine> m_engine;
    QListWidget*                  m_sidebar{nullptr};
    QStackedWidget*               m_stack{nullptr};
    QPushButton*                  m_themeBtn{nullptr};
    QWidget*                      m_sidebarContainer{nullptr};
};

} // namespace space2x::ui
