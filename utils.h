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

// --- Helper Functions ---

/**
 * @brief Sets the geometry of a widget based on its center point.
 * @param widget The widget to position.
 * @param rect A QRect where x, y are the CENTER coordinates, and width, height are dimensions.
 */
inline void setGeometryCentered(QWidget *widget, const QRect &rect) {
    if (!widget) return;
    int w = rect.width();
    int h = rect.height();
    int x = rect.x() - (w / 2);
    int y = rect.y() - (h / 2);
    widget->setGeometry(x, y, w, h);
}

/**
 * @brief Sets the geometry of a widget based on its center point.
 * @param widget The widget to position.
 * @param centerX Center X coordinate.
 * @param centerY Center Y coordinate.
 * @param w Width.
 * @param h Height.
 */
inline void setGeometryCentered(QWidget *widget, int centerX, int centerY, int w, int h) {
    if (!widget) return;
    widget->setGeometry(centerX - (w / 2), centerY - (h / 2), w, h);
}


// A label that initiates a drag operation (Source)
// Used in Warehouse to drag items FROM the shelf TO the cart
class DragSourceLabel : public QLabel {
    Q_OBJECT
public:
    DragSourceLabel(const QString &itemName, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_itemName;
};

// A label that accepts drops (Target)
// Used in Linen Room to drag items FROM the cart TO the shelf
class DropLabel : public QLabel {
    Q_OBJECT
public:
    DropLabel(const QString &text, QWidget *parent = nullptr);

    std::function<void(QString)> onDropCallback;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

// A ListWidget that allows dragging items OUT (Source) and dropping items IN (Target)
// Used for the Cart Inventory
class DraggableListWidget : public QListWidget {
    Q_OBJECT
public:
    DraggableListWidget(QWidget *parent = nullptr);

    // Signal when something is dropped INTO the list
    std::function<void(QString)> onItemDroppedIn;

protected:
    // Handle Dragging OUT
    void startDrag(Qt::DropActions supportedActions) override;

    // Handle Dropping IN (From Warehouse)
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

/**
 * @brief Transparent Clickable Area defined by a Polygon
 * Used for irregular buttons like "Enter Hotel".
 */
class ClickableArea : public QWidget {
    Q_OBJECT
public:
    ClickableArea(QWidget *parent = nullptr);

    // Set the hit area polygon (in local coordinates if possible, or assume widget covers area)
    // To make it simple: The widget will be large (covering the bounding rect),
    // and this polygon defines the click region relative to the widget's TopLeft.
    void setPolygon(const QPolygon &poly);

signals:
    void clicked();
    void hovered(bool status, QString text);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override; // For debug drawing if needed

private:
    QPolygon m_poly;
};

/**
 * @brief Helper: Combined Widget for Warehouse Shelf (Source + Target)
 * Allows Dragging FROM it (Source) AND Dropping ONTO it (Target)
 */
class ShelfArea : public QLabel {
    Q_OBJECT
public:
    ShelfArea(const QString &itemName, QWidget *parent = nullptr);

    std::function<void(QString)> onDropCallback; // When something is put back here

    // Set whether the item can be dragged (taken) from this shelf
    void setDraggable(bool enabled);

    // Set explicit source type for drag
    void setSourceType(const QString &type) { m_sourceType = type; }

signals:
    void hovered(bool status, QString text);

protected:
    // Drag Source Logic (Taking item)
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    // Drop Target Logic (Putting item back)
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString m_itemName;
    QString m_sourceType;
    bool m_isDraggable;
};

/**
 * @brief Custom Arrow Button
 * Draws an arrow shape pointing in a direction + Text at the tail.
 */
class ArrowButton : public QPushButton {
    Q_OBJECT
public:
    ArrowButton(QWidget *parent = nullptr);

    // Set properties
    void setAngle(int degrees); // 0=Right, 90=Down, 180=Left, 270=Up
    void setColor(const QColor &color);
    void setArrowText(const QString &text);
    void setArrowTextSize(int size);

    // Override hitButton to restrict clicks to the arrow shape
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

    // Stores the clickable arrow shape from the last paint event
    QPainterPath m_hitPath;
};

#endif // UTILS_H
