#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class LogsView : public QWidget {
    Q_OBJECT

public:
    explicit LogsView(core::Engine& engine, QWidget* parent = nullptr);
    ~LogsView() override = default;

public slots:
    void refreshLogs();

private:
    void setupUi();

    core::Engine& m_engine;
    QTextEdit*    m_logText{nullptr};
};

} // namespace space2x::ui
