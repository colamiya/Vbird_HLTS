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
#include <QCheckBox>
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
    leftPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_LEFT_WIDTH);
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
    locationLabel->setWordWrap(true);

    // 推车状态图标
    cartStatusLabel = new QLabel();
    cartStatusLabel->setFixedSize(Config::Test3::Geometry::ICON_CART);
    cartStatusLabel->setScaledContents(true);

    // 布局调整
    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(locationLabel);
    topLayout->addStretch();

    QVBoxLayout *midLayout = new QVBoxLayout();
    midLayout->addStretch();
    midLayout->addWidget(cartStatusLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

    QVBoxLayout *botLayout = new QVBoxLayout();
    botLayout->addWidget(returnBtn, 0, Qt::AlignCenter);

    leftLayout->addLayout(topLayout, 2);
    leftLayout->addWidget(tutorialBtn, 2, Qt::AlignCenter);
    leftLayout->addLayout(midLayout, 5);
    leftLayout->addLayout(botLayout, 1);

    rpgLayout->addWidget(leftPanel);

    // --- 中间面板 (游戏主画面) ---
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(Config::Test3::Geometry::CENTER_PANEL_SIZE);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    rpgCenterPanel->installEventFilter(this);
    rpgLayout->addWidget(rpgCenterPanel);

    // 悬浮提示标签 (全局单例)
    hoverHintLabel = new QLabel(rpgCenterPanel);
    hoverHintLabel->setAlignment(Qt::AlignCenter);
    hoverHintLabel->setStyleSheet(Config::Test3::Styles::LBL_HOVER_HINT);
    hoverHintLabel->hide();
    hoverHintLabel->setGeometry(Config::Test3::Geometry::RECT_HOVER_HINT);

    // --- 右侧面板 ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_RIGHT_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 1. 任务标题
    QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT);
    rightLayout->addWidget(taskTitle);

    // 2. 任务列表 (TreeWidget) - 占 50%
    taskListWidget = new QTreeWidget();
    taskListWidget->setStyleSheet(Config::Test3::Styles::LIST_WIDGET);
    taskListWidget->setRootIsDecorated(true);
    // 设置列头: 物品, 需求量, 完成标记
    taskListWidget->setHeaderLabels(QStringList() << "物品" << "需求" << "完成");
    taskListWidget->setHeaderHidden(false);
    // 调整列宽 (70%, 15%, 15% of 330px)
    // 330 * 0.7 = 231, 330 * 0.15 = 49.5 ~ 50
    taskListWidget->header()->resizeSection(0, 230);
    taskListWidget->header()->resizeSection(1, 50);
    taskListWidget->header()->resizeSection(2, 50);
    // 禁止拖动表头(禁止列重排)，防止标题“跟着跑”
    taskListWidget->header()->setSectionsMovable(false);

    // 点击事件处理 (用于切换完成状态)
    connect(taskListWidget, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item, int column) {
        // 只处理第3列 (Index 2) 且该项是子项 (有父节点)
        if (column == 2 && item->parent()) {
            QString itemName = item->text(0);
            QTreeWidgetItem *parent = item->parent();
            int taskIdx = taskListWidget->indexOfTopLevelItem(parent);

            if (taskIdx >= 0 && taskIdx < gameState.tasks.size()) {
                Task &t = gameState.tasks[taskIdx];
                // 切换状态
                bool current = t.itemCompletionStatus.value(itemName, false);
                t.itemCompletionStatus[itemName] = !current;

                // 更新UI显示
                item->setText(2, !current ? "●" : "○");
                item->setForeground(2, !current ? QBrush(Qt::green) : QBrush(Qt::gray));
            }
        }
    });

    rightLayout->addWidget(taskListWidget, 5);

    // 原“查看申领表”按钮已移除

    // 3. 库存标题
    inventoryTitleLabel = new QLabel(Config::Test3::Texts::LBL_INVENTORY_TITLE);
    inventoryTitleLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT);
    rightLayout->addWidget(inventoryTitleLabel);

    // 4. 库存列表 - 占 50%
    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY);
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName)
    {
        handleInventoryDrop(itemName);
    };
    rightLayout->addWidget(inventoryListWidget, 5);

    // --- 电梯面板容器 (Sidebar) ---
    elevatorPanelContainer = new QWidget();
    QVBoxLayout *elePanelLayout = new QVBoxLayout(elevatorPanelContainer);
    elePanelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *eleTitle = new QLabel(Config::Test3::Texts::LBL_ELEVATOR_PANEL_TITLE);
    eleTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT);
    elePanelLayout->addWidget(eleTitle);

    QGridLayout *btnGrid = new QGridLayout();
    btnGrid->setSpacing(Config::Test3::Geometry::GRID_SPACING_ELEVATOR);

    auto createEleBtn = [&](int floor)
    {
        QString txt = (floor == 0) ? "G层" : QString("%1楼").arg(floor);
        QPushButton *btn = new QPushButton(txt);
        btn->setFixedSize(Config::Test3::Geometry::SIZE_ELEVATOR_BTN_SIDEBAR);
        // 使用统一蓝色样式
        btn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_UNIFIED);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, [this, floor]()
                { handleElevatorButton(floor); });
        return btn;
    };

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
            renderStaffHallway();
        } });

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
    gameState.floorInventory.clear();

    // 重置提示标志
    gameState.hasShownTipEntrance = false;
    gameState.hasShownTipWarehouse = false;
    gameState.hasShownTipShelf = false;
    gameState.hasShownTipLinenRoom = false;

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
    QString locStr;
    switch (gameState.currentScene)
    {
    case GameScene::Entrance: locStr = "入口"; break;
    case GameScene::StaffHallway: locStr = "员工通道"; break;
    case GameScene::Office: locStr = "办公室"; break;
    case GameScene::Warehouse: locStr = "布草仓库(入口)"; break;
    case GameScene::WarehouseShelf: locStr = "布草仓库(货架)"; break;
    case GameScene::ElevatorHall: locStr = (gameState.currentFloor == 0) ? "电梯厅 (G)" : QString("%1楼 电梯厅").arg(gameState.currentFloor); break;
    case GameScene::ElevatorInside: locStr = "电梯内"; break;
    case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break;
    case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break;
    }
    locationLabel->setText(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + locStr);

    QString statusImg;
    int cleanCount = 0;
    for (auto v : gameState.inventory.cleanItems)
        cleanCount += v;

    if (gameState.inventory.dirtyItemsCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_DIRTY;
    } else if (cleanCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_CLEAN;
    } else {
        statusImg = Config::Test3::Images::UI_CART_EMPTY;
    }

    QPixmap pix = getPixmap(statusImg);
    if (pix.isNull()) {
        cartStatusLabel->setText(gameState.inventory.dirtyItemsCount > 0 ? Config::Test3::Texts::LBL_CART_DIRTY
                                                                         : (cleanCount > 0 ? Config::Test3::Texts::LBL_CART_HAS_ITEMS : Config::Test3::Texts::LBL_CART_EMPTY));
        cartStatusLabel->setStyleSheet("border: 1px solid white; color: white;");
    } else {
        cartStatusLabel->setPixmap(pix);
        cartStatusLabel->setText("");
        cartStatusLabel->setStyleSheet("border: none;");
    }
}

