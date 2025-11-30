#include "utils.h"
#include <QDebug>
#include <QMessageBox>
#include <QPainterPath>
#include <QtMath> // Required for qDegreesToRadians, qCos, qSin

// --- DragSourceLabel ---
DragSourceLabel::DragSourceLabel(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName) {
    // Transparent style for overlay
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // Changed to PointingHand
    setToolTip("拖拽 " + itemName);
}

void DragSourceLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        drag->setMimeData(mimeData);

        QPixmap pixmap(100, 30);
        pixmap.fill(Qt::lightGray);
        QPainter painter(&pixmap);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, m_itemName);
        drag->setPixmap(pixmap);

        drag->exec(Qt::CopyAction); // Copy because the source (shelf) has infinite supply
    }
}

// --- DropLabel ---
DropLabel::DropLabel(const QString &text, QWidget *parent) : QLabel(text, parent) {
    setAcceptDrops(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(
        "border: 2px dashed #95a5a6;"
        "border-radius: 8px;"
        "background-color: rgba(236, 240, 241, 0.8);"
        "color: #7f8c8d;"
        "font-weight: bold;"
        "font-size: 14px;"
    );
}

void DropLabel::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void DropLabel::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasText()) {
        QString text = event->mimeData()->text();
        if (onDropCallback) {
            onDropCallback(text);
        }
        event->acceptProposedAction();
    }
}

// --- DraggableListWidget ---
DraggableListWidget::DraggableListWidget(QWidget *parent) : QListWidget(parent) {
    setDragEnabled(true);
    setAcceptDrops(true); // Now also accepts drops (from Warehouse)
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::CopyAction); // Default to copy
    setCursor(Qt::PointingHandCursor); // Set cursor
}

void DraggableListWidget::startDrag(Qt::DropActions supportedActions) {
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->data(Qt::UserRole).toString()); // Item type

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        QPixmap pixmap = item->icon().pixmap(50, 50);
        if (pixmap.isNull()) {
            pixmap = QPixmap(100, 30);
            pixmap.fill(Qt::lightGray);
            QPainter painter(&pixmap);
            painter.drawText(pixmap.rect(), Qt::AlignCenter, item->text());
        }
        drag->setPixmap(pixmap);

        drag->exec(Qt::MoveAction);
    }
}

void DraggableListWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        QListWidget::dragEnterEvent(event);
    }
}

void DraggableListWidget::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasText()) {
         event->acceptProposedAction();
    } else {
         QListWidget::dragMoveEvent(event);
    }
}

void DraggableListWidget::dropEvent(QDropEvent *event) {
    if (event->source() != this && event->mimeData()->hasText()) {
        // Drop from external source (Warehouse)
        QString text = event->mimeData()->text();
        if (onItemDroppedIn) {
            onItemDroppedIn(text);
        }
        event->acceptProposedAction();
    } else {
        // Internal reordering or invalid
        QListWidget::dropEvent(event);
    }
}

