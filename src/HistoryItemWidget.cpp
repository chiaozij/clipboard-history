/**
 * @file HistoryItemWidget.cpp
 * @brief 历史面板中单条记录的 UI 组件 — 实现
 *
 * 每条记录呈现为一张圆角卡片：
 * - 左侧：类型图标/缩略图（48x48）
 * - 中间：预览文本 + 时间戳
 * - 右侧：复制按钮
 * - 支持 hover 高亮效果
 * - 左键点击复制，右键弹出上下文菜单（复制/删除）
 */

#include "HistoryItemWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include <QToolTip>
#include <QAbstractButton>
#include "ThemeManager.h"

// ==================== 构造 & 析构 ====================

HistoryItemWidget::HistoryItemWidget(const ClipboardItem &item,
                                     int index,
                                     QWidget *parent)
    : QFrame(parent)
    , m_index(index)
    , m_item(item)
    , m_iconLabel(new QLabel(this))
    , m_previewLabel(new QLabel(this))
    , m_timeLabel(new QLabel(this))
    , m_copyBtn(new QPushButton(this))
    , m_contextMenu(new QMenu(this))
{
    setupUi(item);

    // 右键菜单 — 复制 与 删除
    QAction *copyAction = m_contextMenu->addAction("📋 复制");
    connect(copyAction, &QAction::triggered,
            this, &HistoryItemWidget::onCopyClicked);

    QAction *deleteAction = m_contextMenu->addAction("🗑 删除");
    deleteAction->setIcon(QIcon());  // 不设图标保持简洁
    connect(deleteAction, &QAction::triggered,
            this, &HistoryItemWidget::onDeleteActionTriggered);

    // 为右键菜单应用主题样式
    m_contextMenu->setStyleSheet(ThemeManager::instance()->menuStyle());

    // 每次菜单弹出前刷新样式（确保主题切换后也正确）
    connect(m_contextMenu, &QMenu::aboutToShow, this, [this]() {
        m_contextMenu->setStyleSheet(ThemeManager::instance()->menuStyle());
    });

    // 设置鼠标追踪，用于 hover 效果
    setMouseTracking(true);
}

// ==================== UI 初始化 ====================

void HistoryItemWidget::setupUi(const ClipboardItem &item)
{
    // --- 卡片外观 ---
    setFixedHeight(56);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(
        ThemeManager::instance()->cardNormalStyle()
        + ThemeManager::instance()->cardHoverStyle()
    );
    setCursor(Qt::PointingHandCursor);

    // --- 布局结构 ---
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 6, 10, 6);
    mainLayout->setSpacing(10);

    // ── 左侧：图标/缩略图 ──
    m_iconLabel->setFixedSize(40, 40);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setStyleSheet(ThemeManager::instance()->cardIconStyle());
    setTypeIcon(item);

    // ── 中间：预览文本 + 时间 ──
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);

    m_previewLabel->setText(item.previewText(80));
    m_previewLabel->setStyleSheet(
        "QLabel {"
        "  color: " + ThemeManager::instance()->cardTextColor() + ";"
        "  font-size: 13px;"
        "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "}"
    );
    m_previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    // 单行显示，超出省略
    m_previewLabel->setWordWrap(false);
    m_previewLabel->setTextFormat(Qt::PlainText);

    m_timeLabel->setText(formatTimestamp(item.timestamp()));
    m_timeLabel->setStyleSheet(
        "QLabel {"
        "  color: " + ThemeManager::instance()->cardTimeColor() + ";"
        "  font-size: 11px;"
        "  font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;"
        "}"
    );

    textLayout->addWidget(m_previewLabel);
    textLayout->addWidget(m_timeLabel);

    // ── 右侧：复制按钮 ──
    m_copyBtn->setText("📋");
    m_copyBtn->setFixedSize(32, 32);
    m_copyBtn->setCursor(Qt::PointingHandCursor);
    m_copyBtn->setToolTip("复制到剪贴板");
    m_copyBtn->setStyleSheet(
        ThemeManager::instance()->cardCopyBtnStyle()
        + ThemeManager::instance()->cardCopyBtnHoverStyle()
    );
    connect(m_copyBtn, &QPushButton::clicked,
            this, &HistoryItemWidget::onCopyClicked);

    // ── 组装 ──
    mainLayout->addWidget(m_iconLabel);
    mainLayout->addLayout(textLayout, 1);
    mainLayout->addWidget(m_copyBtn);
}

// ==================== 图标生成 ====================

void HistoryItemWidget::setTypeIcon(const ClipboardItem &item)
{
    if (item.type() == ClipboardItem::Image) {
        // 图片类型：显示缩略图
        QPixmap thumb = item.previewImage(40);
        if (!thumb.isNull()) {
            m_iconLabel->setPixmap(thumb);
            return;
        }
        // 缩略图生成失败时显示占位符
        m_iconLabel->setText("🖼");
        return;
    }

    // 文本类型：显示文本图标
    m_iconLabel->setText("📝");
    m_iconLabel->setStyleSheet(
        ThemeManager::instance()->cardIconStyle()
        + "font-size: 20px;"
    );
}

// ==================== 时间格式化 ====================

QString HistoryItemWidget::formatTimestamp(const QDateTime &dt)
{
    if (!dt.isValid()) {
        return QString();
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 secsAgo = dt.secsTo(now);

    // 刚刚（60秒内）
    if (secsAgo < 60) {
        return "刚刚";
    }

    // X分钟前（60分钟内）
    if (secsAgo < 3600) {
        return QString("%1分钟前").arg(secsAgo / 60);
    }

    // 今天：显示 HH:mm
    if (dt.date() == now.date()) {
        return dt.toString("HH:mm");
    }

    // 昨天：显示 "昨天 HH:mm"
    if (dt.date() == now.date().addDays(-1)) {
        return QString("昨天 %1").arg(dt.toString("HH:mm"));
    }

    // 更早：显示 "MM-dd HH:mm"
    return dt.toString("MM-dd HH:mm");
}

// ==================== 事件处理 ====================

void HistoryItemWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 左键点击 → 复制到剪贴板
        onCopyClicked();
    } else if (event->button() == Qt::RightButton) {
        // 右键 → 弹出上下文菜单
        m_contextMenu->popup(event->globalPosition().toPoint());
    }

    // 不调用 QFrame::mousePressEvent(event)，由我们自己处理
}

void HistoryItemWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    // hover 高亮通过样式表中的 :hover 伪类实现
    // 此处留空，仅用于可能的未来扩展
}

void HistoryItemWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    // 样式表中的 :hover 自动处理恢复
}

// ==================== 私有槽 ====================

void HistoryItemWidget::onCopyClicked()
{
    emit copyRequested(m_index);
}

void HistoryItemWidget::onDeleteActionTriggered()
{
    // 弹出确认对话框
    QMessageBox msgBox;
    msgBox.setWindowTitle("删除确认");
    msgBox.setText("确定要删除这条记录吗？");
    msgBox.setInformativeText("删除后无法恢复。");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    // 设置按钮文字
    msgBox.button(QMessageBox::Yes)->setText("删除");
    msgBox.button(QMessageBox::No)->setText("取消");

    // 设置对话框样式，与当前主题一致
    msgBox.setStyleSheet(ThemeManager::instance()->dialogStyle());

    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        emit deleteRequested(m_index);
    }
}
