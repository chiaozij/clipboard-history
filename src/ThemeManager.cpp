/**
 * @file ThemeManager.cpp
 * @brief 主题管理器 — 实现
 *
 * 两套完整配色方案的定义和切换逻辑。
 */

#include "ThemeManager.h"

// 单例初始化
ThemeManager *ThemeManager::s_instance = nullptr;

// ==================== 单例 ====================

ThemeManager *ThemeManager::instance()
{
    if (!s_instance) {
        s_instance = new ThemeManager();
    }
    return s_instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Dark)  // 默认暗色
{
}

// ==================== 主题切换 ====================

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme == theme) {
        return;  // 相同主题，无需切换
    }

    m_currentTheme = theme;
    emit themeChanged(theme);
}

// ==================== 全局样式表 ====================

QString ThemeManager::globalStyleSheet() const
{
    // 工具提示 + 滚动条全局样式
    return tooltipStyle() + scrollBarStyle();
}

QString ThemeManager::tooltipStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QToolTip {"
            "  background: #1A1A1A;"
            "  color: #E8E8E8;"
            "  border: 1px solid rgba(255,255,255,0.15);"
            "  border-radius: 8px;"
            "  padding: 8px 14px;"
            "  font-size: 12px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}";
    } else {
        return
            "QToolTip {"
            "  background: #FFFFFF;"
            "  color: #1A1A1A;"
            "  border: 1px solid rgba(0,0,0,0.1);"
            "  border-radius: 8px;"
            "  padding: 8px 14px;"
            "  font-size: 12px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}";
    }
}

QString ThemeManager::scrollBarStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QScrollBar:vertical {"
            "  background: transparent;"
            "  width: 6px;"
            "  margin: 0;"
            "}"
            "QScrollBar::handle:vertical {"
            "  background: rgba(255,255,255,0.15);"
            "  border-radius: 3px;"
            "  min-height: 30px;"
            "}"
            "QScrollBar::handle:vertical:hover {"
            "  background: rgba(255,255,255,0.25);"
            "}"
            "QScrollBar::add-line:vertical,"
            "QScrollBar::sub-line:vertical {"
            "  height: 0px;"
            "}"
            "QScrollBar::add-page:vertical,"
            "QScrollBar::sub-page:vertical {"
            "  background: none;"
            "}";
    } else {
        return
            "QScrollBar:vertical {"
            "  background: transparent;"
            "  width: 6px;"
            "  margin: 0;"
            "}"
            "QScrollBar::handle:vertical {"
            "  background: rgba(0,0,0,0.12);"
            "  border-radius: 3px;"
            "  min-height: 30px;"
            "}"
            "QScrollBar::handle:vertical:hover {"
            "  background: rgba(0,0,0,0.2);"
            "}"
            "QScrollBar::add-line:vertical,"
            "QScrollBar::sub-line:vertical {"
            "  height: 0px;"
            "}"
            "QScrollBar::add-page:vertical,"
            "QScrollBar::sub-page:vertical {"
            "  background: none;"
            "}";
    }
}

// ==================== 面板样式 ====================

QString ThemeManager::panelBackgroundColor() const
{
    return m_currentTheme == Dark
        ? "rgba(32, 32, 32, 235)"
        : "rgba(243, 243, 243, 215)";
}

int ThemeManager::panelBackgroundAlpha() const
{
    return m_currentTheme == Dark ? 235 : 215;
}

QString ThemeManager::panelBorderColor() const
{
    return m_currentTheme == Dark
        ? "rgba(255, 255, 255, 30)"
        : "rgba(0, 0, 0, 25)";
}

// ==================== 搜索框样式 ====================

QString ThemeManager::searchBoxStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QLineEdit {"
            "  background: rgba(255, 255, 255, 0.10);"
            "  border: 1px solid rgba(255, 255, 255, 0.12);"
            "  border-radius: 10px;"
            "  padding: 9px 12px;"
            "  color: #E8E8E8;"
            "  font-size: 13px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}"
            "QLineEdit:focus {"
            "  background: rgba(255, 255, 255, 0.15);"
            "  border-color: rgba(255, 255, 255, 0.3);"
            "}";
    } else {
        return
            "QLineEdit {"
            "  background: rgba(0, 0, 0, 0.04);"
            "  border: 1px solid rgba(0, 0, 0, 0.08);"
            "  border-radius: 10px;"
            "  padding: 9px 12px;"
            "  color: #1A1A1A;"
            "  font-size: 13px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}"
            "QLineEdit:focus {"
            "  background: rgba(0, 0, 0, 0.06);"
            "  border-color: rgba(0, 0, 0, 0.2);"
            "}";
    }
}

// ==================== 卡片样式 ====================

