#include "ConfirmDialog.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace space2x::ui {

ConfirmDialog::ConfirmDialog(const QString& title,
                             const QString& message,
                             const QString& details,
                             QWidget* parent)
    : QDialog(parent) {
    setWindowTitle(title);
    setMinimumWidth(480);
    setModal(true);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    auto* titleLabel = new QLabel(title, this);
    titleLabel->setStyleSheet("font-size: 15px; font-weight: 600; color: #1E293B;");
    mainLayout->addWidget(titleLabel);

    auto* messageLabel = new QLabel(message, this);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet("font-size: 13px; color: #475569;");
    mainLayout->addWidget(messageLabel);

    if (!details.isEmpty()) {
        auto* detailsBox = new QTextEdit(this);
        detailsBox->setPlainText(details);
        detailsBox->setReadOnly(true);
        detailsBox->setStyleSheet("background-color: #F8FAFC; border: 1px solid #E2E8F0; font-family: monospace; font-size: 12px; padding: 8px;");
        detailsBox->setMaximumHeight(120);
        mainLayout->addWidget(detailsBox);
    }

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    auto* cancelBtn = new QPushButton("Cancel", this);
    cancelBtn->setStyleSheet("padding: 6px 16px; border: 1px solid #CBD5E1; border-radius: 4px; background: #FFFFFF; font-weight: 500;");
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);

    auto* confirmBtn = new QPushButton("Confirm", this);
    confirmBtn->setStyleSheet("padding: 6px 16px; border-radius: 4px; background: #2563EB; color: #FFFFFF; font-weight: 600;");
    connect(confirmBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(confirmBtn);

    mainLayout->addLayout(buttonLayout);
}

bool ConfirmDialog::confirm(const QString& title,
                            const QString& message,
                            const QString& details,
                            QWidget* parent) {
    ConfirmDialog dlg(title, message, details, parent);
    return dlg.exec() == QDialog::Accepted;
}

} // namespace space2x::ui
