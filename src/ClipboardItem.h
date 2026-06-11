/**
 * @file ClipboardItem.h
 * @brief 剪贴板单条历史记录的数据结构定义
 *
 * 每条记录保存用户复制到剪贴板的内容，支持文本和图片两种类型。
 * 提供预览文本/缩略图生成、内容比较（用于去重）等功能。
 */

#ifndef CLIPBOARDITEM_H
#define CLIPBOARDITEM_H

#include <QDateTime>
#include <QPixmap>
#include <QString>

/**
 * @class ClipboardItem
 * @brief 单条剪贴板历史记录
 *
 * 存储用户复制到系统剪贴板的一条内容。
 * 支持文本和图片两种类型，自动记录复制时间。
 * 提供预览文本（截取前80字符）和预览缩略图（48x48）用于列表显示。
 */
class ClipboardItem
{
public:
    /**
     * @brief 内容类型枚举
     */
    enum Type {
        Text,  ///< 文本内容
        Image  ///< 图片内容
    };

    /**
     * @brief 默认构造函数，创建一个空的文本记录
     */
    ClipboardItem();

    /**
     * @brief 从文本构造一条记录
     * @param text 剪贴板中的文本内容
     */
    explicit ClipboardItem(const QString &text);

    /**
     * @brief 从图片构造一条记录
     * @param pixmap 剪贴板中的图片
     */
    explicit ClipboardItem(const QPixmap &pixmap);

    // ==================== 基本访问器 ====================

    /// 获取记录创建时间
    QDateTime timestamp() const { return m_timestamp; }

    /// 获取内容类型（文本或图片）
    Type type() const { return m_type; }

    /// 获取文本内容（仅在 type() == Text 时有效）
    QString textContent() const { return m_textContent; }

    /// 获取图片内容（仅在 type() == Image 时有效）
    QPixmap imageContent() const { return m_imageContent; }

    // ==================== 预览相关 ====================

    /**
     * @brief 生成文本预览
     * @param maxLength 预览最大字符数，默认 80
     * @return 截取后的文本，换行符会被替换为空格以单行显示
     *
     * 对于文本条目，截取前 maxLength 个字符用于列表预览。
     * 换行符会被替换为空格，确保预览在列表中单行显示。
     * 对于图片条目，返回 "[图片]" 标签。
     */
    QString previewText(int maxLength = 80) const;

    /**
     * @brief 生成图片缩略图
     * @param size 缩略图边长，默认 48px
     * @return 缩放后的缩略图
     *
     * 使用 Qt::SmoothTransformation 保证缩放质量。
     * 对于文本条目，返回空 QPixmap。
     */
    QPixmap previewImage(int size = 48) const;

    // ==================== 比较运算符 ====================

    /**
     * @brief 相等比较（用于去重）
     * @param other 要比较的另一条记录
     * @return 内容相同返回 true
     *
     * 文本条目：比较文本字符串（区分大小写）
     * 图片条目：比较像素数据是否完全相同
     */
    bool operator==(const ClipboardItem &other) const;

    /**
     * @brief 不等比较
     */
    bool operator!=(const ClipboardItem &other) const { return !(*this == other); }

private:
    QDateTime m_timestamp;    ///< 复制时间戳
    Type m_type;              ///< 内容类型
    QString m_textContent;    ///< 文本内容
    QPixmap m_imageContent;   ///< 图片内容
};

#endif // CLIPBOARDITEM_H
