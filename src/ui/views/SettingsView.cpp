#include "SettingsView.h"
#include <space2x/core/Engine.h>

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>

namespace space2x::ui {

SettingsView::SettingsView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
}

void SettingsView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);

    // Header
    auto* titleLabel = new QLabel("System & Application Settings", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    mainLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Environment detection, audit paths, and runtime metadata.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    mainLayout->addWidget(subTitleLabel);

    // Group: System Environment
    auto* envGroup = new QGroupBox("Host Environment & Package Tooling", this);
    envGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* envLayout = new QVBoxLayout(envGroup);

    auto pmRes = m_engine.packageManager().detectPackageManager();
    QString pmText = pmRes.isOk() ? QString::fromStdString(pmRes.value()).toUpper() : "Not Detected / Manual";

    auto* pmLabel = new QLabel("Detected Package Manager: " + pmText, envGroup);
    pmLabel->setStyleSheet("font-size: 13px; color: #334155;");
    envLayout->addWidget(pmLabel);

    auto* auditPathLabel = new QLabel("Audit Journal File: " + QString::fromStdString(m_engine.auditLog().logFilePath()), envGroup);
    auditPathLabel->setStyleSheet("font-size: 13px; color: #334155;");
    envLayout->addWidget(auditPathLabel);

    mainLayout->addWidget(envGroup);

    // Group: About
    auto* aboutGroup = new QGroupBox("About Space2X", this);
    aboutGroup->setStyleSheet("QGroupBox { font-weight: 600; border: 1px solid #E2E8F0; border-radius: 8px; margin-top: 8px; padding-top: 16px; } QGroupBox::title { subcontrol-origin: margin; left: 12px; }");
    auto* aboutLayout = new QVBoxLayout(aboutGroup);

    auto* verLabel = new QLabel("Version: 0.1.0-alpha (C++20)", aboutGroup);
    verLabel->setStyleSheet("font-size: 13px; color: #334155;");
    aboutLayout->addWidget(verLabel);

    auto* licLabel = new QLabel("License: Apache License, Version 2.0", aboutGroup);
    licLabel->setStyleSheet("font-size: 13px; color: #334155;");
    aboutLayout->addWidget(licLabel);

    mainLayout->addWidget(aboutGroup);
    mainLayout->addStretch();
}

} // namespace space2x::ui
