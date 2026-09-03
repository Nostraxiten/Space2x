#include "NetworkView.h"
#include "../ThemeManager.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace space2x::ui {

NetworkView::NetworkView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &NetworkView::applyTheme);
    applyTheme();
    refreshNetwork();
}

void NetworkView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    m_titleLabel = new QLabel("Network Diagnostics & Listening Endpoints", this);
    titleLayout->addWidget(m_titleLabel);

    m_subTitleLabel = new QLabel("Inspect physical/virtual network adapters, IP routing, and bound TCP ports.", this);
    titleLayout->addWidget(m_subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_refreshBtn = new QPushButton("Refresh", this);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(m_refreshBtn, &QPushButton::clicked, this, &NetworkView::refreshNetwork);
    headerLayout->addWidget(m_refreshBtn);

    mainLayout->addLayout(headerLayout);

    // Interfaces Title
    m_ifaceTitle = new QLabel("Network Adapters", this);
    mainLayout->addWidget(m_ifaceTitle);

    m_ifaceTable = new QTableWidget(this);
    m_ifaceTable->setColumnCount(5);
    m_ifaceTable->setHorizontalHeaderLabels({"Adapter Name / Description", "Status", "MAC Address", "IPv4 Address", "Default Gateway"});
    m_ifaceTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_ifaceTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_ifaceTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_ifaceTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_ifaceTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_ifaceTable->verticalHeader()->setVisible(false);
    m_ifaceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ifaceTable->setMaximumHeight(160);
    mainLayout->addWidget(m_ifaceTable);

    // Sockets Title
    m_socketsTitle = new QLabel("Listening Ports (Local Sockets)", this);
    mainLayout->addWidget(m_socketsTitle);

    m_socketsTable = new QTableWidget(this);
    m_socketsTable->setColumnCount(4);
    m_socketsTable->setHorizontalHeaderLabels({"Protocol", "Local Address", "Port", "Owning PID"});
    m_socketsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_socketsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_socketsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_socketsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_socketsTable->verticalHeader()->setVisible(false);
    m_socketsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(m_socketsTable);
}

void NetworkView::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    m_titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1;").arg(mgr.titleColor()));
    m_subTitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(mgr.subtitleColor()));
    m_ifaceTitle->setStyleSheet(QString("font-size: 15px; font-weight: 700; color: %1; margin-top: 8px;").arg(mgr.isDark() ? "#93C5FD" : "#1E40AF"));
    m_socketsTitle->setStyleSheet(QString("font-size: 15px; font-weight: 700; color: %1; margin-top: 12px;").arg(mgr.isDark() ? "#93C5FD" : "#1E40AF"));
    m_refreshBtn->setStyleSheet(mgr.secondaryButtonStyle());
    m_ifaceTable->setStyleSheet(mgr.tableStyle());
    m_socketsTable->setStyleSheet(mgr.tableStyle());
}


void NetworkView::refreshNetwork() {
    auto ifacesRes = m_engine.networkManager().getInterfaces();
    if (ifacesRes.isOk()) {
        const auto& ifaces = ifacesRes.value();
        m_ifaceTable->setRowCount(static_cast<int>(ifaces.size()));
        for (int i = 0; i < static_cast<int>(ifaces.size()); ++i) {
            const auto& iface = ifaces[static_cast<size_t>(i)];
            QString desc = iface.description.empty() ? QString::fromStdString(iface.name) : QString::fromStdString(iface.description);
            m_ifaceTable->setItem(i, 0, new QTableWidgetItem(desc));
            m_ifaceTable->setItem(i, 1, new QTableWidgetItem(iface.isUp ? "UP" : "DOWN"));
            m_ifaceTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(iface.macAddress)));

            QString ips;
            for (const auto& ip : iface.ipv4Addresses) {
                if (!ips.isEmpty()) ips += ", ";
                ips += QString::fromStdString(ip);
            }
            m_ifaceTable->setItem(i, 3, new QTableWidgetItem(ips));
            m_ifaceTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(iface.defaultGateway)));
        }
    }

    auto socketsRes = m_engine.networkManager().getListeningSockets();
    if (socketsRes.isOk()) {
        const auto& sockets = socketsRes.value();
        m_socketsTable->setRowCount(static_cast<int>(sockets.size()));
        for (int i = 0; i < static_cast<int>(sockets.size()); ++i) {
            const auto& sock = sockets[static_cast<size_t>(i)];
            m_socketsTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(sock.protocol).toUpper()));
            m_socketsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(sock.localAddress)));
            m_socketsTable->setItem(i, 2, new QTableWidgetItem(QString::number(sock.localPort)));
            m_socketsTable->setItem(i, 3, new QTableWidgetItem(QString::number(sock.pid)));
        }
    }
}

} // namespace space2x::ui