QString ThemeManager::cardNormalStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "HistoryItemWidget {"
            "  background: rgba(255, 255, 255, 0.06);"
            "  border-radius: 8px;"
            "  margin: 2px 8px;"
            "}";
    } else {
        return
            "HistoryItemWidget {"
            "  background: rgba(0, 0, 0, 0.03);"
            "  border-radius: 8px;"
            "  margin: 2px 8px;"
            "}";
    }
}

QString ThemeManager::cardHoverStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "HistoryItemWidget:hover {"
            "  background: rgba(255, 255, 255, 0.15);"
            "}";
    } else {
        return
            "HistoryItemWidget:hover {"
            "  background: rgba(0, 0, 0, 0.06);"
            "}";
    }
}

QString ThemeManager::cardIconStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QLabel {"
            "  background: rgba(255,255,255,0.08);"
            "  border-radius: 6px;"
            "}";
    } else {
        return
            "QLabel {"
            "  background: rgba(0,0,0,0.04);"
            "  border-radius: 6px;"
            "}";
    }
}

QString ThemeManager::cardTextColor() const
{
    return m_currentTheme == Dark ? "#E8E8E8" : "#1A1A1A";
}

QString ThemeManager::cardTimeColor() const
{
    return m_currentTheme == Dark ? "#888" : "#666";
}

QString ThemeManager::cardCopyBtnStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QPushButton {"
            "  background: rgba(255,255,255,0.08);"
            "  border: none;"
            "  border-radius: 6px;"
            "  font-size: 16px;"
            "}";
    } else {
        return
            "QPushButton {"
            "  background: rgba(0,0,0,0.05);"
            "  border: none;"
            "  border-radius: 6px;"
            "  font-size: 16px;"
            "}";
    }
}

QString ThemeManager::cardCopyBtnHoverStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QPushButton:hover {"
            "  background: rgba(255,255,255,0.2);"
            "}";
    } else {
        return
            "QPushButton:hover {"
            "  background: rgba(0,0,0,0.1);"
            "}";
    }
}

// ==================== 对话框样式 ====================

QString ThemeManager::dialogStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QMessageBox {"
            "  background-color: #2D2D2D;"
            "  color: #E8E8E8;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}"
            "QMessageBox QLabel { color: #E8E8E8; }"
            "QPushButton {"
            "  background: rgba(255,255,255,0.1);"
            "  border: 1px solid rgba(255,255,255,0.15);"
            "  border-radius: 6px;"
            "  padding: 6px 20px;"
            "  color: #E8E8E8;"
            "}"
            "QPushButton:hover { background: rgba(255,255,255,0.2); }";
    } else {
        return
            "QMessageBox {"
            "  background-color: #F5F5F5;"
            "  color: #1A1A1A;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "}"
            "QMessageBox QLabel { color: #1A1A1A; }"
            "QPushButton {"
            "  background: rgba(0,0,0,0.05);"
            "  border: 1px solid rgba(0,0,0,0.1);"
            "  border-radius: 6px;"
            "  padding: 6px 20px;"
            "  color: #1A1A1A;"
            "}"
            "QPushButton:hover { background: rgba(0,0,0,0.1); }";
    }
}

// ==================== 菜单样式 ====================

QString ThemeManager::menuStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QMenu {"
            "  background: #2D2D2D;"
            "  border: 1px solid rgba(255,255,255,0.1);"
            "  border-radius: 8px;"
            "  padding: 6px 4px;"
            "  color: #E8E8E8;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "  font-size: 13px;"
            "}"
            "QMenu::item {"
            "  padding: 8px 28px 8px 14px;"
            "  border-radius: 6px;"
            "  margin: 2px 4px;"
            "}"
            "QMenu::item:selected {"
            "  background: rgba(255,255,255,0.12);"
            "}"
            "QMenu::separator {"
            "  height: 1px;"
            "  background: rgba(255,255,255,0.08);"
            "  margin: 4px 8px;"
            "}";
    } else {
        return
            "QMenu {"
            "  background: #FFFFFF;"
            "  border: 1px solid rgba(0,0,0,0.08);"
            "  border-radius: 8px;"
            "  padding: 6px 4px;"
            "  color: #1A1A1A;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "  font-size: 13px;"
            "}"
            "QMenu::item {"
            "  padding: 8px 28px 8px 14px;"
            "  border-radius: 6px;"
            "  margin: 2px 4px;"
            "}"
            "QMenu::item:selected {"
            "  background: rgba(0,0,0,0.06);"
            "}"
            "QMenu::separator {"
            "  height: 1px;"
            "  background: rgba(0,0,0,0.06);"
            "  margin: 4px 8px;"
            "}";
    }
}

// ==================== 状态标签样式 ====================

QString ThemeManager::statusLabelStyle() const
{
    if (m_currentTheme == Dark) {
        return
            "QLabel {"
            "  color: #777;"
            "  font-size: 13px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "  padding: 30px 0;"
            "}";
    } else {
        return
            "QLabel {"
            "  color: #999;"
            "  font-size: 13px;"
            "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
            "  padding: 30px 0;"
            "}";
    }
}
