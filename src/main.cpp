/**
 * @file main.cpp
 * @brief 应用程序入口
 *
 * 初始化 Qt 应用、创建核心组件并连接信号槽。
 * 应用架构：
 *   1. QApplication        — Qt 事件循环
 *   2. ClipboardManager    — 剪贴板监控 + 历史数据
 *   3. HistoryPanel        — 浮动面板 UI
 *   4. TrayManager         — 系统托盘 + 菜单
 *
 * 应用生命周期：
 *   启动 → 托盘图标出现 → 后台监控剪贴板
 *   → 用户点击托盘图标 → 弹出历史面板
 *   → 用户关闭面板 / 退出 → 清理内存 → 退出
 *
 * 编译：需要 Qt 6 (Core, Gui, Widgets 模块) + C++17
 * 运行：Windows 10 1809+ / Windows 11
 */

#include <QApplication>
#include <QMessageBox>

#include "ClipboardManager.h"
#include "HistoryPanel.h"
#include "TrayManager.h"
#include "ThemeManager.h"

/**
 * @brief 应用入口
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 退出码（0 = 正常退出）
 */
int main(int argc, char *argv[])
{
    // ==================== 1. 初始化 QApplication ====================

    // 启用高 DPI 缩放支持（Qt 6 默认启用，但仍显式设置确保兼容）
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);

    // 应用元数据（被关于对话框等引用）
    app.setApplicationName("剪贴板历史记录");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ClipboardHistory");
    app.setQuitOnLastWindowClosed(false);  // 关闭面板不退出应用，托盘常驻

    // ==================== 2. 全局样式表（由 ThemeManager 管理） ====================
    app.setStyleSheet(
        // 通用字体
        "* {"
        "  font-family: 'Microsoft YaHei', 'Segoe UI', 'PingFang SC', sans-serif;"
        "}"
        + ThemeManager::instance()->globalStyleSheet()
    );

    // ==================== 3. 创建核心组件 ====================

    // 剪贴板管理器（数据层）— 自动开始监控剪贴板
    ClipboardManager clipboardManager;

    // 历史面板（视图层）— 无边框浮动窗口
    HistoryPanel historyPanel(&clipboardManager);

    // 托盘管理器（交互层）— 系统托盘图标和菜单
    TrayManager trayManager(&historyPanel, &clipboardManager);

    // ==================== 4. 连接信号槽 ====================

    // 托盘"退出"菜单 → 退出应用
    QObject::connect(&trayManager, &TrayManager::quitRequested,
                     &app, &QApplication::quit);

    // ==================== 5. 启动托盘 ====================

    // 显示系统托盘图标（应用进入后台监控状态）
    trayManager.show();

    // ==================== 6. 进入事件循环 ====================

    // Qt 事件循环，等待用户交互
    int result = app.exec();

    // 正常退出：历史记录在 ClipboardManager 析构时自动清理（内存数据）
    return result;
}
