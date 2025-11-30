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
#include "config.h"

// --- Data Structures ---

struct Task {
    int targetFloor;
    QMap<QString, int> requiredItems; // Type -> Count: Remaining needed
    QMap<QString, int> placedItems;   // Type -> Count: Already placed
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

// Error Tracking for Final Report
struct ErrorLog {
    bool lateClockIn = false;
    bool missedEmergencyPriority = false;
    bool noReportBeforeHome = false;
    bool noClockOutBeforeHome = false;
    // Helper to check if any error
    bool hasErrors() const {
        return lateClockIn || missedEmergencyPriority || noReportBeforeHome || noClockOutBeforeHome;
    }
};

class Test3 : public QWidget {
    Q_OBJECT
public:
    explicit Test3(bool isDevMode, QWidget *parent = nullptr);
    void setDeveloperMode(bool enabled) { isDeveloperMode = enabled; }
    void setEmergencyMode(bool enabled) { isEmergencyEnabled = enabled; }

    // Reset game state to initial values
    void reset();

signals:
    void levelCompleted();
    void levelCancelled();
    void logMessage(QString msg);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override; // Log when widget is shown

private:
    bool isDeveloperMode;
    bool isEmergencyEnabled = false;
    GameState gameState;
    ErrorLog errorLog;

    // Logic Timers & Flags
    QTimer *latenessTimer;
    bool isLate = false;
    bool isTimerTriggered = false; // Has the 10s timer been triggered?
    bool isEmergencyActive = false; // Is there an active emergency task?

    // UI Elements
    QWidget *rpgCenterPanel;
    QLabel *locationLabel;
    QLabel *hoverHintLabel; // New Label for Hover Hints
    QLabel *cartStatusLabel;
    QListWidget *taskListWidget;

    // Right Sidebar Elements
    QLabel *inventoryTitleLabel;
    DraggableListWidget *inventoryListWidget;
    QWidget *elevatorPanelContainer; // Container for elevator buttons in sidebar

    // Scene Rendering
    void goToScene(GameScene scene);
    void renderScene();
    void updateRPGStatusLabels();
    void refreshInventoryList();
    void refreshTaskList();

    // Helper to install event filter on all children recursively for dev mode
    void installDevFilter(QWidget *widget);

    // Scenes
    void renderEntrance();
    void renderStaffHallway();
    void renderOffice();
    void renderWarehouse();
    void renderWarehouseShelf();
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
    void showTaskSheet(int taskIndex = -1); // -1 means currently selected or last
    void checkEmergencyTask(); // Deprecated but kept for compatibility

    // Normal Dist helper
    int getNormalRandom(int min, int max);

    // Interaction Handlers
    void handleInventoryDrop(QString itemName, const QMimeData *mimeData = nullptr); // From Scene to Cart (Take)
    void handleSceneDrop(QString itemName, bool isWarehouse); // From Cart to Scene (Put)

    void handleLoadCart();

    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
};

#endif // TEST3_H