void Test3::refreshInventoryList()
{
    inventoryListWidget->clear();
    for (auto it = gameState.inventory.cleanItems.begin(); it != gameState.inventory.cleanItems.end(); ++it)
    {
        if (it.value() > 0)
        {
            QListWidgetItem *item = new QListWidgetItem();
            QString classifier = Config::Test3::Logic::CLASSIFIERS().value(it.key(), "个");
            item->setText(QString("%1%2 %3").arg(it.value()).arg(classifier).arg(it.key()));
            item->setData(Qt::UserRole, it.key());

            QString iconPath = Config::Test3::Images::ITEMS().value(it.key());
            if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
            inventoryListWidget->addItem(item);
        }
    }

    if (gameState.inventory.dirtyItemsCount > 0)
    {
        QListWidgetItem *item = new QListWidgetItem();
        QString classifier = Config::Test3::Logic::CLASSIFIERS().value("脏布草", "件");
        item->setText(QString("%1%2 脏布草").arg(gameState.inventory.dirtyItemsCount).arg(classifier));
        QString iconPath = Config::Test3::Images::UI_DIRTY_LINEN;
        if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
        item->setData(Qt::UserRole, "DirtyLinen");
        inventoryListWidget->addItem(item);
    }
    updateRPGStatusLabels();
}

