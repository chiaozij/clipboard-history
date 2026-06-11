/**
 * @file HistoryPanel.cpp
 * @brief 剪贴板历史记录浮动面板 — 实现
 *
 * 核心实现包括：
 * - 无边框圆角窗口的创建和裁剪
 * - Windows DWM 亚克力/毛玻璃背景
 * - 搜索实时过滤
 * - 平滑淡入淡出动画
 * - 点击外部/ESC 自动隐藏
 * - 动态创建/刷新列表条目控件
 */

#include "HistoryPanel.h"
#include "HistoryItemWidget.h"
#include "ThemeManager.h"

#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsOpacityEffect>
#include <QGraphicsBlurEffect>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QRegularExpression>

// Windows DWM API（毛玻璃效果需要）
#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

// ==================== 构造 & 析构 ====================

HistoryPanel::HistoryPanel(ClipboardManager *clipboardManager,
                           QWidget *parent)
    : QWidget(parent)
    , m_clipboardManager(clipboardManager)
    , m_searchBox(new QLineEdit(this))
    , m_scrollArea(new QScrollArea(this))
    , m_listContainer(new QWidget(this))
    , m_listLayout(new QVBoxLayout(m_listContainer))
    , m_statusLabel(new QLabel("暂无剪贴板记录", this))
    , m_opacityAnimation(new QPropertyAnimation(this, "windowOpacity", this))
    , m_geometryAnimation(new QPropertyAnimation(this, "geometry", this))
{
    // ===== 窗口属性 =====
    setWindowFlags(
        Qt::FramelessWindowHint   // 无边框
        | Qt::WindowStaysOnTopHint // 置顶
        | Qt::Popup               // 点击外部自动关闭
        | Qt::NoDropShadowWindowHint // 禁用系统阴影（我们自绘）
    );
    setAttribute(Qt::WA_TranslucentBackground);  // 透明背景（用于圆角和毛玻璃）
    setAttribute(Qt::WA_ShowWithoutActivating);   // 显示时不抢焦点

    // 初始透明（动画从0开始）
    setWindowOpacity(0.0);

    // 固定宽度，高度自适应
    setFixedWidth(PANEL_WIDTH);
    setMaximumHeight(PANEL_MAX_HEIGHT);

    // ===== 整体布局 =====
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    // --- 搜索框 ---
    m_searchBox->setPlaceholderText("🔍  搜索历史记录...");
    m_searchBox->setClearButtonEnabled(true);
    m_searchBox->setStyleSheet(ThemeManager::instance()->searchBoxStyle());
    connect(m_searchBox, &QLineEdit::textChanged,
            this, &HistoryPanel::onSearchTextChanged);

    // --- 滚动区域（条目列表） ---
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "  background: transparent;"
        "  border: none;"
        "}"
    );

    // 列表容器
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(4);
    m_listLayout->setAlignment(Qt::AlignTop);
    m_listContainer->setStyleSheet("background: transparent;");

    m_scrollArea->setWidget(m_listContainer);

    // --- 状态标签（无记录时显示） ---
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(ThemeManager::instance()->statusLabelStyle());

    // --- 组装 ---
    mainLayout->addWidget(m_searchBox);
    mainLayout->addWidget(m_scrollArea, 1);
    mainLayout->addWidget(m_statusLabel);

    // ===== 连接数据源信号 =====
    connect(m_clipboardManager, &ClipboardManager::historyChanged,
            this, &HistoryPanel::refreshList);

    // ===== 监听主题切换，刷新样式 =====
    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, [this](ThemeManager::Theme) {
        m_searchBox->setStyleSheet(ThemeManager::instance()->searchBoxStyle());
        m_statusLabel->setStyleSheet(ThemeManager::instance()->statusLabelStyle());
        enableAcrylicBackground();
        update();  // 重绘面板背景
    });

    // ===== 初次加载列表 =====
    refreshList();
}

HistoryPanel::~HistoryPanel()
{
    // m_itemWidgets 的子项由 Qt 对象树自动管理
    // m_clipboardManager 不由此类拥有，不释放
}

// ==================== 显示 & 隐藏 ====================

