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
#include <QBoxLayout>
#include <QVariant>
#include <functional>

// --- 辅助函数 (Helper Functions) ---

inline void setGeometryCentered(QWidget *widget, const QRect &rect)
{
    if (!widget)
        return;
    // 存储原始几何数据以支持自适应缩放
    widget->setProperty("originalGeometry", rect);

    int w = rect.width();
    int h = rect.height();
    int x = rect.x() - (w / 2);
    int y = rect.y() - (h / 2);
    widget->setGeometry(x, y, w, h);
}

inline void setGeometryCentered(QWidget *widget, int centerX, int centerY, int w, int h)
{
    if (!widget)
        return;
    // 构造 Rect 存储
    QRect rect(centerX, centerY, w, h); // 注意：这里存储的是 CenterX/Y 和 W/H，为了统一，我们在 rescale 逻辑中需要特殊处理，或者统一存储 TopLeft
    // 为了简化，我们统一存储 "Center Rect" (Center X, Center Y, W, H) 还是 "Geometry Rect" (Left, Top, W, H)?
    // 上面的重载传入的是 rect，逻辑是 center based (x - w/2).
    // 这里的参数 explicitly centerX.
    // 让我们统一存储 "Reference Rect" 为 Center-Based 的原始定义可能比较麻烦，因为 Qt 的 QRect 是 Left-Top.
    // 现有的 config 全是 Center Based 吗？
    // config_test3.h: "RECT_..." 通常是中心点? 让我们再次检查。
    // Config: "Test 3 button geometries ... apply to widget centers."
    // 所以 config 中的 QRect 其实是 (CenterX, CenterY, Width, Height).
    // 而 setGeometry 需要 (Left, Top, Width, Height).

    // 所以，我们存储原始的 Config Rect (Center-based) 到 property 中。
    widget->setProperty("originalGeometry", QRect(centerX, centerY, w, h));

    widget->setGeometry(centerX - (w / 2), centerY - (h / 2), w, h);
}

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

class DraggableListWidget : public QListWidget
{
    Q_OBJECT
public:
    DraggableListWidget(QWidget *parent = nullptr);
    std::function<void(QString, const QMimeData*)> onItemDroppedIn;

protected:
    void startDrag(Qt::DropActions supportedActions) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

/**
 * @brief 由多边形定义的透明可交互区域 (Interactive Polygon Area)
 * 支持点击和拖放接收 (Drop)
 */
class ClickableArea : public QWidget
{
    Q_OBJECT
public:
    ClickableArea(QWidget *parent = nullptr);

    void setPolygon(const QPolygon &poly);
    // 缩放多边形
    void rescale(float scaleX, float scaleY);

    // 拖放回调
    std::function<void(QString)> onDropCallback;

signals:
    void clicked();
    void hovered(bool status, QString text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    // 拖放支持
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QPolygon m_poly;
    QPolygon m_originalPoly; // 原始多边形
};

class ShelfArea : public QLabel
{
    Q_OBJECT
public:
    ShelfArea(const QString &itemName, QWidget *parent = nullptr);
    std::function<void(QString)> onDropCallback;
    void setDraggable(bool enabled);
    void setSourceType(const QString &type) { m_sourceType = type; }

signals:
    void hovered(bool status, QString text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString m_itemName;
    QString m_sourceType;
    bool m_isDraggable;
};

class ArrowButton : public QPushButton
{
    Q_OBJECT
public:
    ArrowButton(QWidget *parent = nullptr);
    void setAngle(int degrees);
    void setColor(const QColor &color);
    void setArrowText(const QString &text);
    void setArrowTextSize(int size);
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
    QPainterPath m_hitPath;
};

/**
 * @brief 气泡提示框 (Modal Speech Bubble)
 * 阻挡操作直到关闭
 */
class SpeechBubble : public QWidget
{
    Q_OBJECT
public:
    explicit SpeechBubble(const QString &text, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
};

#endif // UTILS_H
