#include "test3.h"
#include "tutorial_overlay.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMap>
#include <QFile>
#include <cmath>
#include <QtMath>
#include <QMessageBox>
#include <QHeaderView>
#include "config.h"
#include "utils.h"
#include "logger.h"

Test3::Test3(bool isDevMode, QWidget *parent) : QWidget(parent), isDeveloperMode(isDevMode)
{
    // 主布局 (网格布局)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setContentsMargins(0, 0, 0, 0);
    mainGrid->setSpacing(0);

    QWidget *rpgContainer = new QWidget();
    QHBoxLayout *rpgLayout = new QHBoxLayout(rpgContainer);
    rpgLayout->setContentsMargins(0, 0, 0, 0);
    rpgLayout->setSpacing(0);

    // --- 左侧面板 ---
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    leftPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_LEFT);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // 返回主菜单按钮
    QPushButton *returnBtn = new QPushButton(Config::Test3::Texts::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test3::Geometry::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test3::Styles::BTN_RETURN_MENU);
    returnBtn->setCursor(Qt::PointingHandCursor);
    connect(returnBtn, &QPushButton::clicked, [this]()
            {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认退出", "确定要退出当前实训并返回主菜单吗？\n当前进度将不会保留。",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
             reset();
             emit levelCancelled();
        } });

    // 新手教程按钮
    tutorialBtn = new QPushButton(Config::Test3::Texts::BTN_TUTORIAL);
    tutorialBtn->setFixedSize(Config::Test3::Geometry::TUTORIAL_BTN_SIZE);
    tutorialBtn->setStyleSheet(Config::Test3::Styles::BTN_TUTORIAL);
    tutorialBtn->setCursor(Qt::PointingHandCursor);
    connect(tutorialBtn, &QPushButton::clicked, [this]()
            { showTutorial(); });

    locationLabel = new QLabel(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + "入口");
    locationLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE);

    // 推车状态图标
    cartStatusLabel = new QLabel();
    cartStatusLabel->setFixedSize(Config::Test3::Geometry::ICON_CART);
    cartStatusLabel->setScaledContents(true);

    // 布局调整：上20% (位置)，中20% (教程)，下50% (推车)，最下10% (返回)
    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(locationLabel);
    topLayout->addStretch();

    QVBoxLayout *midLayout = new QVBoxLayout();
    midLayout->addStretch();
    midLayout->addWidget(cartStatusLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

    QVBoxLayout *botLayout = new QVBoxLayout();
    botLayout->addWidget(returnBtn, 0, Qt::AlignCenter);

    leftLayout->addLayout(topLayout, 2);                    // 20%
    leftLayout->addWidget(tutorialBtn, 2, Qt::AlignCenter); // 20%
    leftLayout->addLayout(midLayout, 5);                    // 50%
    leftLayout->addLayout(botLayout, 1);                    // 10%

    rpgLayout->addWidget(leftPanel);

    // --- 中间面板 (游戏主画面) ---
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(Config::Test3::Geometry::CENTER_PANEL_SIZE);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    rpgCenterPanel->installEventFilter(this); // 安装过滤器用于 Dev 模式点击追踪
    rpgLayout->addWidget(rpgCenterPanel);

    // 悬浮提示标签 (全局单例，避免重复创建)
    hoverHintLabel = new QLabel(rpgCenterPanel);
    hoverHintLabel->setAlignment(Qt::AlignCenter);
    hoverHintLabel->setStyleSheet(Config::Test3::Styles::LBL_HOVER_HINT);
    hoverHintLabel->hide();
    hoverHintLabel->setGeometry(Config::Test3::Geometry::RECT_HOVER_HINT);

    // --- 右侧面板 ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 1. 任务标题
    QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(taskTitle);

    // 2. 任务列表 (TreeWidget) - 占 40%
    taskListWidget = new QTreeWidget();
    taskListWidget->setHeaderHidden(true);
    taskListWidget->setStyleSheet(Config::Test3::Styles::LIST_WIDGET);
    taskListWidget->setRootIsDecorated(true);
    rightLayout->addWidget(taskListWidget, 4); // 拉伸因子 4 (40%)

    // 查看申领表按钮
    QPushButton *viewTaskSheetBtn = new QPushButton(Config::Test3::Texts::BTN_VIEW_TASK_SHEET);
    viewTaskSheetBtn->setStyleSheet(QString("%1 %2").arg(Config::Test3::Styles::BTN_VIEW_TASK_SHEET, Config::Test3::Styles::BTN_ORANGE));
    viewTaskSheetBtn->setCursor(Qt::PointingHandCursor);
    connect(viewTaskSheetBtn, &QPushButton::clicked, [this]()
            {
        // 获取选中的任务，支持点击子项自动定位到父任务
        QTreeWidgetItem *item = taskListWidget->currentItem();
        int idx = 0;
        if (item) {
             if (item->parent()) item = item->parent();
             idx = taskListWidget->indexOfTopLevelItem(item);
        }
        showTaskSheet(idx); });
    rightLayout->addWidget(viewTaskSheetBtn);

    // 3. 库存标题
    inventoryTitleLabel = new QLabel(Config::Test3::Texts::LBL_INVENTORY_TITLE);
    inventoryTitleLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(inventoryTitleLabel);

    // 4. 库存列表 - 占 60%
    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY);
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName)
    {
        handleInventoryDrop(itemName);
    };
    rightLayout->addWidget(inventoryListWidget, 6); // 拉伸因子 6 (60%)

    // --- 电梯面板容器 (Sidebar) ---
    // 仅在电梯内部场景显示，替换库存列表
    elevatorPanelContainer = new QWidget();
    QVBoxLayout *elePanelLayout = new QVBoxLayout(elevatorPanelContainer);
    elePanelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *eleTitle = new QLabel(Config::Test3::Texts::LBL_ELEVATOR_PANEL_TITLE);
    eleTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    elePanelLayout->addWidget(eleTitle);

    QGridLayout *btnGrid = new QGridLayout();
    btnGrid->setSpacing(Config::Test3::Geometry::GRID_SPACING_ELEVATOR);

    // 创建电梯楼层按钮辅助函数
    auto createEleBtn = [&](int floor)
    {
        QString txt = (floor == 0) ? "G层" : QString("%1楼").arg(floor);
        QPushButton *btn = new QPushButton(txt);
        btn->setFixedSize(Config::Test3::Geometry::SIZE_ELEVATOR_BTN_SIDEBAR);
        btn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_FLOOR);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, [this, floor]()
                { handleElevatorButton(floor); });
        return btn;
    };

    // 布局楼层按钮
    btnGrid->addWidget(createEleBtn(9), 0, 0);
    btnGrid->addWidget(createEleBtn(10), 0, 1);
    btnGrid->addWidget(createEleBtn(7), 1, 0);
    btnGrid->addWidget(createEleBtn(8), 1, 1);
    btnGrid->addWidget(createEleBtn(5), 2, 0);
    btnGrid->addWidget(createEleBtn(6), 2, 1);
    btnGrid->addWidget(createEleBtn(3), 3, 0);
    btnGrid->addWidget(createEleBtn(4), 3, 1);
    btnGrid->addWidget(createEleBtn(0), 4, 0);
    btnGrid->addWidget(createEleBtn(2), 4, 1);

    elePanelLayout->addLayout(btnGrid);
    elePanelLayout->addStretch();

    // 默认隐藏，仅在 GameScene::ElevatorInside 显示
    rightLayout->addWidget(elevatorPanelContainer);
    elevatorPanelContainer->hide();

    rpgLayout->addWidget(rightPanel);
    mainGrid->addWidget(rpgContainer, 0, 0);

    // 迟到计时器
    latenessTimer = new QTimer(this);
    latenessTimer->setSingleShot(true);
    latenessTimer->setInterval(Config::Test3::Logic::TIME_LATE_THRESHOLD_SEC * 1000);
    connect(latenessTimer, &QTimer::timeout, [this]()
            {
        isLate = true;
        emit logMessage("计时结束: 标记为迟到状态");
        if (gameState.currentScene == GameScene::StaffHallway) {
            renderStaffHallway(); // 刷新背景显示迟到状态
        } });

    // 心跳日志计时器 (用于调试主线程活性)
    heartbeatTimer = new QTimer(this);
    heartbeatTimer->setInterval(1000);
    connect(heartbeatTimer, &QTimer::timeout, [this]()
            {
        static int count = 0;
        emit logMessage(QString("Test3 Heartbeat: %1s (Main Thread Active)").arg(++count)); });

    reset();
}

