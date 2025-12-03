#include "utils.h"
#include <QDebug>
#include <QMessageBox>
#include <QPainterPath>
#include <QtMath>
#include <QBoxLayout>

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
            onItemDroppedIn(text);
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
    setMask(m_poly);
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
    p.setBrush(QColor("#007AFF")); // Blue

    p.drawPath(path);
    p.restore();

    // Text rendering logic remains same (optional text)
    if (!m_text.isEmpty()) {
        double radians = qDegreesToRadians((double)m_angle);
        double tailDist = arrowLen / 2.0 + 10.0;
        double tx = cx - tailDist * qCos(radians);
        double ty = cy - tailDist * qSin(radians);

        QRect txtRect(tx - 100, ty - 25, 200, 50);

        p.setPen(m_color);
        QFont f = font();
        f.setBold(true);
        f.setPointSize(m_textSize);
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
    lblText->setStyleSheet("color: #333; font-size: 16px; font-weight: bold; border: none;");
    lblText->setAlignment(Qt::AlignCenter);
    vbox->addWidget(lblText);

    vbox->addSpacing(20);

    // Close Button
    QPushButton *btnClose = new QPushButton("我知道了", container);
    btnClose->setCursor(Qt::PointingHandCursor);
    btnClose->setStyleSheet(
        "QPushButton { background-color: #007AFF; color: white; border-radius: 5px; padding: 8px 20px; font-size: 14px; }"
        "QPushButton:hover { background-color: #0062cc; }"
    );
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
