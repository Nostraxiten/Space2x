#include "ProcessesView.h"
#include "../widgets/ConfirmDialog.h"
#include <space2x/core/Engine.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

namespace space2x::ui {

namespace {

QString formatBytes(uint64_t bytes) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;

    if (bytes >= GB) {
        return QString::number(static_cast<double>(bytes) / GB, 'f', 2) + " GB";
    } else if (bytes >= MB) {
        return QString::number(static_cast<double>(bytes) / MB, 'f', 2) + " MB";
    } else if (bytes >= KB) {
        return QString::number(static_cast<double>(bytes) / KB, 'f', 2) + " KB";
    }
    return QString::number(bytes) + " B";
}

} // anonymous namespace

ProcessesView::ProcessesView(core::Engine& engine, QWidget* parent)
    : QWidget(parent),
      m_engine(engine) {
    setupUi();
    refreshProcesses();
}

void ProcessesView::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Header
    auto* headerLayout = new QHBoxLayout();
    auto* titleLayout = new QVBoxLayout();

    auto* titleLabel = new QLabel("Process Explorer", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 700; color: #0F172A;");
    titleLayout->addWidget(titleLabel);

    auto* subTitleLabel = new QLabel("Inspect active system processes, resident memory consumption, and binaries.", this);
    subTitleLabel->setStyleSheet("font-size: 13px; color: #64748B;");
    titleLayout->addWidget(subTitleLabel);

    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Filter processes by name...");
    m_searchEdit->setFixedWidth(220);
    m_searchEdit->setStyleSheet("padding: 6px 12px; border: 1px solid #CBD5E1; border-radius: 4px;");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ProcessesView::filterChanged);
    headerLayout->addWidget(m_searchEdit);

    auto* terminateBtn = new QPushButton("End Process", this);
    terminateBtn->setStyleSheet("padding: 6px 14px; background: #DC2626; color: white; border-radius: 4px; font-weight: 600;");
    connect(terminateBtn, &QPushButton::clicked, this, &ProcessesView::terminateSelectedProcess);
    headerLayout->addWidget(terminateBtn);

    auto* refreshBtn = new QPushButton("Refresh", this);
    refreshBtn->setStyleSheet("padding: 6px 14px; border: 1px solid #CBD5E1; border-radius: 4px; background: #FFFFFF; font-weight: 500;");
    connect(refreshBtn, &QPushButton::clicked, this, &ProcessesView::refreshProcesses);
    headerLayout->addWidget(refreshBtn);

    mainLayout->addLayout(headerLayout);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"PID", "PPID", "Process Name", "Memory", "Executable Path"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setStyleSheet(
        "QTableWidget { border: 1px solid #E2E8F0; border-radius: 8px; background: #FFFFFF; gridline-color: #F1F5F9; }"
        "QHeaderView::section { background: #F8FAFC; padding: 8px; font-weight: 600; border: none; border-bottom: 1px solid #E2E8F0; }"
    );

    mainLayout->addWidget(m_table);
}

void ProcessesView::refreshProcesses() {
    auto res = m_engine.processManager().listProcesses();
    if (!res.isOk()) {
        QMessageBox::critical(this, "Error", QString::fromStdString(res.error().toString()));
        return;
    }

    const auto& processes = res.value();
    m_table->setRowCount(static_cast<int>(processes.size()));

    for (int row = 0; row < static_cast<int>(processes.size()); ++row) {
        const auto& p = processes[static_cast<size_t>(row)];

        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(p.pid)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(p.parentPid)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p.name)));
        m_table->setItem(row, 3, new QTableWidgetItem(formatBytes(p.memoryBytes)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(p.executablePath)));
    }

    filterChanged(m_searchEdit->text());
}

void ProcessesView::filterChanged(const QString& text) {
    for (int row = 0; row < m_table->rowCount(); ++row) {
        auto* nameItem = m_table->item(row, 2);
        if (!nameItem) continue;

        bool match = text.isEmpty() || nameItem->text().contains(text, Qt::CaseInsensitive);
        m_table->setRowHidden(row, !match);
    }
}

void ProcessesView::terminateSelectedProcess() {
    auto selected = m_table->selectedItems();
    if (selected.empty()) {
        QMessageBox::information(this, "Notice", "Please select a process in the table to terminate.");
        return;
    }

    int row = selected.first()->row();
    uint32_t pid = m_table->item(row, 0)->text().toUInt();
    QString name = m_table->item(row, 2)->text();

    if (!ConfirmDialog::confirm(
            "Terminate Process",
            QString("Are you sure you want to terminate process '%1' (PID: %2)?").arg(name).arg(pid),
            "Warning: Unsaved data in this application may be lost immediately.",
            this)) {
        return;
    }

    auto res = m_engine.processManager().killProcess(pid);
    if (!res.isOk()) {
        QMessageBox::warning(this, "Termination Failed", QString::fromStdString(res.error().toString()));
    } else {
        refreshProcesses();
    }
}

} // namespace space2x::ui
