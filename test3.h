#ifndef TEST3_H
#define TEST3_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QStackedWidget>
#include <QMap>
#include <QVector>
#include <QTimer>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>
#include <QPoint>

#include "utils.h"

// --- Constants for Configuration ---

// Task Sheet Text Coordinates (On "申领表.png")
const QPoint COORD_FLOOR = QPoint(200, 100);
const QPoint COORD_ITEM_1 = QPoint(100, 200); // Large Sheet
const QPoint COORD_ITEM_2 = QPoint(300, 200); // Large Duvet
const QPoint COORD_ITEM_3 = QPoint(100, 300); // Small Duvet
const QPoint COORD_ITEM_4 = QPoint(300, 300); // Pillow Towel
const QPoint COORD_ITEM_5 = QPoint(100, 400); // Good Night Towel
const QPoint COORD_ITEM_6 = QPoint(300, 400); // Towel

// Shelf Item Coordinates (On "取布草的货架.jpg" and "布草间-空.jpg")
// Used for both Taking (Warehouse) and Putting (Linen Room)
// Format: x, y, width, height (width/height usually fixed for touch area)
const QRect SHELF_RECT_1 = QRect(50, 150, 150, 150); // Large Sheet
const QRect SHELF_RECT_2 = QRect(250, 150, 150, 150); // Large Duvet
const QRect SHELF_RECT_3 = QRect(450, 150, 150, 150); // Small Duvet
const QRect SHELF_RECT_4 = QRect(50, 350, 150, 150);  // Pillow Towel
const QRect SHELF_RECT_5 = QRect(250, 350, 150, 150); // Good Night Towel
const QRect SHELF_RECT_6 = QRect(450, 350, 150, 150); // Towel

// --- Data Structures ---

struct Task {
    int targetFloor;
    QMap<QString, int> requiredItems; // Type -> Count
    bool isEmergency;
    bool isCompleted;
};

struct Inventory {
    QMap<QString, int> cleanItems; // Type -> Count
    int dirtyItemsCount = 0;
};

enum class GameScene {
    Entrance,
    StaffHallway,
    Office,
    Warehouse,       // Entry view
    WarehouseShelf,  // Shelf view (New)
    ElevatorHall,
    ElevatorInside,
    FloorCorridor,
    LinenRoom
};

struct GameState {
    GameScene currentScene;
    int currentFloor; // 0 for G, 6, 7
    QList<Task> tasks;
    Inventory inventory;
    bool hasClockedIn;
    bool hasReceivedTask;
    bool hasReported;
    // Track state of dirty bag per floor
    QMap<int, bool> dirtyBagState;
};

class Test3 : public QWidget {
    Q_OBJECT
public:
    explicit Test3(bool isDevMode, QWidget *parent = nullptr);
    void setDeveloperMode(bool enabled) { isDeveloperMode = enabled; }

signals:
    void levelCompleted();
    void logMessage(QString msg);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    bool isDeveloperMode;
    GameState gameState;

    // UI Elements
    QWidget *rpgCenterPanel;
    QLabel *locationLabel;
    QLabel *cartStatusLabel;
    QListWidget *taskListWidget;
    DraggableListWidget *inventoryListWidget;

    // Scene Rendering
    void goToScene(GameScene scene);
    void renderScene();
    void updateRPGStatusLabels();
    void refreshInventoryList();
    void refreshTaskList();

    // Scenes
    void renderEntrance();
    void renderStaffHallway();
    void renderOffice();
    void renderWarehouse();
    void renderWarehouseShelf(); // New
    void renderElevatorHall();
    void renderElevatorInside();
    void renderFloorCorridor();
    void renderLinenRoom();

    // Logic Handlers
    void handleClockIn();
    void handleClockOut();
    void handleGetTask();
    void handleReportWork();
    void handleGoHome();
    void handleElevatorButton(int floor);
    void showTaskSheet();
    void checkEmergencyTask();

    // Interaction Handlers
    void handleInventoryDrop(QString itemName); // From Scene to Cart (Take)
    void handleSceneDrop(QString itemName, bool isWarehouse); // From Cart to Scene (Put)

    void handleLoadCart();

    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
};

#endif // TEST3_H
