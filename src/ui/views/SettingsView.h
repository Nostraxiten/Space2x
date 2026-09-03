#pragma once

#include <QWidget>
#include <QLabel>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class SettingsView : public QWidget {
    Q_OBJECT

public:
    explicit SettingsView(core::Engine& engine, QWidget* parent = nullptr);
    ~SettingsView() override = default;

private:
    void setupUi();

    core::Engine& m_engine;
};

} // namespace space2x::ui
