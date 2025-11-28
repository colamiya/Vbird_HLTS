#ifndef UTILS_H
#define UTILS_H

#include <QListWidget>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
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

// Helper to make transparent clickable areas for task sheet dev mode
class ClickableArea : public QWidget {
    Q_OBJECT
public:
    ClickableArea(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) override;
};

// Helper: Combined Widget for Warehouse Shelf (Source + Target)
// Allows Dragging FROM it (Source) AND Dropping ONTO it (Target)
class ShelfArea : public QLabel {
    Q_OBJECT
public:
    ShelfArea(const QString &itemName, QWidget *parent = nullptr);

    std::function<void(QString)> onDropCallback; // When something is put back here

protected:
    // Drag Source Logic (Taking item)
    void mousePressEvent(QMouseEvent *event) override;

    // Drop Target Logic (Putting item back)
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString m_itemName;
};

#endif // UTILS_H
