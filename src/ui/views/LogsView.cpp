#include "LogsView.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace space2x::ui {

LogsView::LogsView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    refreshLogs();
}

void LogsView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    auto* titleLabel = new QLabel("Audit & Operation Journal", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    titleLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Immutable chronological record of all service lifecycle and configuration actions.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    titleLayout->addWidget(subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    auto* refreshBtn = new QPushButton("Refresh Journal", this);
    refreshBtn->setStyleSheet("padding: 6px 14px; border: 1px solid #CBD5E1; border-radius: 4px; background: #FFFFFF; font-weight: 500;");
    connect(refreshBtn, &QPushButton::clicked, this, &LogsView::refreshLogs);
    headerLayout->addWidget(refreshBtn);

    mainLayout->addLayout(headerLayout);

    m_logText = new QTextEdit(this);
    m_logText->setReadOnly(true);
    m_logText->setStyleSheet(
        "QTextEdit {"
        "  background-color: #0F172A;"
        "  color: #E2E8F0;"
        "  font-family: 'Consolas', 'Courier New', monospace;"
        "  font-size: 12px;"
        "  border-radius: 8px;"
        "  padding: 12px;"
        "  line-height: 1.5;"
        "}"
    );
    mainLayout->addWidget(m_logText);
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
