#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>

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

private:
    void setupUi();
    void terminateSelectedProcess();

    core::Engine& m_engine;
    QLineEdit*    m_searchEdit{nullptr};
    QTableWidget* m_table{nullptr};
};

} // namespace space2x::ui
