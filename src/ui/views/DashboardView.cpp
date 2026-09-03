#include "DashboardView.h"
#include "../ThemeManager.h"
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

    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &DashboardView::applyTheme);
    applyTheme();
    refreshTelemetry();
}

void DashboardView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // Title Section
    m_titleLabel = new QLabel("System & Infrastructure Dashboard", this);
    mainLayout->addWidget(m_titleLabel);

    m_subTitleLabel = new QLabel("Real-time telemetry, resource metrics, and service status overview.", this);
    mainLayout->addWidget(m_subTitleLabel);

    // Cards Grid
    auto* gridLayout = new QGridLayout();
    gridLayout->setSpacing(16);

    // CPU Group
    m_cpuGroup = new QGroupBox("CPU Utilization", this);
    auto* cpuLayout = new QVBoxLayout(m_cpuGroup);
    m_cpuLabel = new QLabel("0.0 %", m_cpuGroup);
    m_cpuLabel->setStyleSheet("font-size: 24px; font-weight: 700; color: #3B82F6;");
    m_cpuProgress = new QProgressBar(m_cpuGroup);
    m_cpuProgress->setRange(0, 100);
    m_cpuProgress->setTextVisible(false);
    m_cpuProgress->setFixedHeight(8);
    cpuLayout->addWidget(m_cpuLabel);
    cpuLayout->addWidget(m_cpuProgress);
    gridLayout->addWidget(m_cpuGroup, 0, 0);

    // Memory Group
    m_memGroup = new QGroupBox("Physical Memory (RAM)", this);
    auto* memLayout = new QVBoxLayout(m_memGroup);
    m_memLabel = new QLabel("0.0 GB / 0.0 GB", m_memGroup);
    m_memLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #10B981;");
    m_memProgress = new QProgressBar(m_memGroup);
    m_memProgress->setRange(0, 100);
    m_memProgress->setTextVisible(false);
    m_memProgress->setFixedHeight(8);
    memLayout->addWidget(m_memLabel);
    memLayout->addWidget(m_memProgress);
    gridLayout->addWidget(m_memGroup, 0, 1);

    // Swap / Paging Group
    m_swapGroup = new QGroupBox("Swap / Pagefile", this);
    auto* swapLayout = new QVBoxLayout(m_swapGroup);
    m_swapLabel = new QLabel("0.0 GB / 0.0 GB", m_swapGroup);
    swapLayout->addWidget(m_swapLabel);
    gridLayout->addWidget(m_swapGroup, 1, 0);

    // Host Uptime Group
    m_uptimeGroup = new QGroupBox("Host Uptime", this);
    auto* uptimeLayout = new QVBoxLayout(m_uptimeGroup);
    m_uptimeLabel = new QLabel("0h 0m 0s", m_uptimeGroup);
    uptimeLayout->addWidget(m_uptimeLabel);
    gridLayout->addWidget(m_uptimeGroup, 1, 1);

    mainLayout->addLayout(gridLayout);

    // Services summary banner
    m_summaryGroup = new QGroupBox("Managed Services Summary", this);
    auto* summaryLayout = new QVBoxLayout(m_summaryGroup);
    m_servicesSummaryLabel = new QLabel("Scanning registered services...", m_summaryGroup);
    summaryLayout->addWidget(m_servicesSummaryLabel);
    mainLayout->addWidget(m_summaryGroup);

    mainLayout->addStretch();
}

void DashboardView::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    bool dark = mgr.isDark();

    m_titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1;").arg(mgr.titleColor()));
    m_subTitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(mgr.subtitleColor()));

    QString gbStyle = mgr.groupboxStyle();
    m_cpuGroup->setStyleSheet(gbStyle);
    m_memGroup->setStyleSheet(gbStyle);
    m_swapGroup->setStyleSheet(gbStyle);
    m_uptimeGroup->setStyleSheet(gbStyle);
    m_summaryGroup->setStyleSheet(gbStyle);

    if (dark) {
        m_cpuProgress->setStyleSheet("QProgressBar { background: #334155; border-radius: 4px; } QProgressBar::chunk { background: #3B82F6; border-radius: 4px; }");
        m_memProgress->setStyleSheet("QProgressBar { background: #334155; border-radius: 4px; } QProgressBar::chunk { background: #10B981; border-radius: 4px; }");
        m_swapLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #F1F5F9;");
        m_uptimeLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #F1F5F9;");
        m_servicesSummaryLabel->setStyleSheet("font-size: 13px; color: #E2E8F0; font-weight: 500;");
    } else {
        m_cpuProgress->setStyleSheet("QProgressBar { background: #E2E8F0; border-radius: 4px; } QProgressBar::chunk { background: #2563EB; border-radius: 4px; }");
        m_memProgress->setStyleSheet("QProgressBar { background: #E2E8F0; border-radius: 4px; } QProgressBar::chunk { background: #0D9488; border-radius: 4px; }");
        m_swapLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #0F172A;");
        m_uptimeLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #0F172A;");
        m_servicesSummaryLabel->setStyleSheet("font-size: 13px; color: #334155; font-weight: 500;");
    }
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

