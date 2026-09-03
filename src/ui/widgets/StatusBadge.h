#pragma once

#include <QLabel>
#include <space2x/core/ServiceState.h>

namespace space2x::ui {

class StatusBadge : public QLabel {
    Q_OBJECT

public:
    explicit StatusBadge(QWidget* parent = nullptr);
    void setState(core::ServiceState state);

private:
    void updateAppearance(core::ServiceState state);
};

} // namespace space2x::ui
