#pragma once

#include <QDialog>
#include <QString>

namespace space2x::ui {

class ConfirmDialog : public QDialog {
    Q_OBJECT

public:
    ConfirmDialog(const QString& title,
                  const QString& message,
                  const QString& details = "",
                  QWidget* parent = nullptr);

    static bool confirm(const QString& title,
                        const QString& message,
                        const QString& details = "",
                        QWidget* parent = nullptr);
};

} // namespace space2x::ui