void Test3::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
}

// 缓存获取辅助函数
QPixmap Test3::getPixmap(const QString &path)
{
    if (m_sceneCache.contains(path))
    {
        return m_sceneCache.value(path);
    }
    QPixmap pix(path);
    if (!pix.isNull())
    {
        m_sceneCache.insert(path, pix);
    }
    return pix;
}

void Test3::reset()
{
    emit logMessage("Test3::reset() called");
    gameState.currentScene = GameScene::Entrance;
    gameState.currentFloor = 0;
    gameState.hasClockedIn = false;
    gameState.hasReceivedTask = false;
    gameState.hasReported = false;
    gameState.tasks.clear();
    gameState.inventory.cleanItems.clear();
    gameState.inventory.dirtyItemsCount = 0;
    gameState.dirtyBagState.clear();
    gameState.floorInventory.clear(); // 清空楼层库存

    isLate = false;
    isTimerTriggered = false;
    isEmergencyActive = false;
    latenessTimer->stop();
    errorLog = ErrorLog();

    taskListWidget->clear();
    inventoryListWidget->clear();

    emit logMessage("测试3: 重置状态");
    renderScene();
}

void Test3::installDevFilter(QWidget *widget)
{
    if (!isDeveloperMode)
        return;
    widget->installEventFilter(this);
    const QObjectList &children = widget->children();
    for (QObject *child : children)
    {
        if (child->isWidgetType())
        {
            installDevFilter(static_cast<QWidget *>(child));
        }
    }
}

bool Test3::eventFilter(QObject *watched, QEvent *event)
{
    // 开发者模式下，点击任何位置都会输出坐标，方便配置 ClickableArea
    if (event->type() == QEvent::MouseButtonPress && isDeveloperMode)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint globalPos = static_cast<QWidget *>(watched)->mapToGlobal(mouseEvent->pos());
        QPoint localPos = rpgCenterPanel->mapFromGlobal(globalPos);

        QString coordText = QString("DevMode Click: (%1, %2)").arg(localPos.x()).arg(localPos.y());
        qDebug() << coordText;
        emit logMessage(coordText);
    }
    return QWidget::eventFilter(watched, event);
}

// --- 逻辑处理 (Logic) ---

void Test3::goToScene(GameScene scene)
{
    emit logMessage(QString("goToScene: %1").arg((int)scene));

    // 迟到计时逻辑: 首次离开员工通道时开始计时
    if (gameState.currentScene == GameScene::StaffHallway && scene != GameScene::StaffHallway)
    {
        if (!gameState.hasClockedIn && !isTimerTriggered && !isLate)
        {
            isTimerTriggered = true;
            latenessTimer->start();
            emit logMessage("计时开始: 10秒迟到倒计时");
        }
    }

    gameState.currentScene = scene;
    emit logMessage("移动到场景: " + QString::number((int)scene));
    renderScene();
}

void Test3::updateRPGStatusLabels()
{
    // 更新位置文本
    QString locStr;
    switch (gameState.currentScene)
    {
    case GameScene::Entrance:
        locStr = "入口";
        break;
    case GameScene::StaffHallway:
        locStr = "员工通道";
        break;
    case GameScene::Office:
        locStr = "办公室";
        break;
    case GameScene::Warehouse:
        locStr = "布草仓库(入口)";
        break;
    case GameScene::WarehouseShelf:
        locStr = "布草仓库(货架)";
        break;
    case GameScene::ElevatorHall:
        locStr = (gameState.currentFloor == 0) ? "电梯厅 (G)" : QString("%1楼 电梯厅").arg(gameState.currentFloor);
        break;
    case GameScene::ElevatorInside:
        locStr = "电梯内";
        break;
    case GameScene::FloorCorridor:
        locStr = QString("%1楼 走廊").arg(gameState.currentFloor);
        break;
    case GameScene::LinenRoom:
        locStr = QString("%1楼 布草间").arg(gameState.currentFloor);
        break;
    }
    locationLabel->setText(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + locStr);

    // 更新推车图标
    QString statusImg;
    int cleanCount = 0;
    for (auto v : gameState.inventory.cleanItems)
        cleanCount += v;

    // 优先显示脏布草状态
    if (gameState.inventory.dirtyItemsCount > 0)
    {
        statusImg = Config::Test3::Images::UI_CART_DIRTY;
    }
    else if (cleanCount > 0)
    {
        statusImg = Config::Test3::Images::UI_CART_CLEAN;
    }
    else
    {
        statusImg = Config::Test3::Images::UI_CART_EMPTY;
    }

    QPixmap pix = getPixmap(statusImg);
    if (pix.isNull())
    {
        // 图片缺失时的文本回退
        cartStatusLabel->setText(gameState.inventory.dirtyItemsCount > 0 ? Config::Test3::Texts::LBL_CART_DIRTY
                                                                         : (cleanCount > 0 ? Config::Test3::Texts::LBL_CART_HAS_ITEMS : Config::Test3::Texts::LBL_CART_EMPTY));
        cartStatusLabel->setStyleSheet("border: 1px solid white; color: white;");
    }
    else
    {
        cartStatusLabel->setPixmap(pix);
        cartStatusLabel->setText("");
        cartStatusLabel->setStyleSheet("border: none;");
    }
}

