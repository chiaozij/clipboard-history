/**
 * @file HistoryItemWidget.h
 * @brief 历史面板中单条记录的自定义 UI 组件
 *
 * 每条记录显示为一张圆角卡片，包含：
 * - 左侧：类型图标（文本）或缩略图（图片），48x48
 * - 中间：预览文本（截取前80字符）+ 时间戳
 * - 右侧：复制按钮（小图标）
 *
 * 交互：
 * - 左键点击 → 复制该条目到剪贴板
 * - 右键 → 弹出菜单（复制 / 删除）
 */

#ifndef HISTORYITEMWIDGET_H
#define HISTORYITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include "ClipboardItem.h"

class QMenu;

/**
 * @class HistoryItemWidget
 * @brief 单条历史记录的可视化卡片控件
 *
 * 每个控件展示一条 ClipboardItem 的内容预览。
 * 使用 QFrame 作为容器，内部使用 QHBoxLayout 水平排列。
 * 支持 hover 高亮效果（通过样式表）。
 */
class HistoryItemWidget : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param item 要展示的剪贴板条目数据
     * @param index 该条目在历史列表中的索引
     * @param parent 父控件
     */
    explicit HistoryItemWidget(const ClipboardItem &item,
                               int index,
                               QWidget *parent = nullptr);

    /// 析构函数
    ~HistoryItemWidget() override = default;

    /// 获取该控件对应的条目索引
    int index() const { return m_index; }

signals:
    /**
     * @brief 用户点击了"复制"（左键点击卡片或右键菜单选复制）
     * @param index 条目在历史列表中的索引
     */
    void copyRequested(int index);

    /**
     * @brief 用户点击了"删除"（右键菜单选删除并确认）
     * @param index 条目在历史列表中的索引
     */
    void deleteRequested(int index);

protected:
    /**
     * @brief 鼠标按下事件 — 左键复制，右键弹出菜单
     * @param event 鼠标事件
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief 鼠标进入事件 — 高亮该卡片
     */
    void enterEvent(QEnterEvent *event) override;

    /**
     * @brief 鼠标离开事件 — 恢复正常背景
     */
    void leaveEvent(QEvent *event) override;

private slots:
    /**
     * @brief 复制按钮被点击时的处理
     */
    void onCopyClicked();

    /**
     * @brief 右键菜单"删除"被点击时的处理
     *
     * 弹出 QMessageBox 确认对话框，用户确认后才发射 deleteRequested 信号。
     */
    void onDeleteActionTriggered();

private:
    /**
     * @brief 初始化 UI 布局和控件
     * @param item 条目数据
     */
    void setupUi(const ClipboardItem &item);

    /**
     * @brief 根据条目类型生成左侧图标/缩略图
     */
    void setTypeIcon(const ClipboardItem &item);

    /**
     * @brief 格式化时间戳为可读字符串
     * @param dt 日期时间
     * @return "刚刚" / "X分钟前" / "HH:mm" / "昨天 HH:mm" / "MM-dd HH:mm"
     */
    static QString formatTimestamp(const QDateTime &dt);

    int m_index;                ///< 在历史列表中的索引
    ClipboardItem m_item;       ///< 关联的条目数据

    // UI 控件
    QLabel *m_iconLabel;        ///< 左侧图标/缩略图
    QLabel *m_previewLabel;     ///< 预览文本
    QLabel *m_timeLabel;        ///< 时间戳标签
    QPushButton *m_copyBtn;     ///< 复制按钮
    QMenu *m_contextMenu;       ///< 右键弹出菜单
};

#endif // HISTORYITEMWIDGET_H
