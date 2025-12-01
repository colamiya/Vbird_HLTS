#ifndef UTILS_H
#define UTILS_H

#include <QListWidget>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPolygon>
#include <QPushButton>
#include <functional>

// --- 辅助函数 (Helper Functions) ---

/**
 * @brief 根据中心点设置控件几何形状
 * @param widget 要定位的控件
 * @param rect QRect，其中 x, y 为中心坐标，width, height 为尺寸
 */
inline void setGeometryCentered(QWidget *widget, const QRect &rect)
{
    if (!widget)
        return;
    int w = rect.width();
    int h = rect.height();
    int x = rect.x() - (w / 2);
    int y = rect.y() - (h / 2);
    widget->setGeometry(x, y, w, h);
}

/**
 * @brief 根据中心点设置控件几何形状
 * @param widget 要定位的控件
 * @param centerX 中心 X 坐标
 * @param centerY 中心 Y 坐标
 * @param w 宽度
 * @param h 高度
 */
inline void setGeometryCentered(QWidget *widget, int centerX, int centerY, int w, int h)
{
    if (!widget)
        return;
    widget->setGeometry(centerX - (w / 2), centerY - (h / 2), w, h);
}

// 拖拽源标签 (Drag Source Label)
// 用于仓库中从货架拖拽物品到推车
class DragSourceLabel : public QLabel
{
    Q_OBJECT
public:
    DragSourceLabel(const QString &itemName, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_itemName;
};

// 放置目标标签 (Drop Label)
// 用于布草间从推车拖拽物品到货架
class DropLabel : public QLabel
{
    Q_OBJECT
public:
    DropLabel(const QString &text, QWidget *parent = nullptr);

    std::function<void(QString)> onDropCallback;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

// 可拖拽列表控件 (Draggable List Widget)
// 用于推车库存，支持拖出 (Source) 和拖入 (Target)
class DraggableListWidget : public QListWidget
{
    Q_OBJECT
public:
    DraggableListWidget(QWidget *parent = nullptr);

    // 当物品被拖入时触发的回调
    std::function<void(QString)> onItemDroppedIn;

protected:
    // 处理拖拽开始 (拖出)
    void startDrag(Qt::DropActions supportedActions) override;

    // 处理拖入 (来自仓库)
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

/**
 * @brief 由多边形定义的透明可点击区域
 * 用于不规则按钮，如“进入酒店”
 */
class ClickableArea : public QWidget
{
    Q_OBJECT
public:
    ClickableArea(QWidget *parent = nullptr);

    // 设置点击区域多边形 (建议使用局部坐标，或者控件覆盖整个区域)
    // 简单做法: 控件覆盖整个边界矩形，此多边形定义相对于控件左上角的点击区域
    void setPolygon(const QPolygon &poly);

signals:
    void clicked();
    void hovered(bool status, QString text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override; // 用于调试绘制

private:
    QPolygon m_poly;
};

/**
 * @brief 辅助类: 仓库货架组合控件 (Source + Target)
 * 允许从中拖拽 (Source) 并且允许放置到上面 (Target)
 */
class ShelfArea : public QLabel
{
    Q_OBJECT
public:
    ShelfArea(const QString &itemName, QWidget *parent = nullptr);

    std::function<void(QString)> onDropCallback; // 当物品被放回此处时回调

    // 设置是否可以从该货架拖拽 (拿取)
    void setDraggable(bool enabled);

    // 设置显式拖拽源类型
    void setSourceType(const QString &type) { m_sourceType = type; }

signals:
    void hovered(bool status, QString text);

protected:
    // 拖拽源逻辑 (拿取物品)
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    // 放置目标逻辑 (放回物品)
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString m_itemName;
    QString m_sourceType;
    bool m_isDraggable;
};

/**
 * @brief 自定义箭头按钮
 * 绘制指向特定方向的箭头 + 尾部文本
 */
class ArrowButton : public QPushButton
{
    Q_OBJECT
public:
    ArrowButton(QWidget *parent = nullptr);

    // 设置属性
    void setAngle(int degrees); // 0=右, 90=下, 180=左, 270=上
    void setColor(const QColor &color);
    void setArrowText(const QString &text);
    void setArrowTextSize(int size);

    // 重写 hitButton 以限制点击区域仅为箭头形状
    bool hitButton(const QPoint &pos) const override;

signals:
    void hovered(bool status, QString text);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    int m_angle;
    QColor m_color;
    QString m_text;
    int m_textSize;

    // 存储上一次绘制的点击区域路径
    QPainterPath m_hitPath;
};

#endif // UTILS_H
