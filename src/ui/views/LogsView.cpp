#include "LogsView.h"
#include "../ThemeManager.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace space2x::ui {

LogsView::LogsView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this, &LogsView::applyTheme);
    applyTheme();
    refreshLogs();
}

void LogsView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    m_titleLabel = new QLabel("Audit & Operation Journal", this);
    titleLayout->addWidget(m_titleLabel);

    m_subTitleLabel = new QLabel("Immutable chronological record of all service lifecycle and configuration actions.", this);
    titleLayout->addWidget(m_subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_refreshBtn = new QPushButton("Refresh Journal", this);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    connect(m_refreshBtn, &QPushButton::clicked, this, &LogsView::refreshLogs);
    headerLayout->addWidget(m_refreshBtn);

    mainLayout->addLayout(headerLayout);

    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    mainLayout->addWidget(m_logText);
}

void LogsView::applyTheme() {
    const auto& mgr = ThemeManager::instance();
    m_titleLabel->setStyleSheet(QString("font-size: 20px; font-weight: 800; color: %1;").arg(mgr.titleColor()));
    m_subTitleLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(mgr.subtitleColor()));
    m_refreshBtn->setStyleSheet(mgr.secondaryButtonStyle());

    if (mgr.isDark()) {
        m_logText->setStyleSheet(
            "QTextEdit {"
            "  background-color: #030712;"
            "  color: #4ADE80;"
            "  border: 1px solid #1F2937;"
            "  border-radius: 8px;"
            "  padding: 12px;"
            "  font-family: 'Consolas', 'Courier New', monospace;"
            "  font-size: 12px;"
            "  line-height: 1.5;"
            "}"
        );
    } else {
        m_logText->setStyleSheet(
            "QTextEdit {"
            "  background-color: #0F172A;"
            "  color: #F8FAFC;"
            "  border: 1px solid #1E293B;"
            "  border-radius: 8px;"
            "  padding: 12px;"
            "  font-family: 'Consolas', 'Courier New', monospace;"
            "  font-size: 12px;"
            "  line-height: 1.5;"
            "}"
        );
    }
}


void LogsView::refreshLogs() {
    auto events = m_engine.auditLog().getRecentEvents(200);
    QString text;
    for (const auto& ev : events) {
        text += QString("[%1] %2 | TARGET: %3 | OUTCOME: %4")
                    .arg(QString::fromStdString(ev.timestamp))
                    .arg(QString::fromStdString(ev.action))
                    .arg(QString::fromStdString(ev.targetId))
                    .arg(QString::fromStdString(ev.outcome));
        if (!ev.detail.empty()) {
            text += " | DETAIL: " + QString::fromStdString(ev.detail);
        }
        text += "\n";
    }

    if (text.isEmpty()) {
        text = "# No audit events recorded in the current session.\n";
    }

    m_logText->setPlainText(text);
    m_logText->moveCursor(QTextCursor::End);
}

} // namespace space2x::ui
