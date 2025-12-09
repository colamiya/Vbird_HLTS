#include "utils.h"
#include <QDebug>
#include <QMessageBox>
#include <QPainterPath>
#include <QtMath>
#include <QBoxLayout>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include "config_global.h"
#include "config_text.h" // Needed for styles

// --- Namespace Utils Implementation ---
namespace Utils {

    // 辅助函数：创建样式化按钮
    QPushButton* CreateStyledButton(const QString &text, const QString &bgColor = "") {
        QPushButton *btn = new QPushButton(text);

        // 尺寸设置 (从 Config::Global 读取)
        int minW = Config::Global::DIALOG_BTN_MIN_WIDTH;
        int minH = Config::Global::DIALOG_BTN_MIN_HEIGHT;
        btn->setMinimumSize(minW, minH);

        // 样式
        QString bg = bgColor.isEmpty() ? Config::Global::COL_BTN_PRIMARY : bgColor;
        QString style = QString(
            "QPushButton { "
            "  background-color: %1; "
            "  color: white; "
            "  border-radius: 6px; "
            "  font-size: %2px; "
            "  font-weight: bold; "
            "  padding: 5px 15px; "
            "} "
            "QPushButton:hover { background-color: %3; } "
            "QPushButton:pressed { background-color: %4; }"
        )
        .arg(bg)
        .arg(Config::Global::DIALOG_BTN_FONT_SIZE)
        .arg(Config::Global::COL_BTN_HOVER) // Simple hover darkening approximation or use config
        .arg(Config::Global::COL_BTN_HOVER);

        btn->setStyleSheet(style);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    }

    void ShowCustomMessageBox(QWidget *parent, const QString &title, const QString &content, bool isWarning) {
        QDialog dlg(parent);
        dlg.setWindowTitle(title);
        dlg.setModal(true);
        // 去掉问号，保留关闭按钮
        dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);

        // 设置最小宽度，避免太窄
        dlg.setMinimumWidth(400);

        QVBoxLayout *layout = new QVBoxLayout(&dlg);
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);

        // 图标 + 内容 (水平布局)
        QHBoxLayout *contentLayout = new QHBoxLayout();

        // 图标 (使用标准图标)
        QLabel *iconLabel = new QLabel();
        QStyle::StandardPixmap iconType = isWarning ? QStyle::SP_MessageBoxWarning : QStyle::SP_MessageBoxInformation;
        QIcon icon = dlg.style()->standardIcon(iconType);
        iconLabel->setPixmap(icon.pixmap(48, 48));
        iconLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QLabel *textLabel = new QLabel(content);
        textLabel->setWordWrap(true);
        textLabel->setStyleSheet(QString("font-size: %1px; color: #333;").arg(Config::Text::SIZE_TEST3_DIALOG_TEXT));
        textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        contentLayout->addWidget(iconLabel);
        contentLayout->addSpacing(15);
        contentLayout->addWidget(textLabel, 1);

        layout->addLayout(contentLayout);

        // 按钮区域
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch();

        QPushButton *okBtn = CreateStyledButton("确定");
        QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

        btnLayout->addWidget(okBtn);
        btnLayout->addStretch(); // Center the button

        layout->addLayout(btnLayout);

        dlg.exec();
    }

    bool ShowCustomConfirmDialog(QWidget *parent, const QString &title, const QString &content) {
        QDialog dlg(parent);
        dlg.setWindowTitle(title);
        dlg.setModal(true);
        dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dlg.setMinimumWidth(400);

        QVBoxLayout *layout = new QVBoxLayout(&dlg);
        layout->setSpacing(20);
        layout->setContentsMargins(20, 20, 20, 20);

        // 图标 + 内容
        QHBoxLayout *contentLayout = new QHBoxLayout();

        QLabel *iconLabel = new QLabel();
        QIcon icon = dlg.style()->standardIcon(QStyle::SP_MessageBoxQuestion);
        iconLabel->setPixmap(icon.pixmap(48, 48));
        iconLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

        QLabel *textLabel = new QLabel(content);
        textLabel->setWordWrap(true);
        textLabel->setStyleSheet(QString("font-size: %1px; color: #333;").arg(Config::Text::SIZE_TEST3_DIALOG_TEXT));

        contentLayout->addWidget(iconLabel);
        contentLayout->addSpacing(15);
        contentLayout->addWidget(textLabel, 1);

        layout->addLayout(contentLayout);

        // 按钮 (Yes / No)
        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch();

        QPushButton *yesBtn = CreateStyledButton("是 (Yes)");
        QPushButton *noBtn = CreateStyledButton("否 (No)", "#95a5a6"); // Grey for No

        QObject::connect(yesBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(noBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

        btnLayout->addWidget(yesBtn);
        btnLayout->addSpacing(20);
        btnLayout->addWidget(noBtn);
        btnLayout->addStretch();

        layout->addLayout(btnLayout);

        return (dlg.exec() == QDialog::Accepted);
    }
}


// --- DragSourceLabel ---
DragSourceLabel::DragSourceLabel(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName)
{
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor);
    setToolTip("拖拽 " + itemName);
}

void DragSourceLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        drag->setMimeData(mimeData);

        QPixmap pixmap(100, 30);
        pixmap.fill(Qt::lightGray);
        QPainter painter(&pixmap);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, m_itemName);
        drag->setPixmap(pixmap);

        drag->exec(Qt::CopyAction);
    }
}

// --- DropLabel ---
DropLabel::DropLabel(const QString &text, QWidget *parent) : QLabel(text, parent)
{
    setAcceptDrops(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(
        "border: 2px dashed #95a5a6;"
        "border-radius: 8px;"
        "background-color: rgba(236, 240, 241, 0.8);"
        "color: #7f8c8d;"
        "font-weight: bold;"
        "font-size: 14px;");
}

void DropLabel::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void DropLabel::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        if (onDropCallback)
        {
            onDropCallback(text);
        }
        event->acceptProposedAction();
    }
}

// --- DraggableListWidget ---
DraggableListWidget::DraggableListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::CopyAction);
    setCursor(Qt::PointingHandCursor);
}

void DraggableListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (item)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->data(Qt::UserRole).toString());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        QPixmap pixmap = item->icon().pixmap(50, 50);
        if (pixmap.isNull())
        {
            pixmap = QPixmap(100, 30);
            pixmap.fill(Qt::lightGray);
            QPainter painter(&pixmap);
            painter.drawText(pixmap.rect(), Qt::AlignCenter, item->text());
        }
        drag->setPixmap(pixmap);

        drag->exec(Qt::MoveAction);
    }
}

void DraggableListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dragEnterEvent(event);
    }
}

void DraggableListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dragMoveEvent(event);
    }
}

void DraggableListWidget::dropEvent(QDropEvent *event)
{
    if (event->source() != this && event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        if (onItemDroppedIn)
        {
            onItemDroppedIn(text, event->mimeData());
        }
        event->acceptProposedAction();
    }
    else
    {
        QListWidget::dropEvent(event);
    }
}

// --- ClickableArea (Upgraded to support Drops) ---
ClickableArea::ClickableArea(QWidget *parent) : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground);
    setAcceptDrops(true); // Enable Drag & Drop support
}

void ClickableArea::setPolygon(const QPolygon &poly)
{
    m_poly = poly;
    // 存储原始多边形 (如果是第一次设置)
    if (m_originalPoly.isEmpty()) {
        m_originalPoly = poly;
    }
    setMask(m_poly);
}

void ClickableArea::rescale(float scaleX, float scaleY)
{
    if (m_originalPoly.isEmpty()) return;

    QPolygon scaledPoly;
    for (const QPoint &pt : m_originalPoly) {
        scaledPoly << QPoint(pt.x() * scaleX, pt.y() * scaleY);
    }
    setPolygon(scaledPoly);
}

void ClickableArea::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
    event->ignore();
}

void ClickableArea::enterEvent(QEnterEvent *event)
{
    emit hovered(true, toolTip());
    QWidget::enterEvent(event);
}

void ClickableArea::leaveEvent(QEvent *event)
{
    emit hovered(false, "");
    QWidget::leaveEvent(event);
}

void ClickableArea::paintEvent(QPaintEvent *)
{
    // Invisible
}

void ClickableArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void ClickableArea::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        if (onDropCallback)
        {
            onDropCallback(text);
        }
        event->acceptProposedAction();
    }
}

// --- ShelfArea ---
ShelfArea::ShelfArea(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName), m_isDraggable(true)
{
    setAcceptDrops(true);
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor);
    setToolTip(itemName);
}

void ShelfArea::setDraggable(bool enabled)
{
    m_isDraggable = enabled;
    setCursor(m_isDraggable ? Qt::PointingHandCursor : Qt::ArrowCursor);
}

void ShelfArea::mousePressEvent(QMouseEvent *event)
{
    if (!m_isDraggable)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        if (!m_sourceType.isEmpty())
        {
            mimeData->setData("application/x-source", m_sourceType.toUtf8());
        }
        drag->setMimeData(mimeData);

        QPixmap pixmap(100, 30);
        pixmap.fill(Qt::lightGray);
        QPainter painter(&pixmap);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, m_itemName);
        drag->setPixmap(pixmap);

        drag->exec(Qt::CopyAction);
    }
}

void ShelfArea::enterEvent(QEnterEvent *event)
{
    emit hovered(true, toolTip());
    QLabel::enterEvent(event);
}

void ShelfArea::leaveEvent(QEvent *event)
{
    emit hovered(false, "");
    QLabel::leaveEvent(event);
}

void ShelfArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
    }
}

void ShelfArea::dropEvent(QDropEvent *event)
{
    if (event->source() == this)
    {
        event->ignore();
        return;
    }

    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        if (text == m_itemName)
        {
            if (onDropCallback)
                onDropCallback(text);
            event->acceptProposedAction();
        }
    }
}

// --- ArrowButton (Modified: Blue Solid Arrow) ---
ArrowButton::ArrowButton(QWidget *parent)
    : QPushButton(parent), m_angle(0), m_color(Qt::blue), m_textSize(14)
{
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
    setStyleSheet("background: transparent; border: none;");
}

