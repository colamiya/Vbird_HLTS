#ifndef TEST3_H
#define TEST3_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QStackedWidget>
#include <QMap>
#include <QVector>
#include <QTimer>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QDebug>
#include <QPoint>
#include <QDateTime>

#include "utils.h"
#include "config.h"

// --- 数据结构 (Data Structures) ---

// 任务结构体
struct Task
{
    int targetFloor;                  // 目标楼层
    QMap<QString, int> requiredItems; // 需求物品 -> 数量 (固定值)
    bool isEmergency;                 // 是否为紧急任务
    bool isCompleted;                 // 是否已完成 (系统判定)
    bool isMarkedComplete = false;    // 学生手动标记完成 (仅视觉)

    // 物品级标记状态 (Item Name -> Is Checked)
    QMap<QString, bool> itemCompletionStatus;
};

// 库存结构体 (用于推车)
struct Inventory
{
    QMap<QString, int> cleanItems; // 物品类型 -> 数量
    int dirtyItemsCount = 0;       // 脏布草数量
};

// 游戏场景枚举
enum class GameScene
{
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
struct GameState
{
    GameScene currentScene; // 当前场景
    int currentFloor;       // 当前楼层: 0 (G), 2-10
    QList<Task> tasks;      // 任务列表
    Inventory inventory;    // 推车库存

    // 楼层库存 (Floor Inventory) - 独立于任务，记录放置在各楼层的物品
    // 结构: Map<楼层号, Map<物品名, 数量>>
    QMap<int, QMap<QString, int>> floorInventory;

    bool hasClockedIn;    // 是否已签到
    bool hasEverClockedIn = false; // 是否曾经签到过 (用于判定忘记打卡)
    bool hasReceivedTask; // 是否已领任务
    bool hasReported;     // 是否已汇报工作

    // 脏布草袋状态 (楼层 -> 是否有脏布草需要回收)
    QMap<int, bool> dirtyBagState;

    // 当前展开的任务索引 (用于手风琴效果)
    int expandedTaskIndex = -1;

    // Tip Flags (Ensure bubbles only show once per session)
    bool hasShownTipEntrance = false;
    bool hasShownTipWarehouse = false;
    bool hasShownTipShelf = false;
    bool hasShownTipLinenRoom = false;
};

// 错误日志 (用于最终汇报评估)
struct ErrorLog
{
    bool lateClockIn = false;             // 迟到
    bool missedEmergencyPriority = false; // 未优先处理紧急任务
    bool noReportBeforeHome = false;      // 下班前未汇报
    bool noClockOutBeforeHome = false;    // 下班前未打卡
    bool mixedLinen = false;              // 混装布草 (脏净混放)

    // 检查是否有任何错误
    bool hasErrors() const
    {
        return lateClockIn || missedEmergencyPriority || noReportBeforeHome || noClockOutBeforeHome || mixedLinen;
    }
};

class Test3 : public QWidget
{
    Q_OBJECT
public:
    explicit Test3(bool isDevMode, QWidget *parent = nullptr);

    // 设置开发者模式
    void setDeveloperMode(bool enabled) { isDeveloperMode = enabled; }
    // 设置突发事件模式
    void setEmergencyMode(bool enabled) { isEmergencyEnabled = enabled; }

    // 重置游戏状态
    void reset();

signals:
    void levelCompleted();        // 关卡完成信号
    void levelCancelled();        // 关卡取消信号
    void logMessage(QString msg); // 日志信号

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    bool isDeveloperMode;
    bool isEmergencyEnabled = false;
    GameState gameState;
    ErrorLog errorLog;

    // 逻辑计时器与标志
    QTimer *latenessTimer;          // 迟到判定计时器
    QTimer *emergencyTimer;         // 紧急事件计时器
    QTimer *heartbeatTimer;         // 心跳日志计时器 (调试用)
    bool isLate = false;            // 是否已迟到
    bool isTimerTriggered = false;  // 计时器是否已触发过
    bool isEmergencyActive = false; // 当前是否有未完成的紧急任务
    int m_roundCount = 1;           // 轮次计数
    QDateTime m_startTime;          // 开始时间

    // 资源缓存
    QMap<QString, QPixmap> m_sceneCache; // 场景与UI图片缓存

    // UI 元素
    QWidget *rpgCenterPanel;     // 游戏主画面面板
    QPushButton *tutorialBtn;    // 新手教程按钮
    QLabel *locationLabel;       // 位置显示标签
    QLabel *hoverHintLabel;      // 悬浮提示标签
    QLabel *cartStatusLabel;     // 推车状态图标
    QListWidget *taskListWidget; // 任务列表控件

    // 右侧侧边栏元素
    QLabel *inventoryTitleLabel;
    QLabel *cartCountLabel;                   // 推车数量显示
    DraggableListWidget *inventoryListWidget; // 可拖拽的库存列表
    QWidget *elevatorPanelContainer;          // 电梯楼层面板

    // 核心渲染逻辑
    void goToScene(GameScene scene); // 切换场景
    void renderScene();              // 渲染当前场景
    void updateRPGStatusLabels();    // 更新状态标签 (位置、推车)
    void refreshInventoryList();     // 刷新库存列表显示
    void refreshTaskList();          // 刷新任务列表显示

    // 获取图片 (带缓存)
    QPixmap getPixmap(const QString &path);

    // 格式化任务清单字符串
    QString formatTaskList() const;

    // 自适应布局更新
    void updateGameLayout();

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
    void handleClockIn();                   // 处理打卡上班
    void handleClockOut();                  // 处理打卡下班
    void handleGetTask();                   // 处理领取任务
    void handleReportWork();                // 处理汇报工作
    void handleGoHome();                    // 处理下班回家
    void handleElevatorButton(int floor);   // 处理电梯楼层选择
    void showTaskSheet(int taskIndex = -1); // 显示物资申领表
    void checkEmergencyTask();              // 检查紧急任务状态

    // 辅助函数
    int getNormalRandom(int min, int max); // 获取正态分布随机数
    void tryShowTip(GameScene scene); // 尝试显示气泡提示

    // 交互处理
    void handleInventoryDrop(QString itemName, const QMimeData *mimeData);           // 处理物品放入推车
    void handleSceneDrop(QString itemName, bool isWarehouse);                        // 处理物品从推车取出放置 (到货架或仓库)

    // 新手教程 (Deprecated but kept for compatibility or manual trigger)
    void showTutorial();

    // 生成占位图 (当资源缺失时)
    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
};

#endif // TEST3_H
