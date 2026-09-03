#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class NetworkView : public QWidget {
    Q_OBJECT

public:
    explicit NetworkView(core::Engine& engine, QWidget* parent = nullptr);
    ~NetworkView() override = default;

public slots:
    void refreshNetwork();
    void applyTheme();

private:
    void setupUi();

    core::Engine& m_engine;
    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};
    QLabel*       m_ifaceTitle{nullptr};
    QLabel*       m_socketsTitle{nullptr};
    QPushButton*  m_refreshBtn{nullptr};
    QTableWidget* m_ifaceTable{nullptr};
    QTableWidget* m_socketsTable{nullptr};
};

} // namespace space2x::ui
