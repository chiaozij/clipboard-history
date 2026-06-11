/**
 * @file ThemeManager.h
 * @brief 主题管理器 — 管理应用的暗色/浅色两套主题配色
 *
 * 单例模式，全局访问点。
 * 提供统一的样式表生成接口，所有 UI 组件通过此管理器获取当前主题的样式。
 * 切换主题时发射 themeChanged 信号，各组件监听并刷新自身样式。
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

/**
 * @class ThemeManager
 * @brief 全局主题管理单例
 *
 * 维护暗色（Dark）和浅色（Light / Win11 Mica）两套主题配置。
 * 每套主题包含：
 * - 全局应用级样式表
 * - 各组件专用样式片段（面板、卡片、搜索框、对话框、菜单）
 * - Windows DWM 模式标记（暗色/亮色 Mica）
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 主题枚举
     */
    enum Theme {
        Dark,   ///< 暗色主题（默认）
        Light   ///< 浅色主题（Win11 Mica 风格）
    };

    /**
     * @brief 获取单例实例
     */
    static ThemeManager *instance();

    /**
     * @brief 获取当前主题
     */
    Theme currentTheme() const { return m_currentTheme; }

    /**
     * @brief 切换主题
     * @param theme 目标主题
     *
     * 切换成功后发射 themeChanged() 信号，所有监听的 UI 组件应随之刷新。
     */
    void setTheme(Theme theme);

    // ==================== 样式表生成 ====================

    /// 全局应用级样式表（QApplication::setStyleSheet 调用）
    QString globalStyleSheet() const;

    /// 面板背景样式（HistoryPanel 自身绘制 + paintEvent）
    QString panelBackgroundColor() const;
    int panelBackgroundAlpha() const;
    QString panelBorderColor() const;

    /// 搜索框样式
    QString searchBoxStyle() const;

    /// 滚动条样式
    QString scrollBarStyle() const;

    /// 列表卡片样式
    QString cardNormalStyle() const;
    QString cardHoverStyle() const;
    QString cardIconStyle() const;
    QString cardTextColor() const;
    QString cardTimeColor() const;
    QString cardCopyBtnStyle() const;
    QString cardCopyBtnHoverStyle() const;

    /// 对话框样式
    QString dialogStyle() const;

    /// 右键菜单样式
    QString menuStyle() const;

    /// 工具提示样式
    QString tooltipStyle() const;

    /// 状态标签样式
    QString statusLabelStyle() const;

    /// Windows DWM 暗色模式标记
    bool useDarkMode() const { return m_currentTheme == Dark; }

signals:
    /**
     * @brief 主题切换后发射
     * @param theme 新的主题
     */
    void themeChanged(Theme theme);

private:
    /// 私有构造函数（单例）
    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager() override = default;

    static ThemeManager *s_instance;  ///< 单例指针

    Theme m_currentTheme;             ///< 当前主题
};

#endif // THEMEMANAGER_H
