/**
 * @file ClipboardManager.h
 * @brief 系统剪贴板监控与历史记录管理
 *
 * 核心功能：
 * - 监控系统剪贴板变化（通过 QClipboard::dataChanged 信号）
 * - 管理剪贴板历史记录列表（QList<ClipboardItem>），最多保留 200 条
 * - 自动去重（连续相同内容不重复记录）
 * - 提供增、删、查、清空等操作接口
 */

#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QObject>
#include <QList>
#include <QClipboard>
#include <QTimer>
#include "ClipboardItem.h"

/// 历史记录最大保留条数
constexpr int MAX_HISTORY_SIZE = 200;

/// 剪贴板变化防抖延迟（毫秒），避免连续多次触发
constexpr int DEBOUNCE_DELAY_MS = 100;

/**
 * @class ClipboardManager
 * @brief 剪贴板监控与历史管理核心类
 *
 * 监听系统剪贴板的内容变化，将每次复制的新内容添加到历史记录中。
 * 支持文本和图片两种内容类型。
 * 通过 100ms 的防抖定时器避免短时间内的重复触发。
 * 最多保留 MAX_HISTORY_SIZE（200）条记录，超出时自动删除最旧的条目。
 * 连续复制相同内容时自动跳过，不去重。
 *
 * @note 历史记录仅保存在内存中，应用关闭后即清空
 */
class ClipboardManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父 QObject，用于 Qt 对象生命周期管理
     *
     * 初始化剪贴板监控、防抖定时器，连接 dataChanged 信号。
     */
    explicit ClipboardManager(QObject *parent = nullptr);

    /// 析构函数
    ~ClipboardManager() override = default;

    // ==================== 查询接口 ====================

    /// 获取历史记录总数
    int count() const { return m_history.size(); }

    /**
     * @brief 获取指定索引的历史记录
     * @param index 索引（0 = 最新）
     * @return 该索引的 ClipboardItem
     * @note 如果索引越界，返回一个空的 ClipboardItem
     */
    ClipboardItem itemAt(int index) const;

    /// 获取完整历史记录列表的只读引用
    const QList<ClipboardItem> &history() const { return m_history; }

signals:
    /**
     * @brief 历史记录发生变化时发射
     *
     * 添加、删除、清空操作都会触发此信号。
     * HistoryPanel 连接此信号来实时刷新 UI。
     */
    void historyChanged();

    /**
     * @brief 新条目被添加到历史记录时发射
     * @param index 新条目的索引（始终为 0，因为新条目插在最前面）
     */
    void itemAdded(int index);

public slots:
    /**
     * @brief 删除指定索引的历史记录
     * @param index 要删除的条目索引
     *
     * 删除前会校验索引有效性。
     * 删除后发射 historyChanged() 信号。
     */
    void removeItem(int index);

    /**
     * @brief 清空所有历史记录
     *
     * 清空前会发射 historyChanged() 信号通知 UI 更新。
     */
    void clearHistory();

    /**
     * @brief 复制指定条目到系统剪贴板
     * @param index 条目索引
     *
     * 根据条目类型，将文本或图片写回系统剪贴板。
     * 用于用户在面板中点击某条历史记录后重新复制。
     */
    void copyToClipboard(int index);

private slots:
    /**
     * @brief 系统剪贴板内容变化时的回调（内部槽函数）
     *
     * 此槽连接到 QClipboard::dataChanged() 信号。
     * 启动防抖定时器，延迟 DEBOUNCE_DELAY_MS 毫秒后真正处理。
     */
    void onClipboardDataChanged();

    /**
     * @brief 防抖定时器超时回调（内部槽函数）
     *
     * 真正读取剪贴板内容并添加到历史记录。
     */
    void onDebounceTimerTimeout();

private:
    /**
     * @brief 添加一条记录到历史
     * @param item ClipboardItem 对象
     *
     * - 如果与新条目与最新一条内容相同，则不添加（去重）
     * - 新的条目插入列表最前面（索引 0）
     * - 如果超过 MAX_HISTORY_SIZE，删除列表末尾最旧的条目
     * - 添加成功后发射 historyChanged() 和 itemAdded() 信号
     */
    void addItem(const ClipboardItem &item);

    QClipboard *m_clipboard;           ///< 系统剪贴板引用
    QList<ClipboardItem> m_history;    ///< 历史记录列表（按时间倒序）
    QTimer *m_debounceTimer;           ///< 防抖定时器
};

#endif // CLIPBOARDMANAGER_H
