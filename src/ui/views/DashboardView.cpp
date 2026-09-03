#include "DashboardView.h"
#include <space2x/core/Engine.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace space2x::ui {

namespace {

QString formatBytes(uint64_t bytes) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    if (bytes >= GB) {
        return QString::number(static_cast<double>(bytes) / GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(static_cast<double>(bytes) / MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(static_cast<double>(bytes) / KB, 'f', 2) + " KB";
    }
    return QString::number(bytes) + " B";
}

} // anonymous namespace

DashboardView::DashboardView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();

    m_telemetryTimer = new QTimer(this);
    connect(m_telemetryTimer, &QTimer::timeout, this, &DashboardView::refreshTelemetry);
    m_telemetryTimer->start(1500);

    refreshTelemetry();
}

void DashboardView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // Title Section
    auto* titleLabel = new QLabel("System & Infrastructure Dashboard", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    mainLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Real-time telemetry, resource metrics, and service status overview.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    mainLayout->addWidget(subTitleLabel);

    // Cards Grid
    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(16);

    // CPU Group
    auto* cpuGroup = new QGroupBox("CPU Utilization", this);
    cpuGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* cpuLayout = new QVBoxLayout(cpuGroup);
    m_cpuLabel = new QLabel("0.0 %", cpuGroup);
    m_cpuLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #2563EB;");
    m_cpuProgress = new QProgressBar(cpuGroup);
    m_cpuProgress->setRange(0, 100);
    m_cpuProgress->setTextVisible(false);
    m_cpuProgress->setFixedHeight(8);
    m_cpuProgress->setStyleSheet("QProgressBar { background: #E2E8F0; border-radius: 4px; } QProgressBar::chunk { background: #2563EB; border-radius: 4px; }");
    cpuLayout->addWidget(m_cpuLabel);
    cpuLayout->addWidget(m_cpuProgress);
    gridLayout->addWidget(cpuGroup, 0, 0);

    // Memory Group
    auto* memGroup = new QGroupBox("Physical Memory (RAM)", this);
    memGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* memLayout = new QVBoxLayout(memGroup);
    m_memLabel = new QLabel("0.0 GB / 0.0 GB", memGroup);
    m_memLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0D9488;");
    m_memProgress = new QProgressBar(memGroup);
    m_memProgress->setRange(0, 100);
    m_memProgress->setTextVisible(false);
    m_memProgress->setFixedHeight(8);
    m_memProgress->setStyleSheet("QProgressBar { background: #E2E8F0; border-radius: 4px; } QProgressBar::chunk { background: #0D9488; border-radius: 4px; }");
    memLayout->addWidget(m_memLabel);
    memLayout->addWidget(m_memProgress);
    gridLayout->addWidget(memGroup, 0, 1);

    // Swap / Paging Group
    auto* swapGroup = new QGroupBox("Swap / Pagefile", this);
    swapGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* swapLayout = new QVBoxLayout(swapGroup);
    m_swapLabel = new QLabel("0.0 GB / 0.0 GB", swapGroup);
    m_swapLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #475569;");
    swapLayout->addWidget(m_swapLabel);
    gridLayout->addWidget(swapGroup, 1, 0);

    // Host Uptime Group
    auto* uptimeGroup = new QGroupBox("Host Uptime", this);
    uptimeGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* uptimeLayout = new QVBoxLayout(uptimeGroup);
    m_uptimeLabel = new QLabel("0h 0m 0s", uptimeGroup);
    m_uptimeLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #475569;");
    uptimeLayout->addWidget(m_uptimeLabel);
    gridLayout->addWidget(uptimeGroup, 1, 1);

    mainLayout->addLayout(gridLayout);

    // Services summary banner
    auto* summaryGroup = new QGroupBox("Managed Services Summary", this);
    summaryGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* summaryLayout = new QVBoxLayout(summaryGroup);
    m_servicesSummaryLabel = new QLabel("Scanning registered services...", summaryGroup);
    m_servicesSummaryLabel->setStyleSheet("font-size: 14px; color: #334155;");
    summaryLayout->addWidget(m_servicesSummaryLabel);
    mainLayout->addWidget(summaryGroup);

    mainLayout->addStretch();
}

void DashboardView::refreshTelemetry() {
    auto res = m_engine.systemMonitor().getMetrics();
    if (res.isOk()) {
        const auto& m = res.value();
        m_cpuLabel->setText(QString::number(m.cpuUsagePercent, 'f', 1) + " %");
        m_cpuProgress->setValue(static_cast<int>(m.cpuUsagePercent));

        if (m.totalPhysicalMemoryBytes > 0) {
            double usedPercent = (static_cast<double>(m.usedPhysicalMemoryBytes) / static_cast<double>(m.totalPhysicalMemoryBytes)) * 100.0;
            m_memLabel->setText(formatBytes(m.usedPhysicalMemoryBytes) + " / " + formatBytes(m.totalPhysicalMemoryBytes));
            m_memProgress->setValue(static_cast<int>(usedPercent));
        }

        m_swapLabel->setText(formatBytes(m.usedSwapBytes) + " / " + formatBytes(m.totalSwapBytes));

        uint64_t hours = m.uptimeSeconds / 3600;
        uint64_t minutes = (m.uptimeSeconds % 3600) / 60;
        uint64_t seconds = m.uptimeSeconds % 60;
        m_uptimeLabel->setText(QString("%1h %2m %3s").arg(hours).arg(minutes).arg(seconds));
    }

    auto svcRes = m_engine.serviceController().listServices();
    if (svcRes.isOk()) {
        size_t running = 0, stopped = 0, other = 0;
        for (const auto& s : svcRes.value()) {
            if (s.state == core::ServiceState::Running) running++;
            else if (s.state == core::ServiceState::Stopped) stopped++;
            else other++;
        }
        m_servicesSummaryLabel->setText(
            QString("Total Providers: %1 | Running: %2 | Stopped: %3 | Other: %4")
                .arg(svcRes.value().size())
                .arg(running)
                .arg(stopped)
                .arg(other)
        );
    }
}

} // namespace space2x::ui
