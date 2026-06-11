/**
 * @file TrayManager.cpp
 * @brief 系统托盘管理 — 实现
 *
 * 创建和管理系统托盘图标、右键菜单。
 * 处理左键点击（切换面板显示/隐藏）和右键菜单各项操作。
 */

#include "TrayManager.h"
#include "HistoryPanel.h"
#include "ClipboardManager.h"
#include "ThemeManager.h"

#include <QApplication>
#include <QAbstractButton>
#include <QMessageBox>
#include <QStyle>

// ==================== 构造 ====================

TrayManager::TrayManager(HistoryPanel *panel,
                         ClipboardManager *clipboardManager,
                         QObject *parent)
    : QObject(parent)
    , m_trayIcon(new QSystemTrayIcon(this))
    , m_trayMenu(new QMenu())
    , m_panel(panel)
    , m_clipboardManager(clipboardManager)
    , m_showAction(nullptr)
    , m_clearAction(nullptr)
    , m_aboutAction(nullptr)
    , m_quitAction(nullptr)
{
    // --- 设置托盘图标 ---
    // 使用系统内置图标作为临时方案，正式发布时替换为自定义图标
    QIcon icon(":/icons/clip.png");
    if (icon.isNull()) {
        // 如果自定义图标不可用，使用 Qt 内置的系统图标
        icon = QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView);
    }
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip("剪贴板历史记录");

    // --- 构建右键菜单 ---
    createTrayMenu();
    m_trayIcon->setContextMenu(m_trayMenu);

    // --- 连接信号 ---
    // 左键点击 / 双击 / 中键点击 → 切换面板
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &TrayManager::onTrayIconActivated);
}

// ==================== 公共方法 ====================

void TrayManager::show()
{
    m_trayIcon->show();

    // 启动时显示一条小提示
    showNotification("剪贴板历史记录",
                     "应用已启动，正在监控剪贴板变化");
}

void TrayManager::showNotification(const QString &title, const QString &message)
{
    if (m_trayIcon->supportsMessages()) {
        m_trayIcon->showMessage(title, message,
                                QSystemTrayIcon::Information, 3000);
    }
}

// ==================== 右键菜单构建 ====================

void TrayManager::createTrayMenu()
{
    // 使用主题感知的菜单样式
    m_trayMenu->setStyleSheet(ThemeManager::instance()->menuStyle());

    // —— 菜单项 ——

    m_showAction = m_trayMenu->addAction("📋 显示历史面板");
    connect(m_showAction, &QAction::triggered,
            this, &TrayManager::onShowPanelAction);

    m_trayMenu->addSeparator();

    // — "切换主题" 菜单项 —
    QAction *themeAction = m_trayMenu->addAction("🎨 切换主题");
    connect(themeAction, &QAction::triggered, this, [this]() {
        ThemeManager::Theme current = ThemeManager::instance()->currentTheme();
        ThemeManager::Theme next = (current == ThemeManager::Dark)
                                   ? ThemeManager::Light
                                   : ThemeManager::Dark;
        ThemeManager::instance()->setTheme(next);

        // 刷新托盘菜单样式
        m_trayMenu->setStyleSheet(ThemeManager::instance()->menuStyle());

        QString themeName = (next == ThemeManager::Light) ? "浅色" : "暗色";
        showNotification("主题已切换", "当前主题：" + themeName);
    });

    m_clearAction = m_trayMenu->addAction("🗑 清空所有记录");
    connect(m_clearAction, &QAction::triggered,
            this, &TrayManager::onClearHistoryAction);

    m_aboutAction = m_trayMenu->addAction("ℹ 关于");
    connect(m_aboutAction, &QAction::triggered,
            this, &TrayManager::onAboutAction);

    m_trayMenu->addSeparator();

    m_quitAction = m_trayMenu->addAction("✕ 退出");
    connect(m_quitAction, &QAction::triggered,
            this, &TrayManager::onQuitAction);
}

// ==================== 私有槽 ====================

void TrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 在 Windows 上，左键单击触发 Trigger，左键双击触发 DoubleClick
    // 我们同时处理两种事件
    if (reason == QSystemTrayIcon::Trigger
        || reason == QSystemTrayIcon::DoubleClick) {
        onShowPanelAction();
    }
    // MiddleClick 也触发（如果有三键鼠标）
    if (reason == QSystemTrayIcon::MiddleClick) {
        onShowPanelAction();
    }
}

void TrayManager::onShowPanelAction()
{
    if (m_panel->isVisible()) {
        // 面板已显示 → 隐藏
        m_panel->hideWithAnimation();
    } else {
        // 面板隐藏中 → 在鼠标附近显示
        m_panel->showNearCursor();
    }
}

void TrayManager::onClearHistoryAction()
{
    // 弹出确认对话框
    QMessageBox msgBox;
    msgBox.setWindowTitle("清空确认");
    msgBox.setText("确定要清空所有剪贴板历史记录吗？");
    msgBox.setInformativeText("此操作不可撤销。");
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("清空");
    msgBox.button(QMessageBox::No)->setText("取消");

    // 对话框样式
    msgBox.setStyleSheet(ThemeManager::instance()->dialogStyle());

    if (msgBox.exec() == QMessageBox::Yes) {
        m_clipboardManager->clearHistory();
        showNotification("剪贴板历史记录", "所有记录已清空");
    }
}

void TrayManager::onAboutAction()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("关于");
    msgBox.setText("剪贴板历史记录");
    msgBox.setInformativeText(
        "版本 1.0.0\n\n"
        "一个轻量级 Windows 剪贴板历史记录小应用。\n"
        "支持文本和图片的记录与管理。\n\n"
        "技术：Qt 6 + C++17\n"
        "UI 风格：iOS / Windows 11 现代设计\n\n"
        "数据仅保存在内存中，关闭即清空。\n"
        "不会收集或上传任何用户数据。\n\n"
        "© 2026 Clipboard History"
    );
    msgBox.setIcon(QMessageBox::Information);

    msgBox.setStyleSheet(ThemeManager::instance()->dialogStyle());

    msgBox.exec();
}

void TrayManager::onQuitAction()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("退出确认");
    msgBox.setText("确定要退出剪贴板历史记录吗？");
    msgBox.setInformativeText("退出后所有历史记录将丢失。");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("退出");
    msgBox.button(QMessageBox::No)->setText("取消");

    msgBox.setStyleSheet(
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
        "QPushButton:hover { background: rgba(255,255,255,0.2); }"
    );

    if (msgBox.exec() == QMessageBox::Yes) {
        // 清理
        m_panel->hide();
        emit quitRequested();
    }
}
