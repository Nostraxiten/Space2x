#include "ThemeManager.h"
#include <QApplication>

namespace space2x::ui {

ThemeManager& ThemeManager::instance() {
    static ThemeManager mgr;
    return mgr;
}

ThemeManager::ThemeManager()
    : m_theme(ThemeMode::Dark) {}

void ThemeManager::setTheme(ThemeMode mode) {
    if (m_theme != mode) {
        m_theme = mode;
        emit themeChanged(m_theme);
    }
}

void ThemeManager::toggleTheme() {
    setTheme(m_theme == ThemeMode::Dark ? ThemeMode::Light : ThemeMode::Dark);
}

QString ThemeManager::windowBackground() const {
    return isDark() ? "#0B0F19" : "#F8FAFC";
}

QString ThemeManager::contentBackground() const {
    return isDark() ? "#0F172A" : "#F1F5F9";
}

QString ThemeManager::cardBackground() const {
    return isDark() ? "#1E293B" : "#FFFFFF";
}

QString ThemeManager::cardBorder() const {
    return isDark() ? "#334155" : "#E2E8F0";
}

QString ThemeManager::titleColor() const {
    return isDark() ? "#F8FAFC" : "#0F172A";
}

QString ThemeManager::subtitleColor() const {
    return isDark() ? "#94A3B8" : "#64748B";
}

QString ThemeManager::textColor() const {
    return isDark() ? "#F1F5F9" : "#0F172A";
}

QString ThemeManager::secondaryTextColor() const {
    return isDark() ? "#94A3B8" : "#475569";
}

QString ThemeManager::tableStyle() const {
    if (isDark()) {
        return "QTableWidget {"
               "  background-color: #111827;"
               "  color: #F8FAFC;"
               "  border: 1px solid #1E293B;"
               "  border-radius: 8px;"
               "  gridline-color: #1F2937;"
               "  selection-background-color: #2563EB;"
               "  selection-color: #FFFFFF;"
               "  outline: none;"
               "}"
               "QTableWidget::item {"
               "  color: #F8FAFC;"
               "  padding: 6px 10px;"
               "  border-bottom: 1px solid #1F2937;"
               "}"
               "QTableWidget::item:selected {"
               "  background-color: #2563EB;"
               "  color: #FFFFFF;"
               "}"
               "QHeaderView::section {"
               "  background-color: #1F2937;"
               "  color: #F1F5F9;"
               "  padding: 8px 10px;"
               "  font-weight: 700;"
               "  border: none;"
               "  border-bottom: 2px solid #374151;"
               "}"
               "QScrollBar:vertical {"
               "  background: #111827;"
               "  width: 10px;"
               "  margin: 0px;"
               "}"
               "QScrollBar::handle:vertical {"
               "  background: #374151;"
               "  min-height: 20px;"
               "  border-radius: 5px;"
               "}"
               "QScrollBar::handle:vertical:hover {"
               "  background: #4B5563;"
               "}"
               "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
               "  height: 0px;"
               "}";
    } else {
        return "QTableWidget {"
               "  background-color: #FFFFFF;"
               "  color: #0F172A;"
               "  border: 1px solid #E2E8F0;"
               "  border-radius: 8px;"
               "  gridline-color: #F1F5F9;"
               "  selection-background-color: #DBEAFE;"
               "  selection-color: #1E40AF;"
               "  outline: none;"
               "}"
               "QTableWidget::item {"
               "  color: #0F172A;"
               "  padding: 6px 10px;"
               "  border-bottom: 1px solid #F1F5F9;"
               "}"
               "QTableWidget::item:selected {"
               "  background-color: #DBEAFE;"
               "  color: #1E40AF;"
               "}"
               "QHeaderView::section {"
               "  background-color: #F8FAFC;"
               "  color: #1E293B;"
               "  padding: 8px 10px;"
               "  font-weight: 700;"
               "  border: none;"
               "  border-bottom: 2px solid #E2E8F0;"
               "}"
               "QScrollBar:vertical {"
               "  background: #F8FAFC;"
               "  width: 10px;"
               "  margin: 0px;"
               "}"
               "QScrollBar::handle:vertical {"
               "  background: #CBD5E1;"
               "  min-height: 20px;"
               "  border-radius: 5px;"
               "}"
               "QScrollBar::handle:vertical:hover {"
               "  background: #94A3B8;"
               "}"
               "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
               "  height: 0px;"
               "}";
    }
}

QString ThemeManager::inputStyle() const {
    if (isDark()) {
        return "QLineEdit {"
               "  background-color: #1E293B;"
               "  color: #F8FAFC;"
               "  border: 1px solid #334155;"
               "  border-radius: 6px;"
               "  padding: 6px 12px;"
               "  selection-background-color: #3B82F6;"
               "  selection-color: #FFFFFF;"
               "}"
               "QLineEdit:focus {"
               "  border: 1px solid #3B82F6;"
               "}";
    } else {
        return "QLineEdit {"
               "  background-color: #FFFFFF;"
               "  color: #0F172A;"
               "  border: 1px solid #CBD5E1;"
               "  border-radius: 6px;"
               "  padding: 6px 12px;"
               "  selection-background-color: #2563EB;"
               "  selection-color: #FFFFFF;"
               "}"
               "QLineEdit:focus {"
               "  border: 1px solid #2563EB;"
               "}";
    }
}

QString ThemeManager::secondaryButtonStyle() const {
    if (isDark()) {
        return "QPushButton {"
               "  background-color: #1E293B;"
               "  color: #F1F5F9;"
               "  border: 1px solid #334155;"
               "  border-radius: 6px;"
               "  padding: 6px 14px;"
               "  font-weight: 500;"
               "}"
               "QPushButton:hover {"
               "  background-color: #334155;"
               "}"
               "QPushButton:pressed {"
               "  background-color: #0F172A;"
               "}";
    } else {
        return "QPushButton {"
               "  background-color: #FFFFFF;"
               "  color: #0F172A;"
               "  border: 1px solid #CBD5E1;"
               "  border-radius: 6px;"
               "  padding: 6px 14px;"
               "  font-weight: 500;"
               "}"
               "QPushButton:hover {"
               "  background-color: #F1F5F9;"
               "}"
               "QPushButton:pressed {"
               "  background-color: #E2E8F0;"
               "}";
    }
}

QString ThemeManager::groupboxStyle() const {
    if (isDark()) {
        return "QGroupBox {"
               "  background-color: #1E293B;"
               "  color: #F8FAFC;"
               "  font-weight: 700;"
               "  font-size: 13px;"
               "  border: 1px solid #334155;"
               "  border-radius: 8px;"
               "  margin-top: 10px;"
               "  padding-top: 16px;"
               "  padding-left: 12px;"
               "  padding-right: 12px;"
               "  padding-bottom: 12px;"
               "}"
               "QGroupBox::title {"
               "  subcontrol-origin: margin;"
               "  subcontrol-position: top left;"
               "  left: 12px;"
               "  padding: 0 4px;"
               "  color: #93C5FD;"
               "}";
    } else {
        return "QGroupBox {"
               "  background-color: #FFFFFF;"
               "  color: #0F172A;"
               "  font-weight: 700;"
               "  font-size: 13px;"
               "  border: 1px solid #E2E8F0;"
               "  border-radius: 8px;"
               "  margin-top: 10px;"
               "  padding-top: 16px;"
               "  padding-left: 12px;"
               "  padding-right: 12px;"
               "  padding-bottom: 12px;"
               "}"
               "QGroupBox::title {"
               "  subcontrol-origin: margin;"
               "  subcontrol-position: top left;"
               "  left: 12px;"
               "  padding: 0 4px;"
               "  color: #1E40AF;"
               "}";
    }
}

} // namespace space2x::ui
