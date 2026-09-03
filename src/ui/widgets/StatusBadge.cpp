#include "StatusBadge.h"

namespace space2x::ui {

StatusBadge::StatusBadge(QWidget* parent)
    : QLabel(parent) {
    setAlignment(Qt::AlignCenter);
    setState(core::ServiceState::Unknown);
}

void StatusBadge::setState(core::ServiceState state) {
    updateAppearance(state);
}

void StatusBadge::updateAppearance(core::ServiceState state) {
    QString text;
    QString bgColor;
    QString textColor = "#FFFFFF";

    switch (state) {
        case core::ServiceState::Running:
            text = "RUNNING";
            bgColor = "#2E7D32"; // Dark Green
            break;
        case core::ServiceState::Starting:
            text = "STARTING";
            bgColor = "#0277BD"; // Blue
            break;
        case core::ServiceState::Stopping:
            text = "STOPPING";
            bgColor = "#E65100"; // Orange
            break;
        case core::ServiceState::Stopped:
            text = "STOPPED";
            bgColor = "#424242"; // Dark Gray
            break;
        case core::ServiceState::Degraded:
            text = "DEGRADED";
            bgColor = "#C62828"; // Red
            break;
        case core::ServiceState::NotInstalled:
            text = "NOT INSTALLED";
            bgColor = "#616161";
            break;
        case core::ServiceState::Installed:
            text = "INSTALLED";
            bgColor = "#37474F";
            break;
        case core::ServiceState::Unknown:
        default:
            text = "UNKNOWN";
            bgColor = "#757575";
            break;
    }

    setText(text);
    setStyleSheet(QString(
        "QLabel {"
        "  background-color: %1;"
        "  color: %2;"
        "  font-size: 11px;"
        "  font-weight: 600;"
        "  border-radius: 4px;"
        "  padding: 3px 8px;"
        "}"
    ).arg(bgColor, textColor));
}

} // namespace space2x::ui