void ArrowButton::setAngle(int degrees)
{
    m_angle = degrees;
    update();
}

void ArrowButton::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void ArrowButton::setArrowText(const QString &text)
{
    m_text = text;
    update();
}

void ArrowButton::setArrowTextSize(int size)
{
    m_textSize = size;
    update();
}

bool ArrowButton::hitButton(const QPoint &pos) const
{
    return QPushButton::hitButton(pos);
}

void ArrowButton::enterEvent(QEnterEvent *event)
{
    emit hovered(true, m_text);
    QPushButton::enterEvent(event);
}

void ArrowButton::leaveEvent(QEvent *event)
{
    emit hovered(false, "");
    QPushButton::leaveEvent(event);
}

void ArrowButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (!p.isActive())
        return;

    p.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();
    if (w <= 0 || h <= 0)
        return;

    int cx = w / 2;
    int cy = h / 2;

    p.save();
    p.translate(cx, cy);
    p.rotate(m_angle);

    int arrowLen = qMin(w, h) * 0.8;
    int headLen = arrowLen * 0.4;
    int shaftThick = arrowLen * 0.3;

    QPainterPath path;
    path.moveTo(arrowLen / 2, 0);
    path.lineTo(arrowLen / 2 - headLen, -arrowLen / 2 * 0.5);
    path.lineTo(arrowLen / 2 - headLen, -shaftThick / 2);
    path.lineTo(-arrowLen / 2, -shaftThick / 2);
    path.lineTo(-arrowLen / 2, shaftThick / 2);
    path.lineTo(arrowLen / 2 - headLen, shaftThick / 2);
    path.lineTo(arrowLen / 2 - headLen, arrowLen / 2 * 0.5);
    path.closeSubpath();

    // Changed: Solid Blue, No Red Border
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#007AFF")); // Blue, fixed for now as graphic style.
    // If arrow color should be configurable, it should be passed in or read from config.
    // Given the request is about text, we keep the graphic as is,
    // but we ensure TEXT color is m_color.

    p.drawPath(path);
    p.restore();

    // Text rendering logic
    if (!m_text.isEmpty()) {
        double radians = qDegreesToRadians((double)m_angle);
        double tailDist = arrowLen / 2.0 + 10.0;
        double tx = cx - tailDist * qCos(radians);
        double ty = cy - tailDist * qSin(radians);

        int txtW = 300;
        int txtH = 100;
        QRect txtRect(tx - txtW/2, ty - txtH/2, txtW, txtH);

        p.setPen(m_color); // Use configured color (set via setColor)
        QFont f = font();
        f.setBold(true);
        if (f.pointSize() <= 0) f.setPointSize(m_textSize);
        p.setFont(f);
        p.drawText(txtRect, Qt::AlignCenter, m_text);
    }
}

// --- SpeechBubble ---
SpeechBubble::SpeechBubble(const QString &text, QWidget *parent) : QWidget(parent), m_text(text)
{
    // Make it cover the parent (modal overlay effect)
    if (parent) {
        this->setGeometry(parent->rect());
    }
    // Block mouse input to underlying widgets
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // Bubble Container
    QWidget *container = new QWidget(this);
    container->setFixedWidth(400); // Fixed width for the bubble

    // Configurable style for bubble text
    container->setStyleSheet(
        "background-color: white;"
        "border-radius: 15px;"
        "border: 2px solid #007AFF;" // Blue border
        "padding: 20px;"
    );

    QVBoxLayout *vbox = new QVBoxLayout(container);

    // Text Label
    QLabel *lblText = new QLabel(m_text, container);
    lblText->setWordWrap(true);

    // Inject Config Values
    QString textStyle = QString("color: %1; font-size: %2px; font-weight: bold; border: none;")
        .arg(Config::Text::COLOR_TEST3_TUTORIAL_TEXT)
        .arg(Config::Text::SIZE_TEST3_TUTORIAL_TEXT);

    lblText->setStyleSheet(textStyle);
    lblText->setAlignment(Qt::AlignCenter);
    vbox->addWidget(lblText);

    vbox->addSpacing(20);

    // Close Button
    QPushButton *btnClose = new QPushButton("我知道了", container);
    btnClose->setCursor(Qt::PointingHandCursor);
    // Reuse tutorial btn style or define new
    QString btnStyle = QString(
        "QPushButton { background-color: #007AFF; color: white; border-radius: 5px; padding: 8px 20px; font-size: %1px; }"
        "QPushButton:hover { background-color: #0062cc; }")
        .arg(Config::Text::SIZE_TEST3_TUTORIAL_BTN); // Approximate

    btnClose->setStyleSheet(btnStyle);
    connect(btnClose, &QPushButton::clicked, this, &QWidget::close);
    connect(btnClose, &QPushButton::clicked, this, &QObject::deleteLater);

    vbox->addWidget(btnClose, 0, Qt::AlignCenter);

    layout->addWidget(container);
}

void SpeechBubble::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    // Semi-transparent dark overlay background
    p.fillRect(rect(), QColor(0, 0, 0, 100));
}
