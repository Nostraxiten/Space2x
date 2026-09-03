#pragma once

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class DashboardView : public QWidget {
    Q_OBJECT

public:
    explicit DashboardView(core::Engine& engine, QWidget* parent = nullptr);
    ~DashboardView() override = default;

public slots:
    void refreshTelemetry();

private:
    void setupUi();

    core::Engine& m_engine;
    QTimer*       m_telemetryTimer{nullptr};

    QLabel*       m_cpuLabel{nullptr};
    QProgressBar* m_cpuProgress{nullptr};

    QLabel*       m_memLabel{nullptr};
    QProgressBar* m_memProgress{nullptr};

    QLabel*       m_swapLabel{nullptr};
    QLabel*       m_uptimeLabel{nullptr};
    QLabel*       m_servicesSummaryLabel{nullptr};
};

} // namespace space2x::ui