void Test3::refreshInventoryList()
{
    inventoryListWidget->clear();
    // 显示干净布草
    for (auto it = gameState.inventory.cleanItems.begin(); it != gameState.inventory.cleanItems.end(); ++it)
    {
        if (it.value() > 0)
        {
            QListWidgetItem *item = new QListWidgetItem();

            // 格式: X条 大床单
            QString classifier = Config::Test3::Logic::CLASSIFIERS().value(it.key(), "个");
            item->setText(QString("%1%2 %3").arg(it.value()).arg(classifier).arg(it.key()));
            item->setData(Qt::UserRole, it.key());

            QString iconPath = Config::Test3::Images::ITEMS().value(it.key());
            if (QFile::exists(iconPath))
            {
                // 图标不缓存，因为 DraggableListWidget 会处理大小
                item->setIcon(QIcon(iconPath));
            }

            inventoryListWidget->addItem(item);
        }
    }

    // 显示脏布草
    if (gameState.inventory.dirtyItemsCount > 0)
    {
        QListWidgetItem *item = new QListWidgetItem();
        QString classifier = Config::Test3::Logic::CLASSIFIERS().value("脏布草", "件");
        item->setText(QString("%1%2 脏布草").arg(gameState.inventory.dirtyItemsCount).arg(classifier));

        QString iconPath = Config::Test3::Images::UI_DIRTY_LINEN;
        if (QFile::exists(iconPath))
            item->setIcon(QIcon(iconPath));
        item->setData(Qt::UserRole, "DirtyLinen");
        inventoryListWidget->addItem(item);
    }
    updateRPGStatusLabels();
}

void Test3::refreshTaskList()
{
    taskListWidget->clear();

    // 遍历任务
    for (int i = 0; i < gameState.tasks.size(); ++i)
    {
        const Task &t = gameState.tasks[i];

        // 状态显示逻辑: 优先显示手动标记状态，其次是系统完成状态
        QString status = "[进行中]";
        if (t.isMarkedComplete) {
            status = "[标记完成]";
        } else if (t.isCompleted) {
            status = "[已完成]";
        }

        QString headerText = QString("任务%1: %2楼 %3 %4")
                                 .arg(i + 1)
                                 .arg(t.targetFloor)
                                 .arg(t.isEmergency ? "[紧急]" : "")
                                 .arg(status);

        QTreeWidgetItem *topItem = new QTreeWidgetItem(taskListWidget);
        topItem->setText(0, headerText);

        // 子项: 需求列表 (固定)
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
        {
            if (it.value() > 0)
            {
                QString classifier = Config::Test3::Logic::CLASSIFIERS().value(it.key(), "个");
                QString subText = QString("%1: %2%3").arg(it.key()).arg(it.value()).arg(classifier);
                QTreeWidgetItem *subItem = new QTreeWidgetItem(topItem);
                subItem->setText(0, subText);
            }
        }
    }
    taskListWidget->expandAll();
}

// --- 场景渲染 (Scene Rendering) ---

void Test3::renderScene()
{
    emit logMessage(QString("renderScene: %1").arg((int)gameState.currentScene));
    // 清理中心面板的旧控件，但保留 hoverHintLabel
    QList<QObject *> children = rpgCenterPanel->children();
    for (QObject *child : children)
    {
        if (child == hoverHintLabel)
            continue;
        if (child->isWidgetType())
            static_cast<QWidget *>(child)->hide();
        child->deleteLater();
    }

    // 切换右侧侧边栏UI (电梯面板 vs 库存列表)
    QVBoxLayout *rightLayout = qobject_cast<QVBoxLayout *>(inventoryListWidget->parentWidget()->layout());
    if (gameState.currentScene == GameScene::ElevatorInside)
    {
        inventoryListWidget->hide();
        inventoryTitleLabel->hide();
        elevatorPanelContainer->show();

        // 调整比例：任务50%，电梯面板50%
        if (rightLayout)
        {
            rightLayout->setStretchFactor(taskListWidget, 5);
            rightLayout->setStretchFactor(elevatorPanelContainer, 5);
        }
    }
    else
    {
        elevatorPanelContainer->hide();
        inventoryTitleLabel->show();
        inventoryListWidget->show();

        // 恢复比例：任务40%，物品栏60%
        if (rightLayout)
        {
            rightLayout->setStretchFactor(taskListWidget, 4);
            rightLayout->setStretchFactor(inventoryListWidget, 6);
        }
    }

    // 根据场景类型调用渲染函数
    switch (gameState.currentScene)
    {
    case GameScene::Entrance:
        renderEntrance();
        break;
    case GameScene::StaffHallway:
        renderStaffHallway();
        break;
    case GameScene::Office:
        renderOffice();
        break;
    case GameScene::Warehouse:
        renderWarehouse();
        break;
    case GameScene::WarehouseShelf:
        renderWarehouseShelf();
        break;
    case GameScene::ElevatorHall:
        renderElevatorHall();
        break;
    case GameScene::ElevatorInside:
        renderElevatorInside();
        break;
    case GameScene::FloorCorridor:
        renderFloorCorridor();
        break;
    case GameScene::LinenRoom:
        renderLinenRoom();
        break;
    }
    updateRPGStatusLabels();
    installDevFilter(rpgCenterPanel); // 重新安装事件过滤器
    if (hoverHintLabel)
        hoverHintLabel->raise(); // 确保提示在最上层
}

