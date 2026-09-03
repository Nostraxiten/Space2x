#include "DatabasesView.h"
#include "../ThemeManager.h"
#include "../widgets/StatusBadge.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QVBoxLayout>

namespace space2x::ui {

DatabasesView::DatabasesView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &DatabasesView::applyTheme);
    applyTheme();
    refreshDatabases();
}

void DatabasesView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    m_titleLabel = new QLabel("Database Engines & Local Instances", this);
    titleLayout->addWidget(m_titleLabel);

    m_subTitleLabel = new QLabel("Monitor local database servers, default listening ports, and status.", this);
    titleLayout->addWidget(m_subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(m_refreshBtn, &QPushButton::clicked, this, &DatabasesView::refreshDatabases);
    headerLayout->addWidget(m_refreshBtn);

    mainLayout->addLayout(headerLayout);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"Engine", "Service Name", "Default Port", "Status", "Health Check"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(m_table);
}

void DatabasesView::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    m_titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1;").arg(mgr.titleColor()));
    m_subTitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(mgr.subtitleColor()));
    m_refreshBtn->setStyleSheet(mgr.secondaryButtonStyle());
    m_table->setStyleSheet(mgr.tableStyle());
}


void DatabasesView::refreshDatabases() {
    std::vector<std::string> dbServices = {"postgresql", "mysql", "redis"};
    m_table->setRowCount(static_cast<int>(dbServices.size()));

    for (int i = 0; i < static_cast<int>(dbServices.size()); ++i) {
        const auto& id = dbServices[static_cast<size_t>(i)];
        auto prov = m_engine.providerRegistry().find(id);
        if (!prov) continue;

        const auto& m = prov->manifest();
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m.displayName)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m.serviceNameForCurrentPlatform())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(m.defaultPort)));

        auto stateRes = m_engine.serviceController().getServiceState(id);
        auto* badge = new StatusBadge(this);
        if (stateRes.isOk()) {
            badge->setState(stateRes.value());
        } else {
            badge->setState(core::ServiceState::Unknown);
        }

        auto* badgeContainer = new QWidget(this);
        auto* badgeLayout = new QHBoxLayout(badgeContainer);
        badgeLayout->setContentsMargins(4, 4, 4, 4);
        badgeLayout->addWidget(badge);
        m_table->setCellWidget(i, 3, badgeContainer);

        auto healthRes = m_engine.serviceController().performHealthCheck(id);
        QString healthText = healthRes.isOk() ? "PASS" : "FAIL / INACTIVE";
        QString healthColor = healthRes.isOk() ? "#16A34A" : "#64748B";

        auto* healthLabel = new QLabel(healthText, this);
        healthLabel->setStyleSheet(QString("font-weight: 600; color: %1; padding: 4px;").arg(healthColor));
        m_table->setCellWidget(i, 4, healthLabel);
    }
}

} // namespace space2x::ui
