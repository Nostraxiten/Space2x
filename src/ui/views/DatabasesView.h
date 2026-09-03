#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class DatabasesView : public QWidget {
    Q_OBJECT

public:
    explicit DatabasesView(core::Engine& engine, QWidget* parent = nullptr);
    ~DatabasesView() override = default;

public slots:
    void refreshDatabases();

private:
    void setupUi();

    core::Engine& m_engine;
    QTableWidget* m_table{nullptr};
};

} // namespace space2x::ui
