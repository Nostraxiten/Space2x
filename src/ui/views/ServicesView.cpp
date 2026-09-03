#include "ServicesView.h"
#include "../widgets/ConfirmDialog.h"
#include "../widgets/StatusBadge.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

namespace space2x::ui {

ServicesView::ServicesView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    refreshServices();
}

void ServicesView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    auto* titleLabel = new QLabel("Infrastructure Services", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    titleLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Manage, configure, start, stop, and inspect local development services.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    titleLayout->addWidget(subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    auto* refreshBtn = new QPushButton("Refresh", this);
    refreshBtn->setStyleSheet("padding: 6px 16px; border: 1px solid #CBD5E1; border-radius: 4px; background: #FFFFFF; font-weight: 500;");
    connect(refreshBtn, &QPushButton::clicked, this, &ServicesView::refreshServices);
    headerLayout->addWidget(refreshBtn);

    mainLayout->addLayout(headerLayout);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"Service ID", "Name", "Category", "Port", "Status", "Actions"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setStyleSheet(
        "QTableWidget { border: 1px solid #E2E8F0; border-radius: 8px; background: #FFFFFF; gridline-color: #F1F5F9; }"
        "QHeaderView::section { background: #F8FAFC; padding: 8px; font-weight: 600; border: none; border-bottom: 1px solid #E2E8F0; }"
    );

    mainLayout->addWidget(m_table);
}

void ServicesView::refreshServices() {
    auto res = m_engine.serviceController().listServices();
    if (!res.isOk()) {
        QMessageBox::critical(this, "Error", QString::fromStdString(res.error().toString()));
        return;
    }

    const auto& services = res.value();
    m_table->setRowCount(static_cast<int>(services.size()));

    for (int row = 0; row < static_cast<int>(services.size()); ++row) {
        const auto& s = services[static_cast<size_t>(row)];

        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(s.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s.displayName)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.category)));
        m_table->setItem(row, 3, new QTableWidgetItem(s.port > 0 ? QString::number(s.port) : "-"));

        // Status badge
        auto* badge = new StatusBadge(this);
        badge->setState(s.state);
        auto* badgeContainer = new QWidget(this);
        auto* badgeLayout = new QHBoxLayout(badgeContainer);
        badgeLayout->setContentsMargins(4, 4, 4, 4);
        badgeLayout->addWidget(badge);
        m_table->setCellWidget(row, 4, badgeContainer);

        // Actions container
        auto* actionsWidget = new QWidget(this);
        auto* actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(4, 2, 4, 2);
        actionsLayout->setSpacing(6);

        std::string serviceId = s.id;

        auto* startBtn = new QPushButton("Start", actionsWidget);
        startBtn->setStyleSheet("padding: 3px 8px; background: #16A34A; color: white; border-radius: 3px; font-weight: 500; font-size: 11px;");
        connect(startBtn, &QPushButton::clicked, this, [this, serviceId]() { handleStart(serviceId); });
        actionsLayout->addWidget(startBtn);

        auto* stopBtn = new QPushButton("Stop", actionsWidget);
        stopBtn->setStyleSheet("padding: 3px 8px; background: #DC2626; color: white; border-radius: 3px; font-weight: 500; font-size: 11px;");
        connect(stopBtn, &QPushButton::clicked, this, [this, serviceId]() { handleStop(serviceId); });
        actionsLayout->addWidget(stopBtn);

        auto* restartBtn = new QPushButton("Restart", actionsWidget);
        restartBtn->setStyleSheet("padding: 3px 8px; background: #D97706; color: white; border-radius: 3px; font-weight: 500; font-size: 11px;");
        connect(restartBtn, &QPushButton::clicked, this, [this, serviceId]() { handleRestart(serviceId); });
        actionsLayout->addWidget(restartBtn);

        auto* healthBtn = new QPushButton("Health", actionsWidget);
        healthBtn->setStyleSheet("padding: 3px 8px; background: #475569; color: white; border-radius: 3px; font-weight: 500; font-size: 11px;");
        connect(healthBtn, &QPushButton::clicked, this, [this, serviceId]() { handleHealthCheck(serviceId); });
        actionsLayout->addWidget(healthBtn);

        m_table->setCellWidget(row, 5, actionsWidget);
    }
}

void ServicesView::handleStart(const std::string& serviceId) {
    if (!ConfirmDialog::confirm(
            "Start Service",
            QString("Are you sure you want to start service '%1'?").arg(QString::fromStdString(serviceId)),
            "Space2X will request the operating system service manager to launch the daemon.",
            this)) {
        return;
    }

    auto res = m_engine.serviceController().startService(serviceId);
    if (!res.isOk()) {
        QMessageBox::warning(this, "Start Failed", QString::fromStdString(res.error().toString()));
    }
    refreshServices();
}

void ServicesView::handleStop(const std::string& serviceId) {
    if (!ConfirmDialog::confirm(
            "Stop Service",
            QString("Are you sure you want to STOP service '%1'?").arg(QString::fromStdString(serviceId)),
            "Warning: Any active client connections to this service will be terminated.",
            this)) {
        return;
    }

    auto res = m_engine.serviceController().stopService(serviceId);
    if (!res.isOk()) {
        QMessageBox::warning(this, "Stop Failed", QString::fromStdString(res.error().toString()));
    }
    refreshServices();
}

void ServicesView::handleRestart(const std::string& serviceId) {
    if (!ConfirmDialog::confirm(
            "Restart Service",
            QString("Are you sure you want to restart service '%1'?").arg(QString::fromStdString(serviceId)),
            "The service daemon will be cleanly stopped and immediately restarted.",
            this)) {
        return;
    }

    auto res = m_engine.serviceController().restartService(serviceId);
    if (!res.isOk()) {
        QMessageBox::warning(this, "Restart Failed", QString::fromStdString(res.error().toString()));
    }
    refreshServices();
}

void ServicesView::handleHealthCheck(const std::string& serviceId) {
    auto res = m_engine.serviceController().performHealthCheck(serviceId);
    if (res.isOk()) {
        QMessageBox::information(this, "Health Check", QString("Service '%1' health check PASSED.").arg(QString::fromStdString(serviceId)));
    } else {
        QMessageBox::warning(this, "Health Check Failed", QString::fromStdString(res.error().toString()));
    }
}

} // namespace space2x::ui