void Test3::renderEntrance()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ENTRANCE);
    if (pix.isNull())
        pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 下班回家
    ArrowButton *btnHome = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(btnHome, Config::Test3::Geometry::RECT_BTN_ENTRANCE_HOME);
    btnHome->setAngle(Config::Test3::Geometry::ANGLE_BTN_ENTRANCE_HOME);
    btnHome->setArrowText("");
    btnHome->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    btnHome->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(btnHome, &QPushButton::clicked, this, &Test3::handleGoHome);
    connect(btnHome, &ArrowButton::hovered, [this](bool status, QString text)
            {
        QString tip = Config::Test3::Texts::TEXT_BTN_ENTRANCE_HOME;
        if (status) { hoverHintLabel->setText(tip); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnHome->show();

    // 进入酒店 (不规则点击区域)
    ClickableArea *btnEnter = new ClickableArea(rpgCenterPanel);
    btnEnter->setPolygon(Config::Test3::Geometry::POLY_ENTRANCE_ENTER());
    btnEnter->setToolTip(Config::Test3::Texts::BTN_ENTER_HOTEL);
    connect(btnEnter, &ClickableArea::clicked, [this]()
            { goToScene(GameScene::StaffHallway); });
    connect(btnEnter, &ClickableArea::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnEnter->setGeometry(rpgCenterPanel->rect());
    btnEnter->show();
}

void Test3::renderStaffHallway()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QString bgPath = Config::Test3::Images::SCENE_HALLWAY_NORMAL;

    // 根据状态选择背景图
    bool anyTaskDone = false;
    for (const auto &t : gameState.tasks)
        if (t.isCompleted)
            anyTaskDone = true;

    if (anyTaskDone)
    {
        bgPath = Config::Test3::Images::SCENE_HALLWAY_CLOCKED_OUT; // 实际逻辑: 任务完成后通常是下班状态
    }
    else if (isLate)
    {
        bgPath = Config::Test3::Images::SCENE_HALLWAY_LATE;
    }

    QPixmap pix = getPixmap(bgPath);
    if (pix.isNull())
        pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 返回入口
    ArrowButton *exitBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_HALLWAY_EXIT);
    exitBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_HALLWAY_EXIT);
    exitBtn->setArrowText("");
    exitBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    exitBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(exitBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::Entrance); });
    connect(exitBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_ENTRANCE); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    exitBtn->show();

    // 打卡区域
    ClickableArea *btnClock = new ClickableArea(rpgCenterPanel);
    btnClock->setPolygon(Config::Test3::Geometry::POLY_HALLWAY_CLOCK());
    QString clockText = gameState.hasClockedIn ? Config::Test3::Texts::BTN_CLOCK_OUT : Config::Test3::Texts::BTN_CLOCK_IN;
    btnClock->setToolTip(clockText);
    connect(btnClock, &ClickableArea::clicked, [this]()
            {
        if (gameState.hasClockedIn) handleClockOut();
        else handleClockIn(); });
    connect(btnClock, &ClickableArea::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnClock->setGeometry(rpgCenterPanel->rect());
    btnClock->show();

    // 导航箭头生成辅助
    auto createArrow = [&](const QRect &rect, int angle, QString text, auto func)
    {
        ArrowButton *btn = new ArrowButton(rpgCenterPanel);
        setGeometryCentered(btn, rect);
        btn->setAngle(angle);
        btn->setArrowText("");
        btn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
        btn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
        connect(btn, &QPushButton::clicked, func);
        connect(btn, &ArrowButton::hovered, [this, text](bool status, QString t)
                {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide(); });
        btn->show();
    };

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_OFFICE,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_OFFICE,
                Config::Test3::Texts::BTN_GO_OFFICE,
                [this]()
                { goToScene(GameScene::Office); });

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_WAREHOUSE,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_WAREHOUSE,
                Config::Test3::Texts::BTN_GO_WAREHOUSE,
                [this]()
                { goToScene(GameScene::Warehouse); });

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_ELEVATOR,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_ELEVATOR,
                Config::Test3::Texts::BTN_GO_ELEVATOR,
                [this]()
                { goToScene(GameScene::ElevatorHall); });
}

void Test3::renderOffice()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_OFFICE);
    if (pix.isNull())
        pix = generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 动作按钮 (领任务 / 汇报)
    QPushButton *actionBtn = new QPushButton(rpgCenterPanel);
    setGeometryCentered(actionBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_ACTION);
    actionBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_OFFICE_ACTION);
    actionBtn->setCursor(Qt::PointingHandCursor);

    if (gameState.hasReceivedTask)
    {
        actionBtn->setText(Config::Test3::Texts::BTN_REPORT_WORK);
        connect(actionBtn, &QPushButton::clicked, this, &Test3::handleReportWork);
    }
    else
    {
        actionBtn->setText(Config::Test3::Texts::BTN_GET_TASK);
        connect(actionBtn, &QPushButton::clicked, this, &Test3::handleGetTask);
    }
    actionBtn->show();

    // 返回按钮
    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_OFFICE_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();

    if (gameState.hasReported)
    {
        QLabel *lbl = new QLabel(Config::Test3::Texts::LBL_WORK_REPORTED, rpgCenterPanel);
        setGeometryCentered(lbl, Config::Test3::Geometry::RECT_LBL_OFFICE_MSG);
        lbl->setStyleSheet(Config::Test3::Styles::LBL_SUCCESS_GREEN);
        lbl->show();
    }
}

