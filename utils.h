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
#include <QDialog>
#include <QMessageBox>

// --- Namespace for Utility Functions ---
namespace Utils {
    /**
     * @brief 显示自定义的消息提示框 (Custom Information/Warning Dialog)
     * 替代 QMessageBox::information/warning，提供更大的按钮和更好的可配置性
     * @param parent 父窗口
     * @param title 标题
     * @param content 内容
     * @param isWarning 是否为警告样式 (默认为 false, 即 Information)
     */
    void ShowCustomMessageBox(QWidget *parent, const QString &title, const QString &content, bool isWarning = false);

    /**
     * @brief 显示自定义的确认对话框 (Custom Yes/No Dialog)
     * 替代 QMessageBox::question，提供更大的按钮
     * @param parent 父窗口
     * @param title 标题
     * @param content 内容
     * @return true if Yes, false if No
     */
    bool ShowCustomConfirmDialog(QWidget *parent, const QString &title, const QString &content);
}

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