// 刷新任务列表 (三列布局: 物品 | 需求 | 完成)
void Test3::refreshTaskList()
{
    taskListWidget->clear();

    for (int i = 0; i < gameState.tasks.size(); ++i)
    {
        Task &t = gameState.tasks[i]; // 引用修改

        // 顶级项 (任务头)
        QString status = Config::Test3::Texts::STATUS_IN_PROGRESS;
        if (t.isMarkedComplete) {
            status = Config::Test3::Texts::STATUS_MARKED_COMPLETE;
        }

        QString headerText = QString("任务%1: %2楼 %3 %4")
                                 .arg(i + 1)
                                 .arg(t.targetFloor)
                                 .arg(t.isEmergency ? "[紧急]" : "")
                                 .arg(status);

        QTreeWidgetItem *topItem = new QTreeWidgetItem(taskListWidget);
        topItem->setText(0, headerText);
        // 合并单元格效果可以通过不设置1,2列实现，或者留空

        // 子项 (需求物品)
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
        {
            if (it.value() > 0)
            {
                QString itemName = it.key();
                int qty = it.value();

                QTreeWidgetItem *subItem = new QTreeWidgetItem(topItem);

                // 列0: 物品名
                subItem->setText(0, itemName);

                // 列1: 需求量
                subItem->setText(1, QString::number(qty));

                // 列2: 完成状态 (圆圈)
                bool isChecked = t.itemCompletionStatus.value(itemName, false);
                subItem->setText(2, isChecked ? "●" : "○");
                subItem->setForeground(2, isChecked ? QBrush(Qt::green) : QBrush(Qt::gray));
                subItem->setTextAlignment(2, Qt::AlignCenter);

                // 存储物品名以便点击时识别
                // (不需要SetData，直接取Text(0)即可)
            }
        }
    }
    taskListWidget->expandAll();
}

// --- 场景渲染 (Scene Rendering) ---

void Test3::renderScene()
{
    emit logMessage(QString("renderScene: %1").arg((int)gameState.currentScene));
    QList<QObject *> children = rpgCenterPanel->children();
    for (QObject *child : children)
    {
        if (child == hoverHintLabel)
            continue;
        if (child->isWidgetType())
            static_cast<QWidget *>(child)->hide();
        child->deleteLater();
    }

    QVBoxLayout *rightLayout = qobject_cast<QVBoxLayout *>(inventoryListWidget->parentWidget()->layout());
    if (gameState.currentScene == GameScene::ElevatorInside)
    {
        inventoryListWidget->hide();
        inventoryTitleLabel->hide();
        elevatorPanelContainer->show();

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

        if (rightLayout)
        {
            rightLayout->setStretchFactor(taskListWidget, 5);
            rightLayout->setStretchFactor(inventoryListWidget, 5);
        }
    }

    // 渲染背景和物体
    switch (gameState.currentScene)
    {
    case GameScene::Entrance: renderEntrance(); tryShowTip(GameScene::Entrance); break;
    case GameScene::StaffHallway: renderStaffHallway(); break;
    case GameScene::Office: renderOffice(); break;
    case GameScene::Warehouse: renderWarehouse(); tryShowTip(GameScene::Warehouse); break;
    case GameScene::WarehouseShelf: renderWarehouseShelf(); tryShowTip(GameScene::WarehouseShelf); break;
    case GameScene::ElevatorHall: renderElevatorHall(); break;
    case GameScene::ElevatorInside: renderElevatorInside(); break;
    case GameScene::FloorCorridor: renderFloorCorridor(); break;
    case GameScene::LinenRoom: renderLinenRoom(); tryShowTip(GameScene::LinenRoom); break;
    }
    updateRPGStatusLabels();
    installDevFilter(rpgCenterPanel);
    if (hoverHintLabel)
        hoverHintLabel->raise();
}