void Test3::renderWarehouse()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_WAREHOUSE_ENTRY);
    if (pix.isNull())
        pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 脏布草回收桶 (可拖入)
    DropLabel *dirtyBin = new DropLabel("", rpgCenterPanel);
    QPixmap binPix = getPixmap(Config::Test3::Images::UI_DIRTY_BIN);
    if (!binPix.isNull())
    {
        dirtyBin->setPixmap(binPix.scaled(Config::Test3::Geometry::RECT_WAREHOUSE_BIN.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        dirtyBin->setAlignment(Qt::AlignCenter);
    }
    dirtyBin->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
    dirtyBin->setToolTip(Config::Test3::Texts::LBL_DIRTY_BIN_TIP);
    dirtyBin->onDropCallback = [this](QString item)
    {
        if (item == "DirtyLinen" || item == "脏布草")
        {
            gameState.inventory.dirtyItemsCount = 0;
            emit logMessage("脏布草已回收");
            refreshInventoryList();
            QMessageBox::information(this, "提示", "脏布草已放入回收桶。");
        }
        else
        {
            QMessageBox::warning(this, "提示", "这里只能放脏布草。");
        }
    };
    setGeometryCentered(dirtyBin, Config::Test3::Geometry::RECT_WAREHOUSE_BIN);
    dirtyBin->show();

    // 拿取布草按钮
    QPushButton *takeBtn = new QPushButton(Config::Test3::Texts::BTN_TAKE_LINEN, rpgCenterPanel);
    setGeometryCentered(takeBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_TAKE);
    takeBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_WAREHOUSE_TAKE);
    takeBtn->setCursor(Qt::PointingHandCursor);
    connect(takeBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::WarehouseShelf); });
    takeBtn->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_WAREHOUSE_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderWarehouseShelf()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_WAREHOUSE_SHELF);
    if (pix.isNull())
        pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 创建货架区域 lambda
    auto createShelfArea = [&](const QString &name, const QRect &rect)
    {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true);
        area->setSourceType("WarehouseShelf"); // 标记源为仓库货架 (无限供应)
        area->setToolTip(name + " (可拿取)");
        connect(area, &ShelfArea::hovered, [this](bool status, QString text)
                {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide(); });

        QString iconPath = Config::Test3::Images::ITEMS().value(name);
        if (QFile::exists(iconPath))
        {
            QPixmap pix = getPixmap(iconPath);
            if (!pix.isNull())
            {
                area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                area->setAlignment(Qt::AlignCenter);
            }
        }

        // 放置回调 (从车放回货架)
        area->onDropCallback = [this, name](QString item)
        {
            if (item != name)
            {
                QMessageBox::critical(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name));
                return;
            }
            handleSceneDrop(item, true); // true 表示放回仓库
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_TOWEL);

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_SHELF_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_SHELF_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::Warehouse); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderElevatorHall()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_HALL);
    if (pix.isNull())
        pix = generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *callElevator = new QPushButton(Config::Test3::Texts::BTN_ENTER_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(callElevator, Config::Test3::Geometry::RECT_BTN_ELEVATOR_ENTER);
    callElevator->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_ENTER);
    callElevator->setCursor(Qt::PointingHandCursor);
    connect(callElevator, &QPushButton::clicked, [this]()
            { goToScene(GameScene::ElevatorInside); });
    callElevator->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_ELEVATOR_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]()
            {
        if (gameState.currentFloor == 0) goToScene(GameScene::StaffHallway);
        else goToScene(GameScene::FloorCorridor); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_BACK); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderElevatorInside()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_INSIDE);
    if (pix.isNull())
        pix = generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *exitBtn = new QPushButton(Config::Test3::Texts::BTN_EXIT_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_EXIT);
    exitBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_EXIT);
    exitBtn->setCursor(Qt::PointingHandCursor);
    connect(exitBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::ElevatorHall); });
    exitBtn->show();
}

void Test3::renderFloorCorridor()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_FLOOR_CORRIDOR);
    if (pix.isNull())
        pix = generatePlaceholder("走廊", Qt::cyan, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    ArrowButton *linenRoomBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(linenRoomBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_LINEN);
    linenRoomBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_LINEN);
    linenRoomBtn->setArrowText("");
    linenRoomBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    linenRoomBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(linenRoomBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::LinenRoom); });
    connect(linenRoomBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_LINEN_ROOM); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    linenRoomBtn->show();

    ArrowButton *elevatorBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(elevatorBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_ELEVATOR);
    elevatorBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_ELEVATOR);
    elevatorBtn->setArrowText("");
    elevatorBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    elevatorBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(elevatorBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::ElevatorHall); });
    connect(elevatorBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_ELEVATOR_HALL); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    elevatorBtn->show();
}

void Test3::renderLinenRoom()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_LINEN_ROOM_EMPTY);
    if (pix.isNull())
        pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 初始化楼层的脏布草状态 (如果不存在)
    if (!gameState.dirtyBagState.contains(gameState.currentFloor))
    {
        gameState.dirtyBagState[gameState.currentFloor] = false;
    }

    auto createShelfArea = [&](const QString &name, const QRect &rect)
    {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setSourceType("LinenRoomShelf"); // 标记源为布草间货架
        area->setToolTip(name + " (拖拽取回/放置)");
        connect(area, &ShelfArea::hovered, [this](bool status, QString text)
                {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide(); });

        // 检查楼层库存 (Floor Inventory)
        int placedCount = gameState.floorInventory[gameState.currentFloor].value(name, 0);

        if (placedCount > 0)
        {
            area->setDraggable(true);
            QString iconPath = Config::Test3::Images::ITEMS().value(name);
            if (QFile::exists(iconPath))
            {
                QPixmap pix = getPixmap(iconPath);
                if (!pix.isNull())
                {
                    area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                    area->setAlignment(Qt::AlignCenter);
                }
            }
            // 显示数量角标
            QLabel *countLbl = new QLabel(QString("x%1").arg(placedCount), area);
            countLbl->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px; background: transparent; padding: 2px;")
                                        .arg(Config::Test3::Fonts::COL_LINEN_COUNT)
                                        .arg(Config::Test3::Fonts::SIZE_LINEN_COUNT));
            countLbl->adjustSize();
            countLbl->move((area->width() - countLbl->width()) / 2, (area->height() - countLbl->height()) / 2);
            countLbl->show();
        }
        else
        {
            area->setDraggable(false);
            area->clear();
        }

        // 放置回调
        area->onDropCallback = [this, name, area](QString item)
        {
            if (item != name)
            {
                QMessageBox::critical(this, "错误", QString("放置失败：不能将 %1 放置在 %2 的位置！").arg(item, name));
                return;
            }
            // 放置到楼层 (false = 不是仓库)
            handleSceneDrop(item, false);
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_LINEN_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_LINEN_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_LINEN_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_LINEN_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_LINEN_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_LINEN_TOWEL);

    // 脏布草 (如果存在)
    if (gameState.dirtyBagState[gameState.currentFloor])
    {
        DragSourceLabel *dirty = new DragSourceLabel("脏布草", rpgCenterPanel);
        QPixmap dirtyPix = getPixmap(Config::Test3::Images::UI_DIRTY_LINEN);
        if (!dirtyPix.isNull())
            dirty->setPixmap(dirtyPix.scaled(Config::Test3::Geometry::ICON_DIRTY_DRAG));
        setGeometryCentered(dirty, Config::Test3::Geometry::RECT_EVENT_DIRTY_LINEN);
        dirty->show();
    }

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_LINEN_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_LINEN_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]()
            { goToScene(GameScene::FloorCorridor); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_CORRIDOR); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

