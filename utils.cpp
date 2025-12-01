#include "utils.h"
#include <QDebug>
#include <QMessageBox>
#include <QPainterPath>
#include <QtMath> // 需要 qDegreesToRadians, qCos, qSin

// --- 拖拽源标签 (DragSourceLabel) ---
DragSourceLabel::DragSourceLabel(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName)
{
    // 设置半透明背景样式，作为视觉叠加层
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // 设置为手型光标，提示可交互
    setToolTip("拖拽 " + itemName);
}

void DragSourceLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 创建拖拽对象
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        drag->setMimeData(mimeData);

        // 创建拖拽时的视觉反馈 (Pixmap)
        QPixmap pixmap(100, 30);
        pixmap.fill(Qt::lightGray);
        QPainter painter(&pixmap);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, m_itemName);
        drag->setPixmap(pixmap);

        // 执行拖拽操作，使用 CopyAction 表示源物品无限供应
        drag->exec(Qt::CopyAction);
    }
}

// --- 放置目标标签 (DropLabel) ---
DropLabel::DropLabel(const QString &text, QWidget *parent) : QLabel(text, parent)
{
    setAcceptDrops(true); // 允许接受放置事件
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
    // 仅接受包含文本数据的拖拽
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
        // 如果设置了回调函数，则执行回调
        if (onDropCallback)
        {
            onDropCallback(text);
        }
        event->acceptProposedAction();
    }
}

// --- 可拖拽列表控件 (DraggableListWidget) ---
DraggableListWidget::DraggableListWidget(QWidget *parent) : QListWidget(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true); // 允许拖入 (例如从仓库货架放回推车)
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setDefaultDropAction(Qt::CopyAction); // 默认操作为复制 (逻辑上由业务层控制数量扣减)
    setCursor(Qt::PointingHandCursor);    // 设置光标
}

void DraggableListWidget::startDrag(Qt::DropActions supportedActions)
{
    QListWidgetItem *item = currentItem();
    if (item)
    {
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(item->data(Qt::UserRole).toString()); // 传递物品类型标识

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);

        // 使用当前项的图标作为拖拽视觉反馈，如果没有则生成文字图
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
    // 如果拖拽源不是自己 (即从外部拖入)，且包含文本数据
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
        // 内部排序或无效操作交由父类处理
        QListWidget::dropEvent(event);
    }
}

// --- 可点击区域 (ClickableArea) ---
ClickableArea::ClickableArea(QWidget *parent) : QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground); // 背景透明
}

void ClickableArea::setPolygon(const QPolygon &poly)
{
    m_poly = poly;
    qDebug() << "ClickableArea::setPolygon 点数:" << poly.count(); // 调试日志
    // 设置遮罩，限制鼠标事件仅在多边形区域内生效
    setMask(m_poly);
}

void ClickableArea::mousePressEvent(QMouseEvent *event)
{
    // 由于设置了 setMask，只有多边形内的点击会触发此事件
    emit clicked();

    // 忽略事件，使其可以传播到父级 (用于开发者模式下的坐标跟踪)
    event->ignore();
}

void ClickableArea::enterEvent(QEnterEvent *event)
{
    // 鼠标进入区域，发出悬停信号
    qDebug() << "ClickableArea::enterEvent:" << toolTip(); // 调试日志
    emit hovered(true, toolTip());
    QWidget::enterEvent(event);
}

void ClickableArea::leaveEvent(QEvent *event)
{
    // 鼠标离开区域，取消悬停信号
    emit hovered(false, "");
    QWidget::leaveEvent(event);
}

void ClickableArea::paintEvent(QPaintEvent *)
{
    // 默认不可见，仅用于交互
}

// --- 货架区域 (ShelfArea) ---
ShelfArea::ShelfArea(const QString &itemName, QWidget *parent)
    : QLabel(parent), m_itemName(itemName), m_isDraggable(true)
{
    setAcceptDrops(true); // 允许作为放置目标
    // 样式: 半透明虚线框
    setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
    setCursor(Qt::PointingHandCursor); // 默认可交互
    setToolTip(itemName);
}

