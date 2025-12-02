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

    // --- 左侧面板 (Updated Size) ---
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

    // 新手教程按钮 (Kept as backup or additional help)
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

    // --- 右侧面板 (Updated Size) ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_RIGHT_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 1. 任务标题
    QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT); // Use dark text for light bg
    rightLayout->addWidget(taskTitle);

    // 2. 任务列表 (TreeWidget) - 占 40%
    taskListWidget = new QTreeWidget();
    taskListWidget->setHeaderHidden(true);
    taskListWidget->setStyleSheet(Config::Test3::Styles::LIST_WIDGET);
    taskListWidget->setRootIsDecorated(true);
    // 鼠标悬停逻辑: 无需额外处理，TreeWidget自带滚动条，宽度已扩大
    rightLayout->addWidget(taskListWidget, 4);

    // 查看申领表按钮
    QPushButton *viewTaskSheetBtn = new QPushButton(Config::Test3::Texts::BTN_VIEW_TASK_SHEET);
    viewTaskSheetBtn->setStyleSheet(QString("%1 %2").arg(Config::Test3::Styles::BTN_VIEW_TASK_SHEET, Config::Test3::Styles::BTN_ORANGE));
    viewTaskSheetBtn->setCursor(Qt::PointingHandCursor);
    connect(viewTaskSheetBtn, &QPushButton::clicked, [this]()
            {
        // 获取选中的任务
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
    inventoryTitleLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT);
    rightLayout->addWidget(inventoryTitleLabel);

    // 4. 库存列表 - 占 60%
    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY);
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName)
    {
        handleInventoryDrop(itemName);
    };
    rightLayout->addWidget(inventoryListWidget, 6);

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
        btn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_FLOOR);
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

    // Reset Tip Flags
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

// Updated refreshTaskList with CheckBoxes
void Test3::refreshTaskList()
{
    taskListWidget->clear();

    for (int i = 0; i < gameState.tasks.size(); ++i)
    {
        Task &t = gameState.tasks[i]; // Reference to modify if needed

        // Top Level Item (Task Header)
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

        // Child Items (Requirements with Checkbox)
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it)
        {
            if (it.value() > 0)
            {
                QString itemName = it.key();
                QString classifier = Config::Test3::Logic::CLASSIFIERS().value(itemName, "个");
                QString subText = QString("%1: %2%3").arg(itemName).arg(it.value()).arg(classifier);

                QTreeWidgetItem *subItem = new QTreeWidgetItem(topItem);
                taskListWidget->setItemWidget(subItem, 0, nullptr); // Clear previous

                // Create a container widget for Text + Checkbox
                QWidget *widget = new QWidget();
                QHBoxLayout *hLayout = new QHBoxLayout(widget);
                hLayout->setContentsMargins(5, 2, 5, 2);

                QLabel *lbl = new QLabel(subText);
                lbl->setStyleSheet("color: black; font-size: 14px;");

                QCheckBox *chk = new QCheckBox();
                // Custom style for round checkbox
                chk->setStyleSheet(
                    "QCheckBox::indicator { width: 20px; height: 20px; border: 2px solid #999; border-radius: 11px; background: white; }"
                    "QCheckBox::indicator:checked { background: transparent; border: 2px solid #27ae60; image: none; }"
                    // Since I can't easily draw a tick with image, I'll use text or background color.
                    // Actually, simpler approach: standard checkbox styled round.
                    // Or let's use a Toggle Button that looks like a circle.
                );
                // Let's stick to standard checkbox but styled round.
                chk->setStyleSheet(
                    "QCheckBox::indicator { width: 18px; height: 18px; border-radius: 10px; border: 2px solid #555; }"
                    "QCheckBox::indicator:checked { background-color: #27ae60; border-color: #27ae60; image: url(:/non_existent); }" // Green fill
                );

                // Initialize state
                bool isChecked = t.itemCompletionStatus.value(itemName, false);
                chk->setChecked(isChecked);

                // Handle click
                connect(chk, &QCheckBox::clicked, [this, &t, itemName](bool checked){
                    t.itemCompletionStatus[itemName] = checked;
                    // No need to full refresh, just update model state
                });

                hLayout->addWidget(lbl);
                hLayout->addStretch();
                hLayout->addWidget(chk);

                taskListWidget->setItemWidget(subItem, 0, widget);
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
            rightLayout->setStretchFactor(taskListWidget, 4);
            rightLayout->setStretchFactor(inventoryListWidget, 6);
        }
    }

    // Render Scene Backgrounds & Objects
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
                tipText = Config::Test3::Texts::TUTORIAL_SHELF; // Contains the 1,2,3 steps
                flag = &gameState.hasShownTipLinenRoom;
            }
            break;
        default: break;
    }

    if (flag && *flag == false) {
        // Show Bubble
        SpeechBubble *bubble = new SpeechBubble(tipText, rpgCenterPanel);
        bubble->show();
        *flag = true; // Mark as shown
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

    // Replaced Rectangular DropLabel with ClickableArea (Polygon) that supports Drop
    ClickableArea *binArea = new ClickableArea(rpgCenterPanel);
    binArea->setPolygon(Config::Test3::Geometry::POLY_WAREHOUSE_BIN());
    binArea->setToolTip(Config::Test3::Texts::LBL_DIRTY_BIN_TIP);
    binArea->setGeometry(rpgCenterPanel->rect()); // Cover full area, masked by polygon

    // Callback handles both "DirtyLinen" and clean linen (Recycle logic)
    binArea->onDropCallback = [this](QString item) {
        if (item == "DirtyLinen" || item == "脏布草") {
            gameState.inventory.dirtyItemsCount = 0;
            emit logMessage("脏布草已回收");
            refreshInventoryList();
            QMessageBox::information(this, "提示", "脏布草已回收。");
        } else {
            // "I can also drag clean linen to it" -> Recycle clean linen (remove from cart)
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
    takeBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_WAREHOUSE_TAKE);
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

    auto createShelfArea = [&](const QString &name, const QRect &rect)
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

        // Add Label Underneath
        QLabel *lbl = new QLabel(name, rpgCenterPanel);
        lbl->setStyleSheet("color: white; font-weight: bold; font-size: 16px; background-color: rgba(0,0,0,0.5); padding: 2px; border-radius: 4px;");
        lbl->adjustSize();
        // Position below the rect
        int lx = rect.center().x() - lbl->width() / 2;
        int ly = rect.bottom() + 5;
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
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::Warehouse); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::handleInventoryDrop(QString itemName, const QMimeData *mimeData)
{
    // Cart limit check (Total count)
    int totalItems = 0;
    for (auto c : gameState.inventory.cleanItems) totalItems += c;
    totalItems += gameState.inventory.dirtyItemsCount;

    // Only check limit when ADDING to cart.
    // However, if we are just moving things around or dropping "DirtyLinen" which might replace something?
    // Actually, dragging from shelf adds. Dragging from floor adds.
    // If dropping dirty linen, it adds 1.
    // We check if (totalItems + 1 > 40).

    // Note: If dropping "DirtyLinen" into "DirtyLinen" slot (handleInventoryDrop logic handles generic add), we check.
    // Logic:

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
    // Removed the "Mark as Complete" blocker logic.
    // "汇报工作那里去掉必须全部标记才能汇报的限制"

    // Original blocker:
    /*
    for (const auto &t : gameState.tasks) {
        if (!t.isMarkedComplete) {
            QMessageBox::warning... return;
        }
    }
    */
    // Now just proceed to validation.

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
