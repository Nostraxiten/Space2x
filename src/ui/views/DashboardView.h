#pragma once

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
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
    void applyTheme();

private:
    void setupUi();

    core::Engine& m_engine;
    QTimer*       m_telemetryTimer{nullptr};

    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};

    QGroupBox*    m_cpuGroup{nullptr};
    QLabel*       m_cpuLabel{nullptr};
    QProgressBar* m_cpuProgress{nullptr};

    QGroupBox*    m_memGroup{nullptr};
    QLabel*       m_memLabel{nullptr};
    QProgressBar* m_memProgress{nullptr};

    QGroupBox*    m_swapGroup{nullptr};
    QLabel*       m_swapLabel{nullptr};

    QGroupBox*    m_uptimeGroup{nullptr};
    QLabel*       m_uptimeLabel{nullptr};

    QGroupBox*    m_summaryGroup{nullptr};
    QLabel*       m_servicesSummaryLabel{nullptr};
};

} // namespace space2x::ui