void Test3::tryShowTip(GameScene scene)
{
    QString tipText;
    bool *flag = nullptr;

    switch (scene) {
        case GameScene::Entrance:
            if (!gameState.hasShownTipEntrance) {
                tipText = Config::Test3::Texts::TUTORIAL_GENERAL;
                flag = &gameState.hasShownTipEntrance;
            }
            break;
        case GameScene::Warehouse:
            if (!gameState.hasShownTipWarehouse) {
                tipText = Config::Test3::Texts::TUTORIAL_WAREHOUSE_ENTRY;
                flag = &gameState.hasShownTipWarehouse;
            }
            break;
        case GameScene::WarehouseShelf:
            if (!gameState.hasShownTipShelf) {
                tipText = Config::Test3::Texts::TUTORIAL_WAREHOUSE_SHELF_ACTION;
                flag = &gameState.hasShownTipShelf;
            }
            break;
        case GameScene::LinenRoom:
             if (!gameState.hasShownTipLinenRoom) {
                tipText = Config::Test3::Texts::TUTORIAL_SHELF;
                flag = &gameState.hasShownTipLinenRoom;
            }
            break;
        default: break;
    }

    if (flag && *flag == false) {
        // 显示气泡
        SpeechBubble *bubble = new SpeechBubble(tipText, rpgCenterPanel);
        bubble->show();
        *flag = true;
    }
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
    bool anyTaskDone = false;
    for (const auto &t : gameState.tasks) if (t.isCompleted) anyTaskDone = true;
    if (anyTaskDone) bgPath = Config::Test3::Images::SCENE_HALLWAY_CLOCKED_OUT;
    else if (isLate) bgPath = Config::Test3::Images::SCENE_HALLWAY_LATE;

    QPixmap pix = getPixmap(bgPath);
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    ArrowButton *exitBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_HALLWAY_EXIT);
    exitBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_HALLWAY_EXIT);
    exitBtn->setArrowText("");
    exitBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    exitBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(exitBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Entrance); });
    connect(exitBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_ENTRANCE); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    exitBtn->show();

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

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_OFFICE, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_OFFICE, Config::Test3::Texts::BTN_GO_OFFICE, [this](){ goToScene(GameScene::Office); });
    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_WAREHOUSE, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_WAREHOUSE, Config::Test3::Texts::BTN_GO_WAREHOUSE, [this](){ goToScene(GameScene::Warehouse); });
    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_ELEVATOR, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_ELEVATOR, Config::Test3::Texts::BTN_GO_ELEVATOR, [this](){ goToScene(GameScene::ElevatorHall); });
}

