#include "MainWindow.h"
#include "ThemeManager.h"
#include "views/DashboardView.h"
#include "views/DatabasesView.h"
#include "views/LogsView.h"
#include "views/NetworkView.h"
#include "views/ProcessesView.h"
#include "views/ServicesView.h"
#include "views/SettingsView.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

namespace space2x::ui {

MainWindow::MainWindow(std::unique_ptr<core::Engine> engine, QWidget* parent)
    : QMainWindow(parent),
      m_engine(std::move(engine)) {
    setupUi();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &MainWindow::applyTheme);
    applyTheme();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi() {
    setWindowTitle("Space2X - Infrastructure Control Plane");
    resize(1120, 740);

    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto* layout = new QHBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Sidebar Container
    m_sidebarContainer = new QWidget(this);
    m_sidebarContainer->setFixedWidth(240);

    auto* sidebarLayout = new QVBoxLayout(m_sidebarContainer);
    sidebarLayout->setContentsMargins(16, 20, 16, 16);
    sidebarLayout->setSpacing(12);

    auto* headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(10);

    auto* logoLabel = new QLabel(m_sidebarContainer);
    QPixmap logoPix(":/resources/app_icon.png");
    if (!logoPix.isNull()) {
        logoLabel->setPixmap(logoPix.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    headerLayout->addWidget(logoLabel);

    auto* titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(2);
    auto* brandLabel = new QLabel("Space2X", m_sidebarContainer);
    brandLabel->setStyleSheet("color: #FFFFFF; font-size: 17px; font-weight: 800;");
    auto* brandSub = new QLabel("Infrastructure Manager", m_sidebarContainer);
    brandSub->setStyleSheet("color: #94A3B8; font-size: 11px; font-weight: 500;");
    titleLayout->addWidget(brandLabel);
    titleLayout->addWidget(brandSub);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();
    sidebarLayout->addLayout(headerLayout);

    m_sidebar = new QListWidget(m_sidebarContainer);
    m_sidebar->addItem("Dashboard");
    m_sidebar->addItem("Services");
    m_sidebar->addItem("Databases");
    m_sidebar->addItem("Processes");
    m_sidebar->addItem("Network");
    m_sidebar->addItem("Audit Journal");
    m_sidebar->addItem("Settings");
    m_sidebar->setCurrentRow(0);

    connect(m_sidebar, &QListWidget::currentRowChanged, this, &MainWindow::onSidebarRowChanged);
    sidebarLayout->addWidget(m_sidebar);
    sidebarLayout->addStretch();

    // Theme Toggle Button
    m_themeBtn = new QPushButton(m_sidebarContainer);
    m_themeBtn->setCursor(Qt::PointingHandCursor);
    connect(m_themeBtn, &QPushButton::clicked, this, &MainWindow::onToggleTheme);
    sidebarLayout->addWidget(m_themeBtn);

    layout->addWidget(m_sidebarContainer);

    // Stacked Widget Views
    m_stack = new QStackedWidget(this);

    m_stack->addWidget(new DashboardView(*m_engine, this));
    m_stack->addWidget(new ServicesView(*m_engine, this));
    m_stack->addWidget(new DatabasesView(*m_engine, this));
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

void MainWindow::onToggleTheme() {
    ThemeManager::instance().toggleTheme();
}

void MainWindow::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    bool dark = mgr.isDark();

    // Main window and stack styling
    setStyleSheet(QString("QMainWindow { background-color: %1; }").arg(mgr.windowBackground()));
    m_stack->setStyleSheet(QString("QStackedWidget { background-color: %1; }").arg(mgr.contentBackground()));

    // Sidebar styling
    if (dark) {
        m_sidebarContainer->setStyleSheet("background-color: #0B0F19; border-right: 1px solid #1E293B;");
        m_sidebar->setStyleSheet(
            "QListWidget { background: transparent; border: none; outline: none; }"
            "QListWidget::item {"
            "  color: #94A3B8;"
            "  font-size: 13px;"
            "  font-weight: 500;"
            "  padding: 10px 12px;"
            "  border-radius: 6px;"
            "  margin-bottom: 3px;"
            "}"
            "QListWidget::item:hover {"
            "  background-color: #1E293B;"
            "  color: #F8FAFC;"
            "}"
            "QListWidget::item:selected {"
            "  background-color: #2563EB;"
            "  color: #FFFFFF;"
            "  font-weight: 600;"
            "}"
        );
        m_themeBtn->setText("☀️ Switch to Light Mode");
        m_themeBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #1E293B;"
            "  color: #F1F5F9;"
            "  border: 1px solid #334155;"
            "  border-radius: 6px;"
            "  padding: 8px 12px;"
            "  font-size: 12px;"
            "  font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "  background-color: #334155;"
            "}"
        );
    } else {
        m_sidebarContainer->setStyleSheet("background-color: #0F172A; border-right: 1px solid #E2E8F0;");
        m_sidebar->setStyleSheet(
            "QListWidget { background: transparent; border: none; outline: none; }"
            "QListWidget::item {"
            "  color: #94A3B8;"
            "  font-size: 13px;"
            "  font-weight: 500;"
            "  padding: 10px 12px;"
            "  border-radius: 6px;"
            "  margin-bottom: 3px;"
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
        m_themeBtn->setText("🌙 Switch to Dark Mode");
        m_themeBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #1E293B;"
            "  color: #F8FAFC;"
            "  border: 1px solid #334155;"
            "  border-radius: 6px;"
            "  padding: 8px 12px;"
            "  font-size: 12px;"
            "  font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "  background-color: #334155;"
            "}"
        );
    }
}

} // namespace space2x::ui

