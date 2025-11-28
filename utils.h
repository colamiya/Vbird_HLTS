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
#include <QDebug>
#include <QMessageBox>
#include <functional>

// A label that initiates a drag operation (Source)
// Used in Warehouse to drag items FROM the shelf TO the cart
class DragSourceLabel : public QLabel {
    Q_OBJECT
public:
    DragSourceLabel(const QString &itemName, QWidget *parent = nullptr)
        : QLabel(parent), m_itemName(itemName) {
        // Transparent style for overlay
        setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
        setCursor(Qt::OpenHandCursor);
        setToolTip("拖拽 " + itemName);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
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

private:
    QString m_itemName;
};

// A label that accepts drops (Target)
// Used in Linen Room to drag items FROM the cart TO the shelf
class DropLabel : public QLabel {
    Q_OBJECT
public:
    DropLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent) {
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

    std::function<void(QString)> onDropCallback;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasText()) {
            QString text = event->mimeData()->text();
            if (onDropCallback) {
                onDropCallback(text);
            }
            event->acceptProposedAction();
        }
    }
};

// A ListWidget that allows dragging items OUT (Source) and dropping items IN (Target)
// Used for the Cart Inventory
class DraggableListWidget : public QListWidget {
    Q_OBJECT
public:
    DraggableListWidget(QWidget *parent = nullptr) : QListWidget(parent) {
        setDragEnabled(true);
        setAcceptDrops(true); // Now also accepts drops (from Warehouse)
        setDropIndicatorShown(true);
        setDragDropMode(QAbstractItemView::DragDrop);
        setDefaultDropAction(Qt::CopyAction); // Default to copy
    }

    // Signal when something is dropped INTO the list
    std::function<void(QString)> onItemDroppedIn;

protected:
    // Handle Dragging OUT
    void startDrag(Qt::DropActions supportedActions) override {
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

    // Handle Dropping IN (From Warehouse)
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        } else {
            QListWidget::dragEnterEvent(event);
        }
    }

    void dragMoveEvent(QDragMoveEvent *event) override {
        if (event->mimeData()->hasText()) {
             event->acceptProposedAction();
        } else {
             QListWidget::dragMoveEvent(event);
        }
    }

    void dropEvent(QDropEvent *event) override {
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
};

// Helper to make transparent clickable areas for task sheet dev mode
class ClickableArea : public QWidget {
    Q_OBJECT
public:
    ClickableArea(QWidget *parent = nullptr) : QWidget(parent) {}
protected:
    void mousePressEvent(QMouseEvent *event) override {
        QPoint pos = event->pos();
        QString coordText = QString("TaskSheet Click: (%1, %2)").arg(pos.x()).arg(pos.y());
        qDebug() << coordText;
        QMessageBox::information(this, "坐标", coordText);
    }
};

// Helper: Combined Widget for Warehouse Shelf (Source + Target)
// Allows Dragging FROM it (Source) AND Dropping ONTO it (Target)
class ShelfArea : public QLabel {
    Q_OBJECT
public:
    ShelfArea(const QString &itemName, QWidget *parent = nullptr)
        : QLabel(parent), m_itemName(itemName) {
        setAcceptDrops(true); // Target
        // Style
        setStyleSheet("background-color: rgba(255, 255, 255, 0.1); border: 1px dashed rgba(255,255,255,0.5);");
        setCursor(Qt::OpenHandCursor);
        setToolTip(itemName);
    }

    std::function<void(QString)> onDropCallback; // When something is put back here

protected:
    // Drag Source Logic (Taking item)
    void mousePressEvent(QMouseEvent *event) override {
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

            drag->exec(Qt::CopyAction);
        }
    }

    // Drop Target Logic (Putting item back)
    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        if (event->mimeData()->hasText()) {
            QString text = event->mimeData()->text();
            if (text == m_itemName) {
                if (onDropCallback) onDropCallback(text);
                event->acceptProposedAction();
            }
        }
    }

private:
    QString m_itemName;
};

#endif // UTILS_H
