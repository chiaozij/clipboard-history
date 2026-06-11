/**
 * @file ClipboardItem.cpp
 * @brief 剪贴板单条历史记录数据结构 — 实现
 */

#include "ClipboardItem.h"
#include <QBuffer>

// ==================== 构造函数 ====================

ClipboardItem::ClipboardItem()
    : m_timestamp(QDateTime::currentDateTime())
    , m_type(Text)
    , m_textContent(QString())
{
    // 创建一个空的文本记录
}

ClipboardItem::ClipboardItem(const QString &text)
    : m_timestamp(QDateTime::currentDateTime())
    , m_type(Text)
    , m_textContent(text)
{
    // 从文本创建记录，自动记录当前时间
}

ClipboardItem::ClipboardItem(const QPixmap &pixmap)
    : m_timestamp(QDateTime::currentDateTime())
    , m_type(Image)
    , m_imageContent(pixmap)
{
    // 从图片创建记录，自动记录当前时间
}

// ==================== 预览生成 ====================

QString ClipboardItem::previewText(int maxLength) const
{
    if (m_type == Image) {
        return "[图片]";
    }

    // 文本类型：截取前 maxLength 个字符
    QString text = m_textContent;

    // 将换行符替换为空格，确保单行显示
    text.replace('\n', ' ');
    text.replace('\r', ' ');
    text.replace('\t', ' ');

    // 压缩连续空格
    while (text.contains("  ")) {
        text.replace("  ", " ");
    }

    // 去除首尾空格
    text = text.trimmed();

    // 截取指定长度
    if (text.length() > maxLength) {
        text = text.left(maxLength) + "…";
    }

    return text.isEmpty() ? "[空文本]" : text;
}

QPixmap ClipboardItem::previewImage(int size) const
{
    if (m_type == Text || m_imageContent.isNull()) {
        return QPixmap();
    }

    // 使用平滑缩放生成缩略图
    return m_imageContent.scaled(size, size,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);
}

// ==================== 比较运算符 ====================

bool ClipboardItem::operator==(const ClipboardItem &other) const
{
    // 类型不同，内容必然不同
    if (m_type != other.m_type) {
        return false;
    }

    if (m_type == Text) {
        // 文本类型：精确比较文本内容（区分大小写）
        return m_textContent == other.m_textContent;
    } else {
        // 图片类型：比较像素数据
        // 使用 QImage 的 byteCount 和逐像素比较
        if (m_imageContent.isNull() && other.m_imageContent.isNull()) {
            return true;
        }
        if (m_imageContent.isNull() || other.m_imageContent.isNull()) {
            return false;
        }

        // 将两个 QPixmap 转为 QImage 进行逐像素比较
        QImage img1 = m_imageContent.toImage();
        QImage img2 = other.m_imageContent.toImage();

        // 尺寸不同则不相等
        if (img1.size() != img2.size()) {
            return false;
        }

        // 逐像素比较
        return img1 == img2;
    }
}
