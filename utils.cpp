#include "utils.h"
#include <QDebug>
#include <QMessageBox>
#include <QPainterPath>
#include <QtMath> // Required for qDegreesToRadians, qCos, qSin

// --- 拖拽源标签 (DragSourceLabel) ---
DragSourceLabel::DragSourceLabel(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName) {
    // 透明样式作为叠加层
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // 改为手型光标
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

        drag->exec(Qt::CopyAction); // 使用 CopyAction 因为源 (货架) 是无限供应的
    }
}

// --- 放置目标标签 (DropLabel) ---
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

// --- 可拖拽列表控件 (DraggableListWidget) ---
DraggableListWidget::DraggableListWidget(QWidget *parent) : QListWidget(parent) {
    setDragEnabled(true);
    setAcceptDrops(true); // 现在也接受拖入 (来自仓库)
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::CopyAction); // 默认为复制
    setCursor(Qt::PointingHandCursor); // 设置光标
}

void DraggableListWidget::startDrag(Qt::DropActions supportedActions) {
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->data(Qt::UserRole).toString()); // 物品类型

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
        // 从外部源 (仓库) 拖入
        QString text = event->mimeData()->text();
        if (onItemDroppedIn) {
            onItemDroppedIn(text);
        }
        event->acceptProposedAction();
    } else {
        // 内部排序或无效
        QListWidget::dropEvent(event);
    }
}

// --- 可点击区域 (ClickableArea) ---
ClickableArea::ClickableArea(QWidget *parent) : QWidget(parent) {
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground);
}

void ClickableArea::setPolygon(const QPolygon &poly) {
    m_poly = poly;
    qDebug() << "ClickableArea::setPolygon points:" << poly.count(); // 调试日志
    // 应用遮罩以限制鼠标事件仅在多边形内
    setMask(m_poly);
}

void ClickableArea::mousePressEvent(QMouseEvent *event) {
    // 设置遮罩后，我们只接收多边形内的事件
    // 因此我们可以无条件接受并发出信号
    emit clicked();

    // 允许事件传播以便在父级过滤器中进行 Dev Mode 跟踪
    event->ignore();
}

void ClickableArea::enterEvent(QEnterEvent *event) {
    qDebug() << "ClickableArea::enterEvent:" << toolTip(); // 调试日志
    emit hovered(true, toolTip());
    QWidget::enterEvent(event);
}

void ClickableArea::leaveEvent(QEvent *event) {
    emit hovered(false, "");
    QWidget::leaveEvent(event);
}

void ClickableArea::paintEvent(QPaintEvent *) {
    // 默认不可见
}

// --- 货架区域 (ShelfArea) ---
ShelfArea::ShelfArea(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName), m_isDraggable(true) {
    setAcceptDrops(true); // 目标
    // 样式
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // 默认可拖拽
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
    // 仅当显式可拖拽时允许拖动
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
    // 防止自我放置 (从货架拖拽并放回货架) 触发逻辑
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

// --- 箭头按钮 (ArrowButton) ---
ArrowButton::ArrowButton(QWidget *parent)
    : QPushButton(parent), m_angle(0), m_color(Qt::blue), m_textSize(14)
{
    setCursor(Qt::PointingHandCursor);
    // 移除默认按钮样式以便自由绘制
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

void ArrowButton::setArrowTextSize(int size) {
    m_textSize = size;
    update();
}

bool ArrowButton::hitButton(const QPoint &pos) const {
    // 用户修改：点击区域为整个控件，而非仅箭头形状
    return QPushButton::hitButton(pos);
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
    // 调试日志 (每个实例仅限首次运行以避免刷屏)
    if (m_angle == 0 && m_text.isEmpty()) {
        qDebug() << "ArrowButton painting with default/empty state (potential issue)";
    }

    static bool loggedOnce = false;
    if (!loggedOnce) {
        qDebug() << "ArrowButton::paintEvent executing (Angle:" << m_angle << "Text:" << m_text << ")";
        loggedOnce = true;
    }

    QPainter p(this);
    if (!p.isActive()) return; // 安全检查

    p.setRenderHint(QPainter::Antialiasing);

    // 计算箭头几何形状
    int w = width();
    int h = height();
    if (w <= 0 || h <= 0) return; // 跳过无效几何

    int cx = w / 2;
    int cy = h / 2;

    // 保存状态
    p.save();

    // 平移到中心并旋转
    p.translate(cx, cy);
    p.rotate(m_angle);

    // 定义箭头形状 (0度时向右)
    // 根据控件尺寸调整大小
    // 如果控件很大 (200x80) 以容纳文本，箭头不应拉伸填满
    // 使用最小维度来定义箭头比例
    int arrowLen = qMin(w, h) * 0.8;
    // 如果 w=200, h=80 (矩形)，qMin 为 80，arrowLen 为 64
    // 这是一个合理的箭头图标大小

    int headLen = arrowLen * 0.4;
    int shaftThick = arrowLen * 0.3;

    QPainterPath path;
    // 尖端
    path.moveTo(arrowLen / 2, 0);
    // 上翼
    path.lineTo(arrowLen / 2 - headLen, -arrowLen / 2 * 0.5);
    // 轴上部
    path.lineTo(arrowLen / 2 - headLen, -shaftThick / 2);
    // 轴尾上部
    path.lineTo(-arrowLen / 2, -shaftThick / 2);
    // 轴尾下部
    path.lineTo(-arrowLen / 2, shaftThick / 2);
    // 轴下部
    path.lineTo(arrowLen / 2 - headLen, shaftThick / 2);
    // 下翼
    path.lineTo(arrowLen / 2 - headLen, arrowLen / 2 * 0.5);
    // 闭合
    path.closeSubpath();

    // 使用红笔 (描边) 和透明画刷
    QPen pen(Qt::red); // 默认红框
    pen.setWidth(3);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    p.drawPath(path);

    // 存储点击区域路径
    // 路径当前在旋转坐标系中 (中心为0,0)
    // 需要将其映射回控件坐标以用于 hitButton
    // 变换: Translate(cx, cy) * Rotate(angle)
    QTransform transform;
    transform.translate(cx, cy);
    transform.rotate(m_angle);
    m_hitPath = transform.map(path);

    p.restore();

    // 绘制文本 - 在“尾部”
    // 为确保文本直立，计算非旋转坐标中的尾部位置

    // 将极坐标转换为笛卡尔坐标以找到尾部中心
    // 尾部位于相对于中心的 -arrowLen/2 处，旋转 angle
    double radians = qDegreesToRadians((double)m_angle);
    // 尾部偏移 (反向)
    double tailDist = arrowLen / 2.0 + 10.0; // 额外间距
    double tx = cx - tailDist * qCos(radians);
    double ty = cy - tailDist * qSin(radians);

    // 定义对齐方式
    int flags = Qt::AlignCenter;

    // 创建围绕 (tx, ty) 的边界矩形
    int txtW = 200;
    int txtH = 50;
    QRect txtRect(tx - txtW/2, ty - txtH/2, txtW, txtH);

    p.setPen(m_color); // 使用用户配置的颜色
    QFont f = font();
    f.setBold(true);
    f.setPointSize(m_textSize); // 使用配置尺寸
    p.setFont(f);

    p.drawText(txtRect, flags, m_text);
}