// --- 动作处理 (Action Handlers) ---

void Test3::handleInventoryDrop(QString itemName, const QMimeData *mimeData)
{
    if (itemName == "脏布草")
    {
        // 混装检查: 如果车里有干净布草，则记录错误
        int cleanCount = 0;
        for (auto c : gameState.inventory.cleanItems)
            cleanCount += c;

        if (cleanCount > 0)
        {
            errorLog.mixedLinen = true;
        }

        gameState.inventory.dirtyItemsCount++;
        gameState.dirtyBagState[gameState.currentFloor] = false; // 清除楼层的脏布草
        emit logMessage("回收脏布草");
        renderScene();
    }
    else
    {
        // 混装检查: 脏布草在车上时装干净布草
        if (gameState.inventory.dirtyItemsCount > 0)
        {
            errorLog.mixedLinen = true;
        }

        if (gameState.currentScene == GameScene::LinenRoom)
        {
            // 从楼层货架取回 -> 减少楼层库存
            if (gameState.floorInventory[gameState.currentFloor].value(itemName, 0) > 0)
            {
                gameState.floorInventory[gameState.currentFloor][itemName]--;
                emit logMessage("从布草间货架取回: " + itemName);
            }
        }

        // 增加推车库存 (默认+1，因为是拖拽一个图标)
        gameState.inventory.cleanItems[itemName]++;
        emit logMessage("装车: " + itemName);

        if (gameState.currentScene == GameScene::LinenRoom)
        {
            renderScene(); // 刷新以更新数量角标
        }
    }
    refreshInventoryList();
}

void Test3::handleSceneDrop(QString itemName, bool isWarehouse)
{
    int countInCart = gameState.inventory.cleanItems[itemName];
    if (countInCart <= 0)
        return;

    if (isWarehouse)
    {
        // 放回仓库：清空推车该物品
        gameState.inventory.cleanItems[itemName] = 0;
        emit logMessage(QString("放回仓库: %1 x%2").arg(itemName).arg(countInCart));
        refreshInventoryList();
        return;
    }

    // 放置到楼层：全部放入
    gameState.inventory.cleanItems[itemName] = 0;
    gameState.floorInventory[gameState.currentFloor][itemName] += countInCart;

    emit logMessage(QString("放置 %1 x%2 到 %3楼货架").arg(itemName).arg(countInCart).arg(gameState.currentFloor));

    // 检查任务完成情况
    bool anyUpdate = false;
    for (int i = 0; i < gameState.tasks.size(); ++i)
    {
        Task &t = gameState.tasks[i];
        if (t.targetFloor == gameState.currentFloor && !t.isCompleted)
        {
            // 检查该任务的所有需求是否满足
            bool allMet = true;
            for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
            {
                QString name = it.key();
                int required = it.value();
                int current = gameState.floorInventory[gameState.currentFloor].value(name, 0);
                if (current < required)
                {
                    allMet = false;
                    break;
                }
            }

            if (allMet)
            {
                t.isCompleted = true;
                anyUpdate = true;
                emit logMessage("任务完成: " + QString::number(gameState.currentFloor) + "楼");

                // 紧急任务优先级检查
                if (isEmergencyActive)
                {
                    bool emergencyDone = false;
                    for (const auto &et : gameState.tasks)
                        if (et.isEmergency && et.isCompleted)
                            emergencyDone = true;

                    // 如果当前完成的不是紧急任务，且紧急任务还未完成 -> 错误
                    if (!t.isEmergency && !emergencyDone)
                    {
                        errorLog.missedEmergencyPriority = true;
                        emit logMessage("错误: 未优先完成紧急任务");
                    }
                    else if (t.isEmergency)
                    {
                        isEmergencyActive = false; // 紧急任务已解决
                    }
                }
            }
        }
    }

    // 全局完成检查
    if (anyUpdate)
    {
        bool allAllDone = true;
        for (const auto &checkT : gameState.tasks)
            if (!checkT.isCompleted)
                allAllDone = false;
        if (allAllDone)
        {
            QMessageBox::information(this, "提示", "所有任务已完成！请回办公室汇报。");
        }
    }

    refreshInventoryList();
    refreshTaskList();
    renderScene();
}

void Test3::checkEmergencyTask()
{
    // 占位符，实际逻辑集成在定时器中
}

int Test3::getNormalRandom(int min, int max)
{
    // Box-Muller 变换生成正态分布
    double u1 = QRandomGenerator::global()->generateDouble();
    while (u1 <= 0.0)
        u1 = QRandomGenerator::global()->generateDouble();

    double u2 = QRandomGenerator::global()->generateDouble();
    double randStd = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

    // 调整均值和标准差 (假设均值5，标准差2)
    int val = std::round(5.0 + 2.0 * randStd);
    if (val < min)
        val = min;
    if (val > max)
        val = max;
    return val;
}

void Test3::handleClockIn()
{
    if (latenessTimer->isActive())
    {
        latenessTimer->stop();
    }

    if (isLate)
    {
        errorLog.lateClockIn = true;
        QMessageBox::warning(this, "通知", "打卡成功，但你迟到了！");
        emit logMessage("上班打卡 (迟到)");
    }
    else
    {
        QMessageBox::information(this, "通知", "上班打卡成功!");
        emit logMessage("上班打卡 (正常)");
    }

    gameState.hasClockedIn = true;
    renderScene();
}

void Test3::handleClockOut()
{
    emit logMessage("已打卡下班");
    gameState.hasClockedIn = false;
    QMessageBox::information(this, "通知", "下班打卡成功! 请回到入口回家。");
    renderScene();
}

