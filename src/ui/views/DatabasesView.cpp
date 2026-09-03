#include "DatabasesView.h"
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
    refreshDatabases();
}

void DatabasesView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    auto* titleLabel = new QLabel("Database Engines & Local Instances", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    titleLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Monitor local database servers, default listening ports, and status.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    titleLayout->addWidget(subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    auto* refreshBtn = new QPushButton("Refresh", this);
    refreshBtn->setStyleSheet("padding: 6px 14px; border: 1px solid #CBD5E1; border-radius: 4px; background: #FFFFFF; font-weight: 500;");
    connect(refreshBtn, &QPushButton::clicked, this, &DatabasesView::refreshDatabases);
    headerLayout->addWidget(refreshBtn);

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
    m_table->setStyleSheet(
        "QTableWidget { border: 1px solid #E2E8F0; border-radius: 8px; background: #FFFFFF; }"
        "QHeaderView::section { background: #F8FAFC; padding: 8px; font-weight: 600; border: none; border-bottom: 1px solid #E2E8F0; }"
    );
    mainLayout->addWidget(m_table);
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
