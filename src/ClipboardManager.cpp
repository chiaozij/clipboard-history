/**
 * @file ClipboardManager.cpp
 * @brief 系统剪贴板监控与历史记录管理 — 实现
 *
 * 监听 QClipboard::dataChanged() 信号，通过防抖定时器（100ms）
 * 避免短时间内重复触发。支持文本和图片两种剪贴板内容。
 * 历史记录最多保留 MAX_HISTORY_SIZE 条，存在内存中。
 */

#include "ClipboardManager.h"
#include <QApplication>
#include <QMimeData>
#include <QBuffer>
#include <QImage>

// ==================== 构造 & 析构 ====================

ClipboardManager::ClipboardManager(QObject *parent)
    : QObject(parent)
    , m_clipboard(QApplication::clipboard())
    , m_debounceTimer(new QTimer(this))
{
    // 设置防抖定时器为单次触发模式
    // 每次剪贴板变化时重启定时器，只有变化停歇后才真正记录
    m_debounceTimer->setSingleShot(true);
    m_debounceTimer->setInterval(DEBOUNCE_DELAY_MS);

    // 系统剪贴板变化 → 启动防抖定时器
    connect(m_clipboard, &QClipboard::dataChanged,
            this, &ClipboardManager::onClipboardDataChanged);

    // 防抖定时器超时 → 真正记录本次剪贴板内容
    connect(m_debounceTimer, &QTimer::timeout,
            this, &ClipboardManager::onDebounceTimerTimeout);
}

// ==================== 查询接口 ====================

ClipboardItem ClipboardManager::itemAt(int index) const
{
    if (index >= 0 && index < m_history.size()) {
        return m_history.at(index);
    }
    // 索引越界时返回空的默认 ClipboardItem
    return ClipboardItem();
}

// ==================== 公共槽：增删改查 ====================

void ClipboardManager::removeItem(int index)
{
    if (index < 0 || index >= m_history.size()) {
        return;  // 索引越界，安全忽略
    }
    m_history.removeAt(index);
    emit historyChanged();
}

void ClipboardManager::clearHistory()
{
    if (m_history.isEmpty()) {
        return;  // 已经是空的，无需操作
    }
    m_history.clear();
    emit historyChanged();
}

void ClipboardManager::copyToClipboard(int index)
{
    if (index < 0 || index >= m_history.size()) {
        return;
    }

    const ClipboardItem &item = m_history.at(index);

    // 阻塞 dataChanged 信号，避免写入剪贴板触发新的记录
    m_clipboard->blockSignals(true);

    switch (item.type()) {
    case ClipboardItem::Text:
        m_clipboard->setText(item.textContent());
        break;
    case ClipboardItem::Image:
        m_clipboard->setPixmap(item.imageContent());
        break;
    }

    // 延迟 50ms 后恢复信号，确保 setText/setPixmap 不会自触发
    QTimer::singleShot(50, this, [this]() {
        m_clipboard->blockSignals(false);
    });
}

// ==================== 私有槽：剪贴板监听 ====================

void ClipboardManager::onClipboardDataChanged()
{
    // 每次剪贴板变化时重启防抖定时器
    // 避免用户快速连续 Ctrl+C 导致重复记录
    m_debounceTimer->start();
}

void ClipboardManager::onDebounceTimerTimeout()
{
    // 防抖超时，剪贴板已稳定，读取内容并记录
    const QMimeData *mimeData = m_clipboard->mimeData();
    if (!mimeData) {
        return;  // 剪贴板为空
    }

    // 优先检测图片（图片剪贴板可能也带有文本标签）
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        if (!image.isNull()) {
            addItem(ClipboardItem(QPixmap::fromImage(image)));
            return;
        }
    }

    // 纯文本优先 — 大多数应用（浏览器/Word/VS Code）复制时
    // 剪贴板中同时包含 HTML 和纯文本，纯文本才是用户想要的干净内容
    if (mimeData->hasText()) {
        addItem(ClipboardItem(mimeData->text()));
        return;
    }

    // HTML 作为后备 — 仅当剪贴板没有纯文本时才用 HTML
    // （少数应用可能只提供 HTML 格式）
    if (mimeData->hasHtml()) {
        addItem(ClipboardItem(mimeData->html()));
    }
}

// ==================== 私有方法 ====================

void ClipboardManager::addItem(const ClipboardItem &item)
{
    // --- 去重：与最新一条相同则跳过 ---
    if (!m_history.isEmpty() && m_history.first() == item) {
        return;
    }

    // --- 插入最前面 ---
    m_history.prepend(item);

    // --- 超出容量则淘汰最旧的 ---
    while (m_history.size() > MAX_HISTORY_SIZE) {
        m_history.removeLast();
    }

    // --- 通知 UI ---
    emit itemAdded(0);
    emit historyChanged();
}
