/**
 * @file TrayManager.h
 * @brief 系统托盘管理
 *
 * 管理应用在 Windows 系统托盘中的图标和行为。
 * - 左键点击托盘图标 → 切换历史面板显示/隐藏
 * - 右键点击托盘图标 → 弹出菜单（显示面板 / 清空记录 / 关于 / 退出）
 * - 退出时弹出确认对话框，防止误操作
 */

#ifndef TRAYMANAGER_H
#define TRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class HistoryPanel;
class ClipboardManager;

/**
 * @class TrayManager
 * @brief 系统托盘图标和菜单管理
 *
 * 封装 QSystemTrayIcon，提供统一的托盘交互接口。
 * 负责托盘图标的创建、左键/右键事件响应、
 * 右键菜单的构建以及对应的操作路由。
 */
class TrayManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param panel 历史面板（用于显示/隐藏切换）
     * @param clipboardManager 剪贴板管理器（用于清空操作）
     * @param parent 父 QObject
     */
    explicit TrayManager(HistoryPanel *panel,
                         ClipboardManager *clipboardManager,
                         QObject *parent = nullptr);

    /// 析构函数
    ~TrayManager() override = default;

    /**
     * @brief 显示托盘图标
     *
     * 应用启动时调用，在系统托盘中显示图标。
     * 如果系统不支持托盘（如某些 Linux 桌面），会静默失败。
     */
    void show();

    /**
     * @brief 显示一条系统通知（气泡提示）
     * @param title 通知标题
     * @param message 通知内容
     *
     * 例如：复制成功时显示 "已复制到剪贴板"
     */
    void showNotification(const QString &title, const QString &message);

signals:
    /**
     * @brief 用户请求退出应用
     *
     * 由 main.cpp 连接此信号到 QApplication::quit()
     */
    void quitRequested();

private slots:
    /**
     * @brief 托盘图标被左键点击（activated 信号）
     * @param reason 激活原因（DoubleClick / Trigger / MiddleClick 等）
     *
     * 在 Windows 上，左键单击通常触发 Trigger 原因。
     * 此处处理为切换面板显示/隐藏。
     */
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    /**
     * @brief "显示/隐藏面板" 菜单项被点击
     */
    void onShowPanelAction();

    /**
     * @brief "清空所有记录" 菜单项被点击
     *
     * 弹出确认对话框，用户确认后调用 ClipboardManager::clearHistory()
     */
    void onClearHistoryAction();

    /**
     * @brief "关于" 菜单项被点击
     *
     * 显示应用名称、版本、作者信息。
     */
    void onAboutAction();

    /**
     * @brief "退出" 菜单项被点击
     *
     * 弹出确认对话框，用户确认后发射 quitRequested() 信号。
     */
    void onQuitAction();

private:
    /**
     * @brief 构建托盘右键菜单
     */
    void createTrayMenu();

    QSystemTrayIcon *m_trayIcon;         ///< 系统托盘图标
    QMenu *m_trayMenu;                  ///< 托盘右键菜单
    HistoryPanel *m_panel;              ///< 关联的历史面板
    ClipboardManager *m_clipboardManager; ///< 剪贴板管理器

    // 菜单项
    QAction *m_showAction;              ///< 显示/隐藏面板
    QAction *m_clearAction;             ///< 清空记录
    QAction *m_aboutAction;             ///< 关于
    QAction *m_quitAction;              ///< 退出
};

#endif // TRAYMANAGER_H