// --- ClickableArea ---
ClickableArea::ClickableArea(QWidget *parent) : QWidget(parent) {
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ClickableArea::setPolygon(const QPolygon &poly) {
    m_poly = poly;
    qDebug() << "ClickableArea::setPolygon points:" << poly.count(); // Debug Log
    // Apply Mask to restrict mouse events strictly to the polygon
    setMask(m_poly);
}

void ClickableArea::mousePressEvent(QMouseEvent *event) {
    // With setMask, we only get events inside the polygon.
    // So we can unconditionally accept and emit.
    emit clicked();

    // Allow event to propagate for Dev Mode tracking in parent filter
    event->ignore();
}

void ClickableArea::enterEvent(QEnterEvent *event) {
    qDebug() << "ClickableArea::enterEvent:" << toolTip(); // Debug Log
    emit hovered(true, toolTip());
    QWidget::enterEvent(event);
}

void ClickableArea::leaveEvent(QEvent *event) {
    emit hovered(false, "");
    QWidget::leaveEvent(event);
}

void ClickableArea::paintEvent(QPaintEvent *) {
    // Invisible by default.
}

// --- ShelfArea ---
ShelfArea::ShelfArea(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName), m_isDraggable(true) {
    setAcceptDrops(true); // Target
    // Style
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // Default draggable
    setToolTip(itemName);
}

void ShelfArea::setDraggable(bool enabled) {
    m_isDraggable = enabled;
    if (m_isDraggable) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void ShelfArea::mousePressEvent(QMouseEvent *event) {
    // Only allow drag if explicitly draggable
    if (!m_isDraggable) return;

    if (event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        if (!m_sourceType.isEmpty()) {
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

void ShelfArea::enterEvent(QEnterEvent *event) {
    emit hovered(true, toolTip());
    QLabel::enterEvent(event);
}

void ShelfArea::leaveEvent(QEvent *event) {
    emit hovered(false, "");
    QLabel::leaveEvent(event);
}

void ShelfArea::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasText()) {
        event->acceptProposedAction();
    }
}

void ShelfArea::dropEvent(QDropEvent *event) {
    // Prevent self-drop (dragging from shelf and dropping back on shelf) from triggering logic
    if (event->source() == this) {
        event->ignore();
        return;
    }

    if (event->mimeData()->hasText()) {
        QString text = event->mimeData()->text();
        if (text == m_itemName) {
            if (onDropCallback) onDropCallback(text);
            event->acceptProposedAction();
        }
    }
}

// --- ArrowButton ---
ArrowButton::ArrowButton(QWidget *parent) : QPushButton(parent), m_angle(0), m_color(Qt::blue) {
    setCursor(Qt::PointingHandCursor);
    // Remove default button styling so we can paint freely
    setFlat(true);
    setStyleSheet("background: transparent; border: none;");
}

void ArrowButton::setAngle(int degrees) {
    m_angle = degrees;
    update();
}

void ArrowButton::setColor(const QColor &color) {
    m_color = color;
    update();
}

void ArrowButton::setArrowText(const QString &text) {
    m_text = text;
    update();
}

void ArrowButton::enterEvent(QEnterEvent *event) {
    emit hovered(true, m_text);
    QPushButton::enterEvent(event);
}

void ArrowButton::leaveEvent(QEvent *event) {
    emit hovered(false, "");
    QPushButton::leaveEvent(event);
}

void ArrowButton::paintEvent(QPaintEvent *) {
    // Debug Log (Throttled to first run per instance to avoid spam)
    if (m_angle == 0 && m_text.isEmpty()) {
        qDebug() << "ArrowButton painting with default/empty state (potential issue)";
    }

    static bool loggedOnce = false;
    if (!loggedOnce) {
        qDebug() << "ArrowButton::paintEvent executing (Angle:" << m_angle << "Text:" << m_text << ")";
        loggedOnce = true;
    }

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Calculate Arrow Geometry
    int w = width();
    int h = height();
    int cx = w / 2;
    int cy = h / 2;

    // Save state
    p.save();

    // Translate to center and rotate
    p.translate(cx, cy);
    p.rotate(m_angle);

    // Define Arrow Shape (Pointing Right at 0 degrees)
    // Adjust size based on widget size. Let's assume a standard arrow size.
    // Length: ~60% of width, Height: ~40% of height
    int arrowLen = qMin(w, h) * 0.8;
    int headLen = arrowLen * 0.4;
    int shaftThick = arrowLen * 0.3;

    QPainterPath path;
    // Tip
    path.moveTo(arrowLen / 2, 0);
    // Top Wing
    path.lineTo(arrowLen / 2 - headLen, -arrowLen / 2 * 0.5);
    // Shaft Top
    path.lineTo(arrowLen / 2 - headLen, -shaftThick / 2);
    // Shaft Tail Top
    path.lineTo(-arrowLen / 2, -shaftThick / 2);
    // Shaft Tail Bottom
    path.lineTo(-arrowLen / 2, shaftThick / 2);
    // Shaft Bottom
    path.lineTo(arrowLen / 2 - headLen, shaftThick / 2);
    // Bottom Wing
    path.lineTo(arrowLen / 2 - headLen, arrowLen / 2 * 0.5);
    // Close to Tip
    path.closeSubpath();

    // Use Red Pen (Stroke) and Transparent Brush
    QPen pen(Qt::red); // Default Red Border
    pen.setWidth(3);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    p.drawPath(path);

    p.restore();

    // Draw Text - At the "Tail"
    // To ensure text is upright, we calculate the tail position in unrotated coords

    // Convert polar to cartesian to find tail center
    // Tail is at -arrowLen/2 relative to center, rotated by angle
    double radians = qDegreesToRadians((double)m_angle);
    // Tail offset (backwards from direction)
    double tailDist = arrowLen / 2.0 + 10.0; // Extra padding
    double tx = cx - tailDist * qCos(radians);
    double ty = cy - tailDist * qSin(radians);

    // Define text alignment based on angle
    int flags = Qt::AlignCenter;
    // Refined logic for text placement relative to tail
    // If Angle is 0 (Right) -> Tail is Left -> Text should be Right Aligned (left of tail)? No, text at Left.
    // Ideally, center the text rect at (tx, ty) but apply alignment.

    // Let's create a bounding rect around (tx, ty)
    int txtW = 200;
    int txtH = 50;
    QRect txtRect(tx - txtW/2, ty - txtH/2, txtW, txtH);

    p.setPen(m_color); // Use user configured color (defaults to black/blue?)
    QFont f = font();
    f.setBold(true);
    f.setPointSize(14); // Make it visible
    p.setFont(f);

    p.drawText(txtRect, flags, m_text);
}