void HistoryPanel::showNearCursor()
{
    // 1. 先刷新列表
    refreshList();

    // 2. 获取鼠标全局位置
    QPoint cursorPos = QCursor::pos();

    // 3. 获取鼠标所在屏幕的可用区域（排除任务栏）
    QScreen *screen = QGuiApplication::screenAt(cursorPos);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    QRect screenRect = screen->availableGeometry();

    // 4. 计算面板位置：默认在光标右下方偏移
    int x = cursorPos.x() + 10;
    int y = cursorPos.y() + 10;

    // 如果超出右边界，放到光标左边
    if (x + width() > screenRect.right()) {
        x = cursorPos.x() - width() - 10;
    }

    // 如果超出下边界，放到光标上方
    if (y + height() > screenRect.bottom()) {
        y = cursorPos.y() - height() - 10;
    }

    // 确保不超出屏幕左/上边界
    if (x < screenRect.left()) x = screenRect.left() + 5;
    if (y < screenRect.top()) y = screenRect.top() + 5;

    // 5. 设置位置和初始透明度
    move(x, y);
    setWindowOpacity(0.0);
    show();
    raise();
    activateWindow();
    m_searchBox->setFocus();

    // 6. 淡入动画
    m_opacityAnimation->setDuration(ANIM_DURATION_SHOW);
    m_opacityAnimation->setStartValue(0.0);
    m_opacityAnimation->setEndValue(0.95);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_opacityAnimation->start();
}

void HistoryPanel::hideWithAnimation()
{
    // 如果已经隐藏，忽略
    if (!isVisible()) return;

    m_opacityAnimation->setDuration(ANIM_DURATION_HIDE);
    m_opacityAnimation->setStartValue(windowOpacity());
    m_opacityAnimation->setEndValue(0.0);
    m_opacityAnimation->setEasingCurve(QEasingCurve::InCubic);

    // 动画结束后真正 hide()
    connect(m_opacityAnimation, &QPropertyAnimation::finished, this, [this]() {
        if (windowOpacity() < 0.01) {
            hide();
        }
        // 断开此 lambda 连接，避免多次连接积累
        disconnect(m_opacityAnimation, &QPropertyAnimation::finished,
                   this, nullptr);
    });

    m_opacityAnimation->start();
}

// ==================== 列表刷新 ====================

void HistoryPanel::refreshList()
{
    // 清空现有列表控件
    for (HistoryItemWidget *widget : m_itemWidgets) {
        m_listLayout->removeWidget(widget);
        widget->deleteLater();  // 安全删除（延迟到事件循环末尾）
    }
    m_itemWidgets.clear();

    int count = m_clipboardManager->count();

    // 无记录时显示状态提示
    if (count == 0) {
        m_statusLabel->setVisible(true);
        m_scrollArea->setVisible(false);

        // 调整窗口高度
        int contentHeight = 120;
        resize(PANEL_WIDTH, contentHeight);
        return;
    }

    // 有记录时隐藏状态提示
    m_statusLabel->setVisible(false);
    m_scrollArea->setVisible(true);

    // 创建每个条目控件
    for (int i = 0; i < count; ++i) {
        ClipboardItem item = m_clipboardManager->itemAt(i);
        HistoryItemWidget *itemWidget = new HistoryItemWidget(item, i, m_listContainer);

        // 连接信号
        connect(itemWidget, &HistoryItemWidget::copyRequested,
                this, &HistoryPanel::onItemCopyRequested);
        connect(itemWidget, &HistoryItemWidget::deleteRequested,
                this, &HistoryPanel::onItemDeleteRequested);

        m_listLayout->addWidget(itemWidget);
        m_itemWidgets.append(itemWidget);
    }

    // 在列表末尾添加弹性空间
    m_listLayout->addStretch();

    // 动态调整窗口高度
    int contentHeight = count * 60 + 80;  // 每条 ~60px + 搜索框
    contentHeight = qMin(contentHeight, PANEL_MAX_HEIGHT);
    resize(PANEL_WIDTH, contentHeight);

    // 应用搜索过滤（如果搜索框当前有文本）
    if (!m_searchBox->text().isEmpty()) {
        filterItems(m_searchBox->text());
    }
}

// ==================== 搜索过滤 ====================

void HistoryPanel::onSearchTextChanged(const QString &text)
{
    filterItems(text.trimmed());
}

