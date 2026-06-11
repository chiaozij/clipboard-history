/**
 * @file HistoryPanel.h
 * @brief 剪贴板历史记录浮动面板（主窗口）
 *
 * 一个现代化的无边框弹出窗口，在鼠标位置附近显示。
 * 顶部为搜索框，中部为可滚动的历史条目列表，底部为状态栏。
 * 支持平滑的淡入淡出动画、毛玻璃/Acrylic 背景效果。
 * 失去焦点或按 ESC 时自动隐藏。
 */

#ifndef HISTORYPANEL_H
#define HISTORYPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QList>
#include "ClipboardManager.h"

// 面板尺寸常量
constexpr int PANEL_WIDTH = 380;   ///< 面板固定宽度
constexpr int PANEL_MAX_HEIGHT = 500;  ///< 面板最大高度
constexpr int ANIM_DURATION_SHOW = 200;   ///< 弹出动画时长（毫秒）
constexpr int ANIM_DURATION_HIDE = 150;   ///< 消失动画时长（毫秒）

class HistoryItemWidget;

/**
 * @class HistoryPanel
 * @brief 剪贴板历史浮动面板
 *
 * 无边框的圆角窗口，通常显示在鼠标光标附近。
 * 包含搜索过滤、条目列表和状态提示。
 * 使用 QPropertyAnimation 实现 opacity 和 geometry 的平滑过渡。
 *
 * 窗口标志：
 * - FramelessWindowHint：无边框，自制标题栏
 * - WindowStaysOnTopHint：始终在其他窗口之上
 * - Popup：点击外部时自动关闭
 *
 * 毛玻璃效果：
 * - 优先调用 Windows DWM API (DwmExtendFrameIntoClientArea)
 * - 若不可用则退化为 QGraphicsBlurEffect + 半透明黑色背景
 */
class HistoryPanel : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param clipboardManager 剪贴板管理器（数据源）
     * @param parent 父控件（通常为 nullptr，因为这是顶层窗口）
     */
    explicit HistoryPanel(ClipboardManager *clipboardManager,
                          QWidget *parent = nullptr);

    /// 析构函数
    ~HistoryPanel() override;

    /**
     * @brief 在鼠标光标附近显示面板（带弹出动画）
     *
     * 计算鼠标位置，将面板定位在光标左下方。
     * 如果空间不足，自动调整面板位置使其不超出屏幕。
     * 显示面板前设置初始透明度为 0，然后执行 opacity 动画。
     */
    void showNearCursor();

    /**
     * @brief 以淡出动画隐藏面板
     *
     * 播放 opacity 动画从当前值到 0，动画结束后调用 hide()。
     */
    void hideWithAnimation();

public slots:
    /**
     * @brief 刷新历史记录列表
     *
     * 当 ClipboardManager::historyChanged() 发射时调用此槽。
     * 清空并重新创建所有 HistoryItemWidget 控件。
     * 如果搜索框中有文本，则进行过滤。
     */
    void refreshList();

protected:
    // ==================== Qt 事件重写 ====================

    /**
     * @brief 窗口失去焦点事件 — 自动隐藏面板
     */
    void focusOutEvent(QFocusEvent *event) override;

    /**
     * @brief 按键事件 — ESC 键隐藏面板
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief 窗口显示事件 — 在此启用 Windows DWM 毛玻璃效果
     */
    void showEvent(QShowEvent *event) override;

    /**
     * @brief 绘制事件 — 绘制圆角边框和阴影
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 窗口大小变化事件 — 更新圆角裁剪区域
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief 搜索框文本变化时的实时过滤
     * @param text 搜索框当前文本
     *
     * 遍历列表中的每个 HistoryItemWidget，
     * 根据预览文本是否包含搜索关键字来决定显示/隐藏。
     * 不区分大小写。
     */
    void onSearchTextChanged(const QString &text);

    /**
     * @brief 某条记录被请求复制
     * @param index 条目索引
     *
     * 调用 ClipboardManager::copyToClipboard() 复制内容，
     * 然后隐藏面板。
     */
    void onItemCopyRequested(int index);

    /**
     * @brief 某条记录被请求删除
     * @param index 条目索引
     *
     * 调用 ClipboardManager::removeItem() 删除指定条目，
     * 然后调用 refreshList() 刷新 UI。
     */
    void onItemDeleteRequested(int index);

private:
    /**
     * @brief 启用 Windows DWM 亚克力/毛玻璃背景
     *
     * 通过 DwmExtendFrameIntoClientArea + DwmSetWindowAttribute
     * 设置 Acrylic/Mica 效果。
     * 如果 DWM 不可用，则通过样式表设置半透明黑色背景作为退化方案。
     */
    void enableAcrylicBackground();

    /**
     * @brief 设置窗口圆角裁剪区域
     *
     * 使用 QPainterPath 创建圆角矩形路径，
     * 通过 setMask() 裁剪窗口四角。
     */
    void updateRoundedMask();

    /**
     * @brief 根据搜索文本过滤列表项
     */
    void filterItems(const QString &searchText);

    // ==================== 成员变量 ====================

    ClipboardManager *m_clipboardManager;  ///< 数据源

    // —— UI 控件 ——
    QLineEdit *m_searchBox;               ///< 顶部搜索框
    QScrollArea *m_scrollArea;            ///< 可滚动区域
    QWidget *m_listContainer;             ///< 条目容器（scrollArea 的内容 widget）
    QVBoxLayout *m_listLayout;            ///< 条目容器的垂直布局
    QLabel *m_statusLabel;                ///< 状态文本（"暂无记录"）

    // —— 动画 ——
    QPropertyAnimation *m_opacityAnimation;  ///< 透明度动画
    QPropertyAnimation *m_geometryAnimation; ///< 几何位置/大小动画

    // —— 条目列表 ——
    QList<HistoryItemWidget *> m_itemWidgets;  ///< 当前显示的条目控件列表
};

#endif // HISTORYPANEL_H
