#pragma once

#include <QWidget>
#include <QTableWidget>

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

private:
    void setupUi();

    core::Engine& m_engine;
    QTableWidget* m_ifaceTable{nullptr};
    QTableWidget* m_socketsTable{nullptr};
};

} // namespace space2x::ui
