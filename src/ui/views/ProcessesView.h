#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>

namespace space2x::core {
class Engine;
}

namespace space2x::ui {

class ProcessesView : public QWidget {
    Q_OBJECT

public:
    explicit ProcessesView(core::Engine& engine, QWidget* parent = nullptr);
    ~ProcessesView() override = default;

public slots:
    void refreshProcesses();
    void filterChanged(const QString& text);
    void applyTheme();

private:
    void setupUi();
    void terminateSelectedProcess();

    core::Engine& m_engine;
    QLabel*       m_titleLabel{nullptr};
    QLabel*       m_subTitleLabel{nullptr};
    QLineEdit*    m_searchEdit{nullptr};
    QPushButton*  m_terminateBtn{nullptr};
    QPushButton*  m_refreshBtn{nullptr};
    QTableWidget* m_table{nullptr};
};

} // namespace space2x::ui