void Test3::handleGetTask()
{
    if (gameState.hasReceivedTask)
        return;

    // 任务生成 Lambda
    auto generateTask = [this](int floor, bool emergency) -> Task
    {
        Task t;
        t.targetFloor = floor;
        t.isEmergency = emergency;
        t.isCompleted = false;

        QStringList allTypes = {"大床单", "大被套", "小被套", "枕巾", "晚安巾", "毛巾"};
        // Fisher-Yates 洗牌算法随机排列
        for (int i = 0; i < allTypes.size(); ++i)
        {
            int j = QRandomGenerator::global()->bounded(allTypes.size());
            allTypes.swapItemsAt(i, j);
        }

        int typesCount = QRandomGenerator::global()->bounded(4, Config::Test3::Logic::MAX_TASK_ITEM_TYPES + 1);
        for (int i = 0; i < typesCount; ++i)
        {
            int count = getNormalRandom(1, Config::Test3::Logic::MAX_TASK_ITEM_COUNT);
            t.requiredItems.insert(allTypes[i], count);
        }
        return t;
    };

    // 生成两个固定基础任务
    gameState.tasks.append(generateTask(Config::Test3::Logic::TASK_FIXED_FLOOR_1, false));
    gameState.tasks.append(generateTask(Config::Test3::Logic::TASK_FIXED_FLOOR_2, false));

    // 突发事件逻辑
    if (isEmergencyEnabled)
    {
        bool isEventB = QRandomGenerator::global()->bounded(2) == 0; // 50% 概率

        if (isEventB)
        {
            // 事件 B: 脏布草
            int targetFloor = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
            gameState.dirtyBagState[targetFloor] = true;
            emit logMessage("突发事件B: 脏布草回收");
        }
        else
        {
            // 事件 A: 延迟紧急任务
            int delay = QRandomGenerator::global()->bounded(20000, 40001); // 20-40秒
            QTimer::singleShot(delay, this, [this, generateTask]()
                               {
                // 如果所有任务已完成，不再触发
                bool allDone = true;
                for(const auto &t : gameState.tasks) if(!t.isCompleted) allDone = false;
                if (allDone) return;

                int floorA;
                do {
                    floorA = QRandomGenerator::global()->bounded(2, 11);
                } while (floorA == 6 || floorA == 7); // 避开基础层

                Task t = generateTask(floorA, true);
                gameState.tasks.append(t);
                isEmergencyActive = true;

                QString msg = QString(Config::Test3::Texts::POPUP_EMERGENCY_MANAGER).arg(floorA);
                QMessageBox::warning(this, "突发事件", msg);
                emit logMessage(QString("突发事件A: %1楼").arg(floorA));
                refreshTaskList(); });
        }
    }

    gameState.hasReceivedTask = true;
    refreshTaskList();
    emit logMessage("领取任务完成");

    // 自动弹出申领表
    showTaskSheet(0);
    renderScene();
}

void Test3::handleReportWork()
{
    QString msg = Config::Test3::Texts::REPORT_SUCCESS;
    QStringList errors;

    bool incomplete = false;
    // 检查所有任务是否满足要求
    for (const auto &t : gameState.tasks)
    {
        // 实时检查库存 (System Check)
        bool isMet = true;
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
        {
            int current = gameState.floorInventory[t.targetFloor].value(it.key(), 0);
            if (current < it.value())
            {
                isMet = false;
                break;
            }
        }

        if (!isMet)
            incomplete = true;
    }

    if (incomplete)
        errors << Config::Test3::Texts::REPORT_ERR_MISSING_TASK;

    if (errorLog.lateClockIn)
        errors << Config::Test3::Texts::REPORT_ERR_LATE;
    if (errorLog.missedEmergencyPriority)
        errors << Config::Test3::Texts::REPORT_ERR_PRIORITY;
    if (errorLog.mixedLinen)
        errors << Config::Test3::Texts::ERR_LOG_MIXED_LINEN;

    if (!errors.isEmpty())
    {
        msg = errors.join("\n");
    }

    QMessageBox::information(this, "汇报结果", msg);
    gameState.hasReported = true;
    emit logMessage("已汇报工作: " + msg);
    renderScene();
}

void Test3::handleGoHome()
{
    if (!gameState.hasReported)
        errorLog.noReportBeforeHome = true;
    if (gameState.hasClockedIn)
        errorLog.noClockOutBeforeHome = true;

    if (errorLog.noReportBeforeHome)
        emit logMessage("错误: 下班前未汇报");
    if (errorLog.noClockOutBeforeHome)
        emit logMessage("错误: 下班前未打卡");

    // --- 填充简要报表数据 ---
    Logger::Test3BriefData data;

    data.clockInStatus = errorLog.lateClockIn ? "迟到" : "正常";
    data.clockOutStatus = errorLog.noClockOutBeforeHome ? "未打卡" : "已打卡";
    data.isLate = errorLog.lateClockIn;
    data.emergencyPriorityMet = !errorLog.missedEmergencyPriority;
    data.mixedLinen = errorLog.mixedLinen;

    for (const auto &t : gameState.tasks)
    {
        Logger::Test3BriefData::FloorStatus fs;
        fs.floor = t.targetFloor;

        // 最终检查: 实际库存 vs 需求 (忽略中间过程状态)
        bool isCorrect = true;
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
        {
            int current = gameState.floorInventory[t.targetFloor].value(it.key(), 0);
            if (current < it.value())
            {
                isCorrect = false;
                break;
            }
        }

        fs.isCorrect = isCorrect;
        if (isCorrect)
            fs.details = "正确完成";
        else
            fs.details = "未满足需求";
        data.floorStatuses.append(fs);
    }

    Logger::instance().test3Data = data;
    Logger::instance().generateBriefReport(); // 生成最终报表

    emit levelCompleted();
}

void Test3::handleElevatorButton(int floor)
{
    emit logMessage(QString("电梯前往 %1 楼").arg(floor));
    QTimer::singleShot(500, this, [this, floor]()
                       {
        gameState.currentFloor = floor;
        emit logMessage(QString("抵达 %1 楼").arg(floor));
        goToScene(GameScene::ElevatorHall); });
}

