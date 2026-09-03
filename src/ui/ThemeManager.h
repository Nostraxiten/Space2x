#pragma once

#include <QObject>
#include <QString>

namespace space2x::ui {

enum class ThemeMode {
    Dark,
    Light
};

class ThemeManager : public QObject {
    Q_OBJECT

public:
    static ThemeManager& instance();

    ThemeMode currentTheme() const { return m_theme; }
    bool isDark() const { return m_theme == ThemeMode::Dark; }

    void setTheme(ThemeMode mode);
    void toggleTheme();

    // Theme style helper strings
    QString windowBackground() const;
    QString contentBackground() const;
    QString cardBackground() const;
    QString cardBorder() const;
    QString titleColor() const;
    QString subtitleColor() const;
    QString textColor() const;
    QString secondaryTextColor() const;
    QString tableStyle() const;
    QString inputStyle() const;
    QString secondaryButtonStyle() const;
    QString groupboxStyle() const;

signals:
    void themeChanged(ThemeMode mode);

private:
    ThemeManager();
    ThemeMode m_theme = ThemeMode::Dark;
};

} // namespace space2x::ui
