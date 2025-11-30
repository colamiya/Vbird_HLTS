#ifndef TEST3_H
#define TEST3_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTreeWidget> // Changed from ListWidget
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

// --- 数据结构 (Data Structures) ---

// 任务结构体
struct Task {
    int targetFloor;                  // 目标楼层
    QMap<QString, int> requiredItems; // 需求物品 -> 数量 (固定值)
    bool isEmergency;                 // 是否紧急
    bool isCompleted;                 // 是否已完成
};

// 库存结构体 (用于推车)
struct Inventory {
    QMap<QString, int> cleanItems; // 物品类型 -> 数量
    int dirtyItemsCount = 0;       // 脏布草数量
};

// 游戏场景枚举
enum class GameScene {
    Entrance,       // 入口
    StaffHallway,   // 员工通道
    Office,         // 办公室
    Warehouse,      // 仓库入口
    WarehouseShelf, // 仓库货架
    ElevatorHall,   // 电梯厅
    ElevatorInside, // 电梯内部
    FloorCorridor,  // 楼层走廊
    LinenRoom       // 布草间
};

// 游戏状态结构体
struct GameState {
    GameScene currentScene;
    int currentFloor; // 0 (G), 2-10
    QList<Task> tasks;
    Inventory inventory; // 推车库存

    // 楼层库存 (Floor Inventory) - 独立于任务，记录放置在各楼层的物品
    // Map<Floor, Map<ItemName, Count>>
    QMap<int, QMap<QString, int>> floorInventory;

    bool hasClockedIn;    // 已签到
    bool hasReceivedTask; // 已领任务
    bool hasReported;     // 已汇报

    // 脏布草袋状态 (楼层 -> 是否有脏布草)
    QMap<int, bool> dirtyBagState;
};

// 错误日志 (用于最终汇报)
struct ErrorLog {
    bool lateClockIn = false;           // 迟到
    bool missedEmergencyPriority = false; // 未优先处理紧急任务
    bool noReportBeforeHome = false;    // 下班前未汇报
    bool noClockOutBeforeHome = false;  // 下班前未打卡
    bool mixedLinen = false;            // 混装布草 (新)

    // 检查是否有错误
    bool hasErrors() const {
        return lateClockIn || missedEmergencyPriority || noReportBeforeHome || noClockOutBeforeHome || mixedLinen;
    }
};

class Test3 : public QWidget {
    Q_OBJECT
public:
    explicit Test3(bool isDevMode, QWidget *parent = nullptr);
    void setDeveloperMode(bool enabled) { isDeveloperMode = enabled; }
    void setEmergencyMode(bool enabled) { isEmergencyEnabled = enabled; }

    // 重置游戏状态
    void reset();

signals:
    void levelCompleted();
    void levelCancelled();
    void logMessage(QString msg);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    bool isDeveloperMode;
    bool isEmergencyEnabled = false;
    GameState gameState;
    ErrorLog errorLog;

    // 逻辑计时器与标志
    QTimer *latenessTimer;
    QTimer *heartbeatTimer;
    bool isLate = false;
    bool isTimerTriggered = false;
    bool isEmergencyActive = false;

    // UI 元素
    QWidget *rpgCenterPanel;
    QLabel *locationLabel;
    QLabel *hoverHintLabel;
    QLabel *cartStatusLabel;
    QTreeWidget *taskListWidget; // 改为 TreeWidget 以支持展开

    // 右侧侧边栏元素
    QLabel *inventoryTitleLabel;
    DraggableListWidget *inventoryListWidget;
    QWidget *elevatorPanelContainer;

    // 场景渲染
    void goToScene(GameScene scene);
    void renderScene();
    void updateRPGStatusLabels();
    void refreshInventoryList();
    void refreshTaskList();

    // 开发者模式过滤器
    void installDevFilter(QWidget *widget);

    // 各场景渲染函数
    void renderEntrance();
    void renderStaffHallway();
    void renderOffice();
    void renderWarehouse();
    void renderWarehouseShelf();
    void renderElevatorHall();
    void renderElevatorInside();
    void renderFloorCorridor();
    void renderLinenRoom();

    // 逻辑处理函数
    void handleClockIn();
    void handleClockOut();
    void handleGetTask();
    void handleReportWork();
    void handleGoHome();
    void handleElevatorButton(int floor);
    void showTaskSheet(int taskIndex = -1);
    void checkEmergencyTask();

    // 辅助函数
    int getNormalRandom(int min, int max);

    // 交互处理
    void handleInventoryDrop(QString itemName, const QMimeData *mimeData = nullptr); // 拿取 (货架->车)
    void handleSceneDrop(QString itemName, bool isWarehouse); // 放置 (车->货架)

    // 新手教程
    void showTutorial();

    // 生成占位图
    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
};

#endif // TEST3_H
