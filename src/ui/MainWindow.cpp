#include "MainWindow.h"
#include "views/DashboardView.h"
#include "views/LogsView.h"
#include "views/NetworkView.h"
#include "views/ProcessesView.h"
#include "views/ServicesView.h"
#include "views/SettingsView.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

namespace space2x::ui {

MainWindow::MainWindow(std::unique_ptr<core::Engine> engine, QWidget* parent)
    : QMainWindow(parent),
      m_engine(std::move(engine)) {
    setupUi();
}

void MainWindow::setupUi() {
    setWindowTitle("Space2X - Infrastructure Control Plane");
    resize(1100, 720);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QHBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Sidebar Container
    auto* sidebarContainer = new QWidget(this);
    sidebarContainer->setFixedWidth(230);
    sidebarContainer->setStyleSheet("background-color: #0F172A; border-right: 1px solid #1E293B;");

    auto* sidebarLayout = new QVBoxLayout(sidebarContainer);
    sidebarLayout->setContentsMargins(16, 20, 16, 20);
    sidebarLayout->setSpacing(12);

    auto* brandLabel = new QLabel("Space2X", sidebarContainer);
    brandLabel->setStyleSheet("color: #FFFFFF; font-size: 18px; font-weight: 800; padding-left: 8px;");
    sidebarLayout->addWidget(brandLabel);

    auto* brandSub = new QLabel("Infrastructure Manager", sidebarContainer);
    brandSub->setStyleSheet("color: #94A3B8; font-size: 11px; font-weight: 500; padding-left: 8px; margin-bottom: 8px;");
    sidebarLayout->addWidget(brandSub);

    m_sidebar = new QListWidget(sidebarContainer);
    m_sidebar->addItem("Dashboard");
    m_sidebar->addItem("Services");
    m_sidebar->addItem("Processes");
    m_sidebar->addItem("Network");
    m_sidebar->addItem("Audit Journal");
    m_sidebar->addItem("Settings");
    m_sidebar->setCurrentRow(0);

    m_sidebar->setStyleSheet(
        "QListWidget { background: transparent; border: none; outline: none; }"
        "QListWidget::item {"
        "  color: #CBD5E1;"
        "  font-size: 13px;"
        "  font-weight: 500;"
        "  padding: 10px 12px;"
        "  border-radius: 6px;"
        "  margin-bottom: 2px;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #1E293B;"
        "  color: #FFFFFF;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #2563EB;"
        "  color: #FFFFFF;"
        "  font-weight: 600;"
        "}"
    );

    connect(m_sidebar, &QListWidget::currentRowChanged, this, &MainWindow::onSidebarRowChanged);
    sidebarLayout->addWidget(m_sidebar);
    sidebarLayout->addStretch();

    layout->addWidget(sidebarContainer);

    // Stacked Widget Views
    m_stack = new QStackedWidget(this);
    m_stack->setStyleSheet("background-color: #F8FAFC;");

    m_stack->addWidget(new DashboardView(*m_engine, this));
    m_stack->addWidget(new ServicesView(*m_engine, this));
    m_stack->addWidget(new ProcessesView(*m_engine, this));
    m_stack->addWidget(new NetworkView(*m_engine, this));
    m_stack->addWidget(new LogsView(*m_engine, this));
    m_stack->addWidget(new SettingsView(*m_engine, this));

    layout->addWidget(m_stack);
}

void MainWindow::onSidebarRowChanged(int row) {
    if (row >= 0 && row < m_stack->count()) {
        m_stack->setCurrentIndex(row);
    }
}

} // namespace space2x::ui