void Test3::showTaskSheet(int taskIndex)
{
    if (gameState.tasks.isEmpty())
    {
        QMessageBox::information(this, "提示", "当前没有任务。");
        return;
    }
    if (taskIndex < 0 || taskIndex >= gameState.tasks.size())
        taskIndex = 0;

    Task *tPtr = &gameState.tasks[taskIndex];

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(QString("物资申领表 (任务 %1)").arg(taskIndex + 1));
    dlg->setFixedSize(Config::Test3::Geometry::SHEET_DIALOG);

    installDevFilter(dlg);

    QLabel *bg = new QLabel(dlg);
    QPixmap pix = getPixmap(Config::Test3::Images::UI_TASK_SHEET);
    if (pix.isNull())
    {
        pix = QPixmap(Config::Test3::Geometry::SHEET_DIALOG);
        pix.fill(Qt::white);
    }
    else
    {
        pix = pix.scaled(Config::Test3::Geometry::SHEET_DIALOG, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPainter painter(&pix);
    painter.setPen(QColor(0, 0, 0));
    painter.setFont(QFont(Config::Test3::Fonts::SHEET_FONT_FAMILY,
                          Config::Test3::Fonts::SHEET_FONT_SIZE,
                          Config::Test3::Fonts::SHEET_FONT_WEIGHT));

    auto drawCenteredText = [&](QPoint center, QString text)
    {
        int w = Config::Test3::Geometry::SHEET_TEXT_BOX.width();
        int h = Config::Test3::Geometry::SHEET_TEXT_BOX.height();
        QRect rect(center.x() - w / 2, center.y() - h / 2, w, h);
        painter.drawText(rect, Qt::AlignCenter, text);
    };

    drawCenteredText(Config::Test3::Geometry::TXT_FLOOR, QString::number(tPtr->targetFloor));

    QMap<QString, QPoint> itemCoords;
    itemCoords["大床单"] = Config::Test3::Geometry::TXT_SHEET;
    itemCoords["大被套"] = Config::Test3::Geometry::TXT_DUVET;
    itemCoords["小被套"] = Config::Test3::Geometry::TXT_S_DUVET;
    itemCoords["枕巾"] = Config::Test3::Geometry::TXT_PILLOW;
    itemCoords["晚安巾"] = Config::Test3::Geometry::TXT_GN_TOWEL;
    itemCoords["毛巾"] = Config::Test3::Geometry::TXT_TOWEL;

    // 显示需求量 (固定)
    for (auto it = itemCoords.begin(); it != itemCoords.end(); ++it)
    {
        int count = tPtr->requiredItems.value(it.key(), 0);
        drawCenteredText(it.value(), QString::number(count));
    }

    bg->setPixmap(pix);
    bg->setGeometry(0, 0, Config::Test3::Geometry::SHEET_DIALOG.width(), Config::Test3::Geometry::SHEET_DIALOG.height());

    // --- "标记为完成" 按钮与印章 ---

    // 盖章 Label (初始隐藏或显示取决于状态)
    QLabel *stampLbl = new QLabel("✔", bg);
    stampLbl->setStyleSheet(Config::Test3::Styles::STYLE_LBL_CHECKMARK);
    stampLbl->adjustSize();
    stampLbl->move(Config::Test3::Geometry::SHEET_DIALOG.width() - stampLbl->width() - 50,
                   Config::Test3::Geometry::SHEET_DIALOG.height() - stampLbl->height() - 50);
    stampLbl->setVisible(tPtr->isMarkedComplete);

    // 标记按钮
    QPushButton *markBtn = new QPushButton(dlg);
    markBtn->setText(tPtr->isMarkedComplete ? "取消标记" : "标记为完成");
    markBtn->setGeometry(Config::Test3::Geometry::RECT_BTN_MARK_COMPLETE);
    markBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_MARK_COMPLETE);
    markBtn->setCursor(Qt::PointingHandCursor);

    connect(markBtn, &QPushButton::clicked, [this, tPtr, markBtn, stampLbl]()
            {
        tPtr->isMarkedComplete = !tPtr->isMarkedComplete;
        stampLbl->setVisible(tPtr->isMarkedComplete);
        markBtn->setText(tPtr->isMarkedComplete ? "取消标记" : "标记为完成");

        QString msg = tPtr->isMarkedComplete ? "任务标记为完成" : "取消任务标记";
        Logger::instance().logAction("Test3", msg);

        // 实时刷新右侧任务列表状态
        refreshTaskList();
    });

    dlg->exec();
}

void Test3::showTutorial()
{
    emit logMessage("显示教程: " + QString::number((int)gameState.currentScene));

    if (tutorialBtn)
        tutorialBtn->setEnabled(false);

    // 使用自定义 Widget 覆盖在 Test3 (this) 上
    TutorialOverlay *overlay = new TutorialOverlay(this);
    overlay->setGeometry(this->rect()); // 全屏覆盖
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 0.5);");
    // 监听 destroyed 信号以恢复按钮状态
    connect(overlay, &QObject::destroyed, [this]()
            {
        if (tutorialBtn) tutorialBtn->setEnabled(true); });

    // 使用布局使内容框垂直居中
    QVBoxLayout *layout = new QVBoxLayout(overlay);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    // 内容框
    QWidget *contentBox = new QWidget(overlay);
    contentBox->setFixedSize(Config::Test3::Geometry::RECT_TUTORIAL_OVERLAY.size());
    contentBox->setStyleSheet("background-color: rgba(0, 0, 0, 0.85); border-radius: 12px; border: 2px solid white;");

    layout->addWidget(contentBox);

    // 确定文本内容 和 图片路径
    QString tutorialText = Config::Test3::Texts::TUTORIAL_GENERAL;
    QString imagePath = Config::Test3::Images::IMAGE_TUTORIAL_GENERAL;

    if (gameState.currentScene == GameScene::WarehouseShelf || gameState.currentScene == GameScene::LinenRoom)
    {
        tutorialText = Config::Test3::Texts::TUTORIAL_SHELF;
        imagePath = Config::Test3::Images::IMAGE_TUTORIAL_SHELF;
    }
    else if (gameState.currentScene == GameScene::Warehouse)
    {
        tutorialText = Config::Test3::Texts::TUTORIAL_WAREHOUSE_ENTRY;
        imagePath = Config::Test3::Images::IMAGE_TUTORIAL_WAREHOUSE;
    }

    // 图片显示区域
    QLabel *imgLbl = new QLabel(contentBox);
    imgLbl->setGeometry(Config::Test3::Geometry::RECT_TUTORIAL_IMAGE);
    imgLbl->setAlignment(Qt::AlignCenter);

    QPixmap pix = getPixmap(imagePath);
    if (!pix.isNull())
    {
        imgLbl->setPixmap(pix.scaled(imgLbl->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    else
    {
        imgLbl->setText("示意图 (未找到图片)");
        imgLbl->setStyleSheet("color: #aaa; font-size: 20px; border: 2px dashed #555;");
    }

    // 文本显示区域
    QLabel *txtLbl = new QLabel(tutorialText, contentBox);
    txtLbl->setGeometry(Config::Test3::Geometry::RECT_TUTORIAL_TEXT);
    txtLbl->setStyleSheet(Config::Test3::Styles::TUTORIAL_TEXT_STYLE);
    txtLbl->setWordWrap(true);
    txtLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    overlay->show();
    overlay->raise();
}

QPixmap Test3::generatePlaceholder(QString text, QColor color, QSize size)
{
    // 动态生成，不缓存
    emit logMessage("Generating Placeholder for: " + text);
    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
