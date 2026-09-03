#pragma once

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class SettingsView : public QWidget {
    Q_OBJECT

public:
    explicit SettingsView(core::Engine& engine, QWidget* parent = nullptr);
    ~SettingsView() override = default;

public slots:
    void applyTheme();

private:
    void setupUi();

    core::Engine& m_engine;
    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};
    QGroupBox*    m_themeGroup{nullptr};
    QLabel*       m_themeDescLabel{nullptr};
    QPushButton*  m_themeToggleBtn{nullptr};
    QGroupBox*    m_envGroup{nullptr};
    QLabel*       m_pmLabel{nullptr};
    QLabel*       m_auditPathLabel{nullptr};
    QGroupBox*    m_aboutGroup{nullptr};
    QLabel*       m_verLabel{nullptr};
    QLabel*       m_licLabel{nullptr};
};

} // namespace space2x::ui