void Test3::renderWarehouse()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_WAREHOUSE_ENTRY);
    if (pix.isNull()) pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 脏布草回收区域 (Polygon)
    ClickableArea *binArea = new ClickableArea(rpgCenterPanel);
    binArea->setPolygon(Config::Test3::Geometry::POLY_WAREHOUSE_BIN());
    binArea->setToolTip(Config::Test3::Texts::LBL_DIRTY_BIN_TIP);
    binArea->setGeometry(rpgCenterPanel->rect());

    binArea->onDropCallback = [this](QString item) {
        if (item == "DirtyLinen" || item == "脏布草") {
            gameState.inventory.dirtyItemsCount = 0;
            emit logMessage("脏布草已回收");
            refreshInventoryList();
            QMessageBox::information(this, "提示", "脏布草已回收。");
        } else {
            // 回收干净布草
            int count = gameState.inventory.cleanItems[item];
            if (count > 0) {
                 gameState.inventory.cleanItems[item] = 0;
                 emit logMessage("回收(丢弃): " + item);
                 refreshInventoryList();
                 QMessageBox::information(this, "提示", QString("已将 %1 放入回收处理。").arg(item));
            }
        }
    };
    connect(binArea, &ClickableArea::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    binArea->show();

    QPushButton *takeBtn = new QPushButton(Config::Test3::Texts::BTN_TAKE_LINEN, rpgCenterPanel);
    setGeometryCentered(takeBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_TAKE);
    // 使用统一蓝色样式 (原圆环样式移除，用户要求统一)
    takeBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_UNIFIED);
    takeBtn->setCursor(Qt::PointingHandCursor);
    connect(takeBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::WarehouseShelf); });
    takeBtn->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_WAREHOUSE_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
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
    if (pix.isNull()) pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    auto createShelfArea = [&](const QString &name, const QRect &rect, const QPoint &lblPos)
    {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true);
        area->setSourceType("WarehouseShelf");
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

        // 标签显示
        QLabel *lbl = new QLabel(name, rpgCenterPanel);
        lbl->setStyleSheet("color: white; font-weight: bold; font-size: 16px; background-color: rgba(0,0,0,0.5); padding: 2px; border-radius: 4px;");
        lbl->adjustSize();
        // 放置位置使用配置的坐标 (居中显示)
        int lx = lblPos.x() - lbl->width() / 2;
        int ly = lblPos.y() - lbl->height() / 2;
        lbl->move(lx, ly);
        lbl->show();

        area->onDropCallback = [this, name](QString item)
        {
            if (item != name)
            {
                QMessageBox::critical(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name));
                return;
            }
            handleSceneDrop(item, true);
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_SHEET, Config::Test3::Geometry::POS_LBL_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_DUVET, Config::Test3::Geometry::POS_LBL_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_S_DUVET, Config::Test3::Geometry::POS_LBL_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_PILLOW, Config::Test3::Geometry::POS_LBL_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_GN_TOWEL, Config::Test3::Geometry::POS_LBL_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_TOWEL, Config::Test3::Geometry::POS_LBL_TOWEL);

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_SHELF_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_SHELF_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::Warehouse); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::handleInventoryDrop(QString itemName, const QMimeData *mimeData)
{
    // 推车限制检查
    int totalItems = 0;
    for (auto c : gameState.inventory.cleanItems) totalItems += c;
    totalItems += gameState.inventory.dirtyItemsCount;

    if (itemName == "脏布草")
    {
        if (totalItems + 1 > Config::Test3::Logic::MAX_CART_ITEMS) {
            QMessageBox::warning(this, "推车已满", "推车已满，无法再装入物品！(上限40件)");
            return;
        }

        // 混装检查: 如果车里有干净布草，则记录错误
        int cleanCount = 0;
        for (auto c : gameState.inventory.cleanItems) cleanCount += c;
        if (cleanCount > 0) errorLog.mixedLinen = true;

        gameState.inventory.dirtyItemsCount++;
        gameState.dirtyBagState[gameState.currentFloor] = false;
        emit logMessage("回收脏布草");
        renderScene();
    }
    else
    {
        if (totalItems + 1 > Config::Test3::Logic::MAX_CART_ITEMS) {
            QMessageBox::warning(this, "推车已满", "推车已满，无法再装入物品！(上限40件)");
            return;
        }

        // 混装检查: 脏布草在车上时装干净布草
        if (gameState.inventory.dirtyItemsCount > 0) errorLog.mixedLinen = true;

        if (gameState.currentScene == GameScene::LinenRoom)
        {
            if (gameState.floorInventory[gameState.currentFloor].value(itemName, 0) > 0)
            {
                gameState.floorInventory[gameState.currentFloor][itemName]--;
                emit logMessage("从布草间货架取回: " + itemName);
            }
        }

        gameState.inventory.cleanItems[itemName]++;
        emit logMessage("装车: " + itemName);

        if (gameState.currentScene == GameScene::LinenRoom) renderScene();
    }
    refreshInventoryList();
}

