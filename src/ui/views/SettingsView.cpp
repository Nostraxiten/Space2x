#include "SettingsView.h"
#include "../ThemeManager.h"
#include <space2x/core/Engine.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace space2x::ui {

SettingsView::SettingsView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &SettingsView::applyTheme);
    applyTheme();
}

void SettingsView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // Header
    m_titleLabel = new QLabel("System & Application Settings", this);
    mainLayout->addWidget(m_titleLabel);

    m_subTitleLabel = new QLabel("Environment detection, appearance mode, audit paths, and runtime metadata.", this);
    mainLayout->addWidget(m_subTitleLabel);

    // Group: Appearance / Theme
    m_themeGroup = new QGroupBox("Appearance & Theme", this);
    auto* themeLayout = new QHBoxLayout(m_themeGroup);
    m_themeDescLabel = new QLabel("Select application visual style and contrast mode:", m_themeGroup);
    themeLayout->addWidget(m_themeDescLabel);
    themeLayout->addStretch();

    m_themeToggleBtn = new QPushButton(m_themeGroup);
    m_themeToggleBtn->setCursor(Qt::PointingHandCursor);
    connect(m_themeToggleBtn, &QPushButton::clicked, this, []() {
        ThemeManager::instance().toggleTheme();
    });
    themeLayout->addWidget(m_themeToggleBtn);
    mainLayout->addWidget(m_themeGroup);

    // Group: System Environment
    m_envGroup = new QGroupBox("Host Environment & Package Tooling", this);
    auto* envLayout = new QVBoxLayout(m_envGroup);

    auto pmRes = m_engine.packageManager().detectPackageManager();
    QString pmText = pmRes.isOk() ? QString::fromStdString(pmRes.value()).toUpper() : "Not Detected / Manual";

    m_pmLabel = new QLabel("Detected Package Manager: " + pmText, m_envGroup);
    envLayout->addWidget(m_pmLabel);

    m_auditPathLabel = new QLabel("Audit Journal File: " + QString::fromStdString(m_engine.auditLog().logFilePath()), m_envGroup);
    envLayout->addWidget(m_auditPathLabel);

    mainLayout->addWidget(m_envGroup);

    // Group: About
    m_aboutGroup = new QGroupBox("About Space2X", this);
    auto* aboutLayout = new QVBoxLayout(m_aboutGroup);

    m_verLabel = new QLabel("Version: 0.1.0-alpha (C++20)", m_aboutGroup);
    aboutLayout->addWidget(m_verLabel);

    m_licLabel = new QLabel("License: Apache License, Version 2.0", m_aboutGroup);
    aboutLayout->addWidget(m_licLabel);

    mainLayout->addWidget(m_aboutGroup);
    mainLayout->addStretch();
}

void SettingsView::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    bool dark = mgr.isDark();

    m_titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1;").arg(mgr.titleColor()));
    m_subTitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(mgr.subtitleColor()));

    QString gbStyle = mgr.groupboxStyle();
    m_themeGroup->setStyleSheet(gbStyle);
    m_envGroup->setStyleSheet(gbStyle);
    m_aboutGroup->setStyleSheet(gbStyle);

    QString textCol = mgr.textColor();
    m_themeDescLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(textCol));
    m_pmLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(textCol));
    m_auditPathLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(textCol));
    m_verLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(textCol));
    m_licLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(textCol));

    m_themeToggleBtn->setText(dark ? "☀️ Switch to Light Theme" : "🌙 Switch to Dark Theme");
    m_themeToggleBtn->setStyleSheet(mgr.secondaryButtonStyle());
}

} // namespace space2x::ui