void HistoryPanel::filterItems(const QString &searchText)
{
    for (HistoryItemWidget *widget : m_itemWidgets) {
        // 获取该条目控件的预览文本（从其子 QLabel 中取得）
        QList<QLabel *> labels = widget->findChildren<QLabel *>();
        bool match = false;

        if (searchText.isEmpty()) {
            match = true;  // 搜索文本为空 → 全部显示
        } else {
            // 遍历所有 QLabel，检查是否包含搜索关键字
            for (QLabel *label : labels) {
                if (label->text().contains(searchText, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
        }

        widget->setVisible(match);
    }

    // 更新列表序号（因为某些条目被隐藏后序号应重新排列）
    int visibleCount = 0;
    for (HistoryItemWidget *widget : m_itemWidgets) {
        if (widget->isVisible()) {
            visibleCount++;
        }
    }

    // 如果搜索无结果，显示提示
    if (visibleCount == 0 && !searchText.isEmpty() && !m_itemWidgets.isEmpty()) {
        m_statusLabel->setText("无匹配结果");
        m_statusLabel->setVisible(true);
    } else if (m_itemWidgets.isEmpty()) {
        m_statusLabel->setText("暂无剪贴板记录");
        m_statusLabel->setVisible(true);
    } else {
        m_statusLabel->setVisible(false);
    }
}

// ==================== 条目操作槽 ====================

void HistoryPanel::onItemCopyRequested(int index)
{
    m_clipboardManager->copyToClipboard(index);
    // 复制后面板自动隐藏
    hideWithAnimation();
}

void HistoryPanel::onItemDeleteRequested(int index)
{
    m_clipboardManager->removeItem(index);
    // refreshList() 会由 historyChanged 信号自动触发
}

// ==================== Qt 事件重写 ====================

void HistoryPanel::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event)
    // 失去焦点 → 自动隐藏
    hideWithAnimation();
}

void HistoryPanel::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        // ESC 键 → 隐藏面板
        hideWithAnimation();
    } else if (event->key() == Qt::Key_Return
               || event->key() == Qt::Key_Enter) {
        // 回车键 → 如果有可见条目，复制第一个
        for (HistoryItemWidget *widget : m_itemWidgets) {
            if (widget->isVisible()) {
                onItemCopyRequested(widget->index());
                break;
            }
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void HistoryPanel::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    // 每次显示面板时更新圆角裁剪区域
    updateRoundedMask();
    // 尝试启用毛玻璃效果
    enableAcrylicBackground();
}

void HistoryPanel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制圆角矩形背景 — 颜色来自 ThemeManager
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);

    // 解析主题背景颜色（格式: "rgba(r, g, b, a)"）
    QString bgStr = ThemeManager::instance()->panelBackgroundColor();
    QColor bgColor;
    // 简单解析 rgba(r,g,b,a) 格式
    QRegularExpression re("rgba\\((\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
    QRegularExpressionMatch m = re.match(bgStr);
    if (m.hasMatch()) {
        bgColor = QColor(m.captured(1).toInt(),
                         m.captured(2).toInt(),
                         m.captured(3).toInt(),
                         m.captured(4).toInt());
    } else {
        bgColor = QColor(32, 32, 32, 235);  // 退化
    }

    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    // 绘制 1px 边框
    painter.setBrush(Qt::NoBrush);
    QString borderStr = ThemeManager::instance()->panelBorderColor();
    QColor borderColor;
    QRegularExpression re2("rgba\\((\\d+),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
    QRegularExpressionMatch m2 = re2.match(borderStr);
    if (m2.hasMatch()) {
        borderColor = QColor(m2.captured(1).toInt(),
                             m2.captured(2).toInt(),
                             m2.captured(3).toInt(),
                             m2.captured(4).toInt());
    } else {
        borderColor = QColor(255, 255, 255, 30);
    }

    QPen borderPen(borderColor);
    borderPen.setWidth(1);
    painter.setPen(borderPen);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 12, 12);
}

void HistoryPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateRoundedMask();
}

// ==================== 毛玻璃效果 ====================

void HistoryPanel::enableAcrylicBackground()
{
#ifdef Q_OS_WIN
    // 使用 Windows DWM API 设置 Acrylic 毛玻璃效果
    HWND hwnd = reinterpret_cast<HWND>(winId());

    // 方式 1：DWM 扩展边框（Mica 效果基础）
    MARGINS margins = {0, 0, 0, 0};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // 方式 2：设置 Acrylic 材质（Win10 1803+ / Win11）
    // DWM_SYSTEMBACKDROP_TYPE: 0=None, 1=Mica, 2=Acrylic, 3=Tabbed
    // 使用 DWMWA_SYSTEMBACKDROP_TYPE (38)
    const int DWM_SYSTEMBACKDROP_ACRYLIC = 2;
    DwmSetWindowAttribute(hwnd,
                          38,  // DWMWA_SYSTEMBACKDROP_TYPE
                          &DWM_SYSTEMBACKDROP_ACRYLIC,
                          sizeof(int));

    // 方式 3：暗色/亮色模式适配（跟随主题）
    BOOL useDarkMode = ThemeManager::instance()->useDarkMode() ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd,
                          20,  // DWMWA_USE_IMMERSIVE_DARK_MODE
                          &useDarkMode,
                          sizeof(BOOL));
#endif
    // 非 Windows 平台不做任何处理（退化为 paintEvent 中的半透明绘制）
}

// ==================== 圆角裁剪 ====================

void HistoryPanel::updateRoundedMask()
{
    // 使用 QPainterPath 创建圆角矩形区域
    QPainterPath path;
    path.addRoundedRect(rect(), 12, 12);

    // setMask 将窗口以外的区域变为透明
    QRegion region = QRegion(path.toFillPolygon().toPolygon());
    setMask(region);
}