void Test3::handleReportWork()
{
    QString msg = Config::Test3::Texts::REPORT_SUCCESS;
    QStringList errors;

    bool incomplete = false;
    for (const auto &t : gameState.tasks)
    {
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
        {
            incomplete = true;
            if (t.isMarkedComplete) {
                errors << QString(Config::Test3::Texts::REPORT_ERR_MARKED_INCOMPLETE).arg(t.targetFloor);
            }
        }
    }

    if (incomplete)
    {
        bool hasSpecificError = false;
        for(const QString &err : errors) {
            if(err.contains("标记完成但未满足")) hasSpecificError = true;
        }
        if(!hasSpecificError) {
             errors << Config::Test3::Texts::REPORT_ERR_MISSING_TASK;
        }
    }

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

void Test3::handleGetTask()
{
    if (gameState.hasReceivedTask) {
        QMessageBox::information(this, "提示", "您已经领取过任务了，请查看右侧任务列表。");
        return;
    }

    gameState.hasReceivedTask = true;

    // 分配固定任务 (6楼 和 7楼)
    Task t1; t1.targetFloor = 6; t1.requiredItems = {{"大床单", 2}, {"枕巾", 2}};
    t1.isEmergency = false;
    gameState.tasks.append(t1);

    Task t2; t2.targetFloor = 7; t2.requiredItems = {{"大被套", 1}, {"毛巾", 2}};
    t2.isEmergency = false;
    gameState.tasks.append(t2);

    refreshTaskList();
    emit logMessage("任务已下发: 6楼, 7楼");
    QMessageBox::information(this, "提示", "任务已下发，请查看右侧任务列表。");
    renderScene();
}

void Test3::checkEmergencyTask()
{
    // 随机事件逻辑
}

int Test3::getNormalRandom(int min, int max)
{
    return QRandomGenerator::global()->bounded(min, max + 1);
}

// --- 辅助函数 ---

QPixmap Test3::generatePlaceholder(QString text, QColor color, QSize size)
{
    QPixmap pix(size);
    pix.fill(color);
    QPainter p(&pix);
    p.setPen(Qt::white);
    p.drawText(pix.rect(), Qt::AlignCenter, text);
    return pix;
}

void Test3::showTutorial()
{
    gameState.hasShownTipEntrance = false;
    tryShowTip(GameScene::Entrance);
}

void Test3::handleClockIn()
{
    if (gameState.hasClockedIn) return;
    gameState.hasClockedIn = true;
    emit logMessage("上班打卡成功");
    QMessageBox::information(this, "提示", "上班打卡成功！请前往办公室领取任务。");
    renderScene();
}

void Test3::handleClockOut()
{
    gameState.hasClockedIn = false;
    emit logMessage("下班打卡");
    QMessageBox::information(this, "提示", "已打卡下班。");
    renderScene();
}

void Test3::handleGoHome()
{
    QStringList report;
    if (!gameState.hasReported) {
        errorLog.noReportBeforeHome = true;
        report << "错误: 下班前未汇报工作";
    }

    if (gameState.hasClockedIn) {
        errorLog.noClockOutBeforeHome = true;
        report << "错误: 下班前未打卡";
    }

    if (errorLog.hasErrors()) {
         QString msg = "本次实训存在以下问题:\n" + report.join("\n");
         QMessageBox::warning(this, "实训结束 (有瑕疵)", msg);
    } else {
         QMessageBox::information(this, "实训结束", "恭喜！流程规范，完美下班。");
    }
    emit levelCompleted();
}

void Test3::handleElevatorButton(int floor)
{
    if (floor == 1) return; // 1楼不停
    gameState.currentFloor = floor;
    emit logMessage(QString("电梯到达: %1层").arg(floor));
    goToScene(GameScene::ElevatorHall);
}

void Test3::handleSceneDrop(QString itemName, bool isWarehouse)
{
    if (gameState.inventory.cleanItems.value(itemName) <= 0) return;

    // 从推车移除
    gameState.inventory.cleanItems[itemName]--;

    if (isWarehouse) {
        emit logMessage("归还物品到仓库: " + itemName);
    } else {
        // 布草间
        gameState.floorInventory[gameState.currentFloor][itemName]++;
        emit logMessage(QString("放置物品到 %1楼 布草间: %2").arg(gameState.currentFloor).arg(itemName));
    }

    refreshInventoryList();
    renderScene();
}

void Test3::showTaskSheet(int taskIndex)
{
    // 此函数保留，但按钮入口已移除，防止逻辑报错。
    // 如果需要弹窗查看，可以在TaskList双击或其他方式触发。
    // 目前根据需求已移除申领表按钮。
}

void Test3::renderOffice()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_OFFICE);
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 动作按钮 (领任务 / 汇报)
    QPushButton *actionBtn = new QPushButton(gameState.hasReported ? Config::Test3::Texts::LBL_WORK_REPORTED :
                                            (gameState.hasReceivedTask ? Config::Test3::Texts::BTN_REPORT_WORK : Config::Test3::Texts::BTN_GET_TASK), rpgCenterPanel);
    setGeometryCentered(actionBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_ACTION);
    // 统一蓝色样式
    actionBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_UNIFIED);
    actionBtn->setCursor(Qt::PointingHandCursor);
    if (gameState.hasReported) actionBtn->setEnabled(false);

    connect(actionBtn, &QPushButton::clicked, [this]() {
        if (!gameState.hasReceivedTask) {
            handleGetTask();
        } else {
            handleReportWork();
        }
    });
    actionBtn->show();

    // Back Button
    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_OFFICE_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderElevatorHall()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_HALL);
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 进入电梯按钮
    QPushButton *enterBtn = new QPushButton(Config::Test3::Texts::BTN_ENTER_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(enterBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_ENTER);
    // 统一蓝色样式
    enterBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_UNIFIED);
    enterBtn->setCursor(Qt::PointingHandCursor);
    connect(enterBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::ElevatorInside); });
    enterBtn->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_ELEVATOR_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));

    if (gameState.currentFloor == 0) {
        connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
        connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
        });
    } else {
        connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::FloorCorridor); });
        connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_CORRIDOR); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
        });
    }
    backBtn->show();
}

