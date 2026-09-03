#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

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
    void applyTheme();

private:
    void setupUi();

    core::Engine& m_engine;
    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};
    QPushButton*  m_refreshBtn{nullptr};
    QTableWidget* m_table{nullptr};
};

} // namespace space2x::ui
