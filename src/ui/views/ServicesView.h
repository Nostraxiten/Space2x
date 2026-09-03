#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class ServicesView : public QWidget {
    Q_OBJECT

public:
    explicit ServicesView(core::Engine& engine, QWidget* parent = nullptr);
    ~ServicesView() override = default;

public slots:
    void refreshServices();
    void applyTheme();

private:
    void setupUi();
    void handleStart(const std::string& serviceId);
    void handleStop(const std::string& serviceId);
    void handleRestart(const std::string& serviceId);
    void handleHealthCheck(const std::string& serviceId);

    core::Engine& m_engine;
    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};
    QPushButton*  m_refreshBtn{nullptr};
    QTableWidget* m_table{nullptr};
};

} // namespace space2x::ui