void Test3::renderElevatorInside()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_INSIDE);
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 出电梯按钮
    QPushButton *exitBtn = new QPushButton(Config::Test3::Texts::BTN_EXIT_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_EXIT);
    // 统一蓝色样式
    exitBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_UNIFIED);
    exitBtn->setCursor(Qt::PointingHandCursor);
    connect(exitBtn, &QPushButton::clicked, [this](){
        goToScene(GameScene::ElevatorHall);
    });
    exitBtn->show();
}

void Test3::renderFloorCorridor()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_FLOOR_CORRIDOR);
    if (pix.isNull()) pix = generatePlaceholder("楼层走廊", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 去布草间
    ArrowButton *linenBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(linenBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_LINEN);
    linenBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_LINEN);
    linenBtn->setArrowText("");
    linenBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    linenBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(linenBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::LinenRoom); });
    connect(linenBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_LINEN_ROOM); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    linenBtn->show();

    // 去电梯厅
    ArrowButton *eleBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(eleBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_ELEVATOR);
    eleBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_ELEVATOR);
    eleBtn->setArrowText("");
    eleBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    eleBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(eleBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::ElevatorHall); });
    connect(eleBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_ELEVATOR_HALL); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    eleBtn->show();
}

void Test3::renderLinenRoom()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_LINEN_ROOM_EMPTY);
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    auto createLinenShelf = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true);
        area->setSourceType("LinenRoomShelf");

        int count = gameState.floorInventory[gameState.currentFloor].value(name, 0);

        if (count > 0) {
            QString iconPath = Config::Test3::Images::ITEMS().value(name);
            if (QFile::exists(iconPath)) {
                QPixmap p = getPixmap(iconPath);
                area->setPixmap(p.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                area->setAlignment(Qt::AlignCenter);
            }

            // Count Label
            QLabel *cntLbl = new QLabel(QString::number(count), rpgCenterPanel);
            cntLbl->setStyleSheet(QString("color: %1; font-size: %2px; font-weight: bold;")
                                  .arg(Config::Test3::Fonts::COL_LINEN_COUNT)
                                  .arg(Config::Test3::Fonts::SIZE_LINEN_COUNT));
            cntLbl->adjustSize();
            cntLbl->move(rect.right() - 20, rect.bottom() - 20);
            cntLbl->show();
        } else {
            // Empty
            area->setDraggable(false);
        }

        area->onDropCallback = [this, name](QString item) {
             if (item != name) {
                QMessageBox::critical(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name));
                return;
            }
            handleSceneDrop(item, false); // false = LinenRoom
        };

        connect(area, &ShelfArea::hovered, [this, name, count](bool status, QString text) {
             if(status) {
                 QString t = QString("%1 (当前: %2)").arg(name).arg(count);
                 hoverHintLabel->setText(t); hoverHintLabel->show(); hoverHintLabel->raise();
             } else hoverHintLabel->hide();
        });
        area->show();
    };

    createLinenShelf("大床单", Config::Test3::Geometry::AREA_LINEN_SHEET);
    createLinenShelf("大被套", Config::Test3::Geometry::AREA_LINEN_DUVET);
    createLinenShelf("小被套", Config::Test3::Geometry::AREA_LINEN_S_DUVET);
    createLinenShelf("枕巾", Config::Test3::Geometry::AREA_LINEN_PILLOW);
    createLinenShelf("晚安巾", Config::Test3::Geometry::AREA_LINEN_GN_TOWEL);
    createLinenShelf("毛巾", Config::Test3::Geometry::AREA_LINEN_TOWEL);

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_LINEN_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_LINEN_BACK);
    backBtn->setArrowText("");
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::FloorCorridor); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_CORRIDOR); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    backBtn->show();
}