void ShelfArea::setDraggable(bool enabled)
{
    m_isDraggable = enabled;
    if (m_isDraggable)
    {
        setCursor(Qt::PointingHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void ShelfArea::mousePressEvent(QMouseEvent *event)
{
    // 如果被禁用拖拽 (例如无库存)，则直接返回
    if (!m_isDraggable)
        return;

    if (event->button() == Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(m_itemName);
        // 如果设置了源类型 (如 "WarehouseShelf")，则附加此数据
        if (!m_sourceType.isEmpty())
        {
            mimeData->setData("application/x-source", m_sourceType.toUtf8());
        }
        drag->setMimeData(mimeData);

        // 生成拖拽预览图
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
    // 防止自我放置 (从货架拖起又放回同一个货架)
    if (event->source() == this)
    {
        event->ignore();
        return;
    }

    if (event->mimeData()->hasText())
    {
        QString text = event->mimeData()->text();
        // 只有当拖入的物品名称与货架名称一致时才接受 (例如只能将"床单"放回"床单"货架)
        // 注意: 实际上 onDropCallback 内部可能还会做进一步检查
        if (text == m_itemName)
        {
            if (onDropCallback)
                onDropCallback(text);
            event->acceptProposedAction();
        }
    }
}

// --- 箭头按钮 (ArrowButton) ---
ArrowButton::ArrowButton(QWidget *parent)
    : QPushButton(parent), m_angle(0), m_color(Qt::blue), m_textSize(14)
{
    setCursor(Qt::PointingHandCursor);
    // 移除默认按钮样式以便完全自定义绘制
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
    // 点击判定区域改为整个控件矩形，而不是仅限于绘制的箭头形状，以提高易用性
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
    // 调试日志 (每个实例仅限首次运行以避免刷屏影响性能)
    if (m_angle == 0 && m_text.isEmpty())
    {
        qDebug() << "ArrowButton: 正在绘制默认/空状态 (可能是潜在配置问题)";
    }

    static bool loggedOnce = false;
    if (!loggedOnce)
    {
        qDebug() << "ArrowButton::paintEvent 执行中 (角度:" << m_angle << "文本:" << m_text << ")";
        loggedOnce = true;
    }

    QPainter p(this);
    // 安全检查: 确保 Painter 处于活跃状态，防止崩溃
    if (!p.isActive())
        return;

    p.setRenderHint(QPainter::Antialiasing);

    // 获取控件尺寸
    int w = width();
    int h = height();
    if (w <= 0 || h <= 0)
        return; // 跳过无效几何尺寸

    int cx = w / 2;
    int cy = h / 2;

    // 保存 Painter 状态
    p.save();

    // 平移到中心并旋转
    p.translate(cx, cy);
    p.rotate(m_angle);

    // 定义箭头形状 (0度时指向右侧)
    // 根据控件尺寸动态调整大小
    // 使用最小边长来确定箭头比例，防止在长方形控件中变形
    int arrowLen = qMin(w, h) * 0.8;
    // 示例: 如果 w=200, h=80，则 arrowLen 为 64

    int headLen = arrowLen * 0.4;    // 箭头头部长度
    int shaftThick = arrowLen * 0.3; // 箭杆厚度

    QPainterPath path;
    // 绘制箭头路径
    path.moveTo(arrowLen / 2, 0);                             // 尖端
    path.lineTo(arrowLen / 2 - headLen, -arrowLen / 2 * 0.5); // 上翼点
    path.lineTo(arrowLen / 2 - headLen, -shaftThick / 2);     // 轴上部连接点
    path.lineTo(-arrowLen / 2, -shaftThick / 2);              // 轴尾上部
    path.lineTo(-arrowLen / 2, shaftThick / 2);               // 轴尾下部
    path.lineTo(arrowLen / 2 - headLen, shaftThick / 2);      // 轴下部连接点
    path.lineTo(arrowLen / 2 - headLen, arrowLen / 2 * 0.5);  // 下翼点
    path.closeSubpath();                                      // 闭合路径

    // 设置画笔 (红色描边) 和 画刷 (透明填充)
    QPen pen(Qt::red);
    pen.setWidth(3);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    p.drawPath(path);

    // 存储点击区域路径 (当前未使用，因为 hitButton 已改为全控件)
    // 变换: Translate(cx, cy) * Rotate(angle)
    QTransform transform;
    transform.translate(cx, cy);
    transform.rotate(m_angle);
    m_hitPath = transform.map(path);

    p.restore(); // 恢复 Painter 状态，用于绘制文本 (不随箭头旋转)

    // 绘制文本 - 定位在箭头“尾部”的反方向
    // 计算尾部中心点 (相对于控件中心)
    double radians = qDegreesToRadians((double)m_angle);
    double tailDist = arrowLen / 2.0 + 10.0; // 额外间距
    double tx = cx - tailDist * qCos(radians);
    double ty = cy - tailDist * qSin(radians);

    // 定义文本框大小
    int txtW = 200;
    int txtH = 50;
    QRect txtRect(tx - txtW / 2, ty - txtH / 2, txtW, txtH);

    p.setPen(m_color); // 使用用户配置的颜色
    QFont f = font();
    f.setBold(true);
    f.setPointSize(m_textSize); // 使用配置尺寸
    p.setFont(f);

    p.drawText(txtRect, Qt::AlignCenter, m_text);
}
