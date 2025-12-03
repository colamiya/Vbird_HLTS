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
#include <functional>

// --- 辅助函数 (Helper Functions) ---

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

inline void setGeometryCentered(QWidget *widget, int centerX, int centerY, int w, int h)
{
    if (!widget)
        return;
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
