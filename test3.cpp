#include "test3.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMap>
#include <QFile>
#include <cmath>
#include <QtMath>
#include <QMessageBox>
#include "config.h"
#include "utils.h"

Test3::Test3(bool isDevMode, QWidget *parent) : QWidget(parent), isDeveloperMode(isDevMode) {
    // Main Layout (Grid) to allow top-right positioning
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setContentsMargins(0,0,0,0);
    mainGrid->setSpacing(0);

    QWidget *rpgContainer = new QWidget();
    QHBoxLayout *rpgLayout = new QHBoxLayout(rpgContainer);
    rpgLayout->setContentsMargins(0,0,0,0);
    rpgLayout->setSpacing(0);

    // --- Left Panel ---
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    leftPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_LEFT);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    locationLabel = new QLabel(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + "入口");
    locationLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    cartStatusLabel = new QLabel(); // Image label
    cartStatusLabel->setFixedSize(Config::Test3::Geometry::ICON_CART);
    cartStatusLabel->setScaledContents(true);

    leftLayout->addWidget(locationLabel);
    leftLayout->addStretch();
    leftLayout->addWidget(cartStatusLabel, 0, Qt::AlignBottom | Qt::AlignHCenter);
    leftLayout->addSpacing(20);
    rpgLayout->addWidget(leftPanel);

    // --- Center Panel ---
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(Config::Test3::Geometry::CENTER_PANEL_SIZE);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    // We install event filter later recursively, but also on the panel itself
    rpgCenterPanel->installEventFilter(this);
    rpgLayout->addWidget(rpgCenterPanel);

    // Hover Hint Label
    hoverHintLabel = new QLabel(rpgCenterPanel);
    hoverHintLabel->setAlignment(Qt::AlignCenter);
    // Use new config style
    hoverHintLabel->setStyleSheet(Config::Test3::Styles::LBL_HOVER_HINT);
    hoverHintLabel->hide();
    // Use new config geometry
    hoverHintLabel->setGeometry(Config::Test3::Geometry::RECT_HOVER_HINT);

    // --- Right Panel ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QPushButton *returnBtn = new QPushButton(Config::Test3::Texts::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test3::Geometry::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test3::Styles::BTN_RETURN_MENU);
    returnBtn->setCursor(Qt::PointingHandCursor);
    connect(returnBtn, &QPushButton::clicked, [this]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认退出", "确定要退出当前实训并返回主菜单吗？\n当前进度将不会保留。",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
             reset(); // Reset state on exit
             emit levelCancelled();
        }
    });
    rightLayout->addWidget(returnBtn);

    QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(taskTitle);

    taskListWidget = new QListWidget();
    taskListWidget->setStyleSheet(Config::Test3::Styles::LIST_WIDGET);
    taskListWidget->setFixedHeight(Config::Test3::Geometry::TASK_LIST_HEIGHT);
    rightLayout->addWidget(taskListWidget);

    QPushButton *viewTaskSheetBtn = new QPushButton(Config::Test3::Texts::BTN_VIEW_TASK_SHEET);
    // Combine base style with color
    viewTaskSheetBtn->setStyleSheet(QString("%1 %2").arg(Config::Test3::Styles::BTN_VIEW_TASK_SHEET, Config::Test3::Styles::BTN_ORANGE));
    viewTaskSheetBtn->setCursor(Qt::PointingHandCursor);
    connect(viewTaskSheetBtn, &QPushButton::clicked, [this](){
        // Get selected index
        int idx = taskListWidget->currentRow();
        if (idx < 0 && taskListWidget->count() > 0) idx = 0;
        showTaskSheet(idx);
    });
    rightLayout->addWidget(viewTaskSheetBtn);

    // --- Inventory Title & List (Toggleable) ---
    inventoryTitleLabel = new QLabel(Config::Test3::Texts::LBL_INVENTORY_TITLE);
    inventoryTitleLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(inventoryTitleLabel);

    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY); // Large icons
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName) {
        handleInventoryDrop(itemName);
    };
    rightLayout->addWidget(inventoryListWidget);

    // --- Elevator Panel (Sidebar, initially hidden) ---
    elevatorPanelContainer = new QWidget();
    QVBoxLayout *elePanelLayout = new QVBoxLayout(elevatorPanelContainer);
    elePanelLayout->setContentsMargins(0,0,0,0);

    QLabel *eleTitle = new QLabel(Config::Test3::Texts::LBL_ELEVATOR_PANEL_TITLE);
    eleTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    elePanelLayout->addWidget(eleTitle);

    // Grid of buttons
    QGridLayout *btnGrid = new QGridLayout();
    btnGrid->setSpacing(Config::Test3::Geometry::GRID_SPACING_ELEVATOR);

    // Create G, 2-10 buttons (No 1F)
    auto createEleBtn = [&](int floor) {
        QString txt = (floor == 0) ? "G层" : QString("%1楼").arg(floor);
        QPushButton *btn = new QPushButton(txt);
        btn->setFixedSize(Config::Test3::Geometry::SIZE_ELEVATOR_BTN_SIDEBAR);
        btn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_FLOOR);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, [this, floor](){ handleElevatorButton(floor); });
        return btn;
    };

    // Explicit placement
    btnGrid->addWidget(createEleBtn(9), 0, 0);
    btnGrid->addWidget(createEleBtn(10), 0, 1);
    btnGrid->addWidget(createEleBtn(7), 1, 0);
    btnGrid->addWidget(createEleBtn(8), 1, 1);
    btnGrid->addWidget(createEleBtn(5), 2, 0);
    btnGrid->addWidget(createEleBtn(6), 2, 1);
    btnGrid->addWidget(createEleBtn(3), 3, 0);
    btnGrid->addWidget(createEleBtn(4), 3, 1);
    btnGrid->addWidget(createEleBtn(0), 4, 0); // G (Logic 0)
    btnGrid->addWidget(createEleBtn(2), 4, 1);

    elePanelLayout->addLayout(btnGrid);
    elePanelLayout->addStretch();

    rightLayout->addWidget(elevatorPanelContainer);
    elevatorPanelContainer->hide(); // Start hidden

    rpgLayout->addWidget(rightPanel);

    // Add RPG container to main layout
    mainGrid->addWidget(rpgContainer, 0, 0);

    // Timers
    latenessTimer = new QTimer(this);
    latenessTimer->setSingleShot(true);
    latenessTimer->setInterval(Config::Test3::Logic::TIME_LATE_THRESHOLD_SEC * 1000);
    connect(latenessTimer, &QTimer::timeout, [this]() {
        isLate = true;
        emit logMessage("计时结束: 标记为迟到状态");
        // If we are currently in Hallway, update the background immediately regardless of clock in status
        // (Unless tasks are done, which is handled in renderStaffHallway priority)
        if (gameState.currentScene == GameScene::StaffHallway) {
            renderStaffHallway();
        }
    });

    // Debug Heartbeat
    heartbeatTimer = new QTimer(this);
    heartbeatTimer->setInterval(1000);
    connect(heartbeatTimer, &QTimer::timeout, [this]() {
        static int count = 0;
        emit logMessage(QString("Test3 Heartbeat: %1s (Main Thread Active)").arg(++count));
    });

    // Init State
    reset();
}

void Test3::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
}

void Test3::reset() {
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

    // Reset Logic Flags
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

void Test3::installDevFilter(QWidget *widget) {
    if (!isDeveloperMode) return;
    widget->installEventFilter(this);
    // Recursively install on children
    const QObjectList &children = widget->children();
    for (QObject *child : children) {
        if (child->isWidgetType()) {
            installDevFilter(static_cast<QWidget*>(child));
        }
    }
}

bool Test3::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress && isDeveloperMode) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        // Map to Center Panel Coordinates
        QPoint globalPos = static_cast<QWidget*>(watched)->mapToGlobal(mouseEvent->pos());
        QPoint localPos = rpgCenterPanel->mapFromGlobal(globalPos);

        // Log coord
        QString coordText = QString("DevMode Click: (%1, %2)").arg(localPos.x()).arg(localPos.y());
        qDebug() << coordText;
        emit logMessage(coordText);
        // Note: Do NOT return true, allow event to propagate to the button
    }
    return QWidget::eventFilter(watched, event);
}

// --- Logic ---

void Test3::goToScene(GameScene scene) {
    emit logMessage(QString("goToScene: %1").arg((int)scene));
    // Logic for Lateness Timer
    // Condition: Switching scene FROM StaffHallway TO somewhere else (e.g. Office, Elevator, Warehouse)
    // AND Not Clocked In yet.
    if (gameState.currentScene == GameScene::StaffHallway && scene != GameScene::StaffHallway) {
        if (!gameState.hasClockedIn && !isTimerTriggered && !isLate) {
            isTimerTriggered = true;
            latenessTimer->start();
            emit logMessage("计时开始: 10秒迟到倒计时");
        }
    }

    gameState.currentScene = scene;
    emit logMessage("移动到场景: " + QString::number((int)scene));
    renderScene();
}

void Test3::updateRPGStatusLabels() {
    // Location
    QString locStr;
    switch(gameState.currentScene) {
        case GameScene::Entrance: locStr = "入口"; break;
        case GameScene::StaffHallway: locStr = "员工通道"; break;
        case GameScene::Office: locStr = "办公室"; break;
        case GameScene::Warehouse: locStr = "布草仓库(入口)"; break;
        case GameScene::WarehouseShelf: locStr = "布草仓库(货架)"; break;
        case GameScene::ElevatorHall:
            locStr = (gameState.currentFloor == 0) ? "电梯厅 (G)" : QString("%1楼 电梯厅").arg(gameState.currentFloor);
            break;
        case GameScene::ElevatorInside: locStr = "电梯内"; break;
        case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break;
        case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break;
    }
    locationLabel->setText(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + locStr);

    // Cart Status Image
    QString statusImg;
    int cleanCount = 0;
    for (auto v : gameState.inventory.cleanItems) cleanCount += v;

    if (gameState.inventory.dirtyItemsCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_DIRTY;
    } else if (cleanCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_CLEAN;
    } else {
        statusImg = Config::Test3::Images::UI_CART_EMPTY;
    }

    QPixmap pix(statusImg);
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

void Test3::refreshInventoryList() {
    inventoryListWidget->clear();
    // Show Clean Items
    for (auto it = gameState.inventory.cleanItems.begin(); it != gameState.inventory.cleanItems.end(); ++it) {
        if (it.value() > 0) {
            // Create item with image
            QListWidgetItem *item = new QListWidgetItem();
            item->setText(QString("%1 x%2").arg(it.key()).arg(it.value())); // Keep text for quantity
            item->setData(Qt::UserRole, it.key()); // Drag data

            // Load Icon from Config Map
            QString iconPath = Config::Test3::Images::ITEMS().value(it.key());
            if (QFile::exists(iconPath)) {
                item->setIcon(QIcon(iconPath));
            }

            inventoryListWidget->addItem(item);
        }
    }

    // Show Dirty Items
    if (gameState.inventory.dirtyItemsCount > 0) {
         QListWidgetItem *item = new QListWidgetItem("脏布草");
         QString iconPath = Config::Test3::Images::UI_DIRTY_LINEN;
         if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
         item->setData(Qt::UserRole, "DirtyLinen");
         inventoryListWidget->addItem(item);
    }
    updateRPGStatusLabels();
}

void Test3::refreshTaskList() {
    taskListWidget->clear();
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        const Task &t = gameState.tasks[i];
        QString status = t.isCompleted ? "[已完成]" : "[进行中]";
        QString txt = QString("任务%1: %2楼 %3 %4").arg(i+1).arg(t.targetFloor).arg(t.isEmergency ? "[紧急]" : "").arg(status);
        taskListWidget->addItem(txt);
    }
}

// --- Scene Rendering ---

void Test3::renderScene() {
    emit logMessage(QString("renderScene: %1").arg((int)gameState.currentScene));
    // Clear Center Panel
    QList<QObject*> children = rpgCenterPanel->children();
    for (QObject *child : children) {
        if (child == hoverHintLabel) continue; // Skip hover label
        if (child->isWidgetType()) static_cast<QWidget*>(child)->hide();
        child->deleteLater();
    }

    // Toggle Sidebar UI
    if (gameState.currentScene == GameScene::ElevatorInside) {
        inventoryListWidget->hide();
        inventoryTitleLabel->hide();
        elevatorPanelContainer->show();
    } else {
        elevatorPanelContainer->hide();
        inventoryTitleLabel->show();
        inventoryListWidget->show();
    }

    switch(gameState.currentScene) {
        case GameScene::Entrance: renderEntrance(); break;
        case GameScene::StaffHallway: renderStaffHallway(); break;
        case GameScene::Office: renderOffice(); break;
        case GameScene::Warehouse: renderWarehouse(); break;
        case GameScene::WarehouseShelf: renderWarehouseShelf(); break;
        case GameScene::ElevatorHall: renderElevatorHall(); break;
        case GameScene::ElevatorInside: renderElevatorInside(); break;
        case GameScene::FloorCorridor: renderFloorCorridor(); break;
        case GameScene::LinenRoom: renderLinenRoom(); break;
    }
    updateRPGStatusLabels();

    // Install Event Filter on ALL new children for Dev Mode
    installDevFilter(rpgCenterPanel);

    // Ensure hover hint label is on top
    if (hoverHintLabel) hoverHintLabel->raise();
}

void Test3::renderEntrance() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ENTRANCE);
    if (pix.isNull()) pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Go Home Button (Arrow)
    ArrowButton *btnHome = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(btnHome, Config::Test3::Geometry::RECT_BTN_ENTRANCE_HOME);
    btnHome->setAngle(Config::Test3::Geometry::ANGLE_BTN_ENTRANCE_HOME);
    btnHome->setArrowText(Config::Test3::Texts::TEXT_BTN_ENTRANCE_HOME);
    btnHome->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE); // New Config
    btnHome->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(btnHome, &QPushButton::clicked, this, &Test3::handleGoHome);
    connect(btnHome, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    btnHome->show();

    // Enter Hotel Area
    ClickableArea *btnEnter = new ClickableArea(rpgCenterPanel);
    btnEnter->setPolygon(Config::Test3::Geometry::POLY_ENTRANCE_ENTER());
    btnEnter->setToolTip(Config::Test3::Texts::BTN_ENTER_HOTEL);
    connect(btnEnter, &ClickableArea::clicked, [this]() {
        goToScene(GameScene::StaffHallway);
    });
    connect(btnEnter, &ClickableArea::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    btnEnter->setGeometry(rpgCenterPanel->rect());
    btnEnter->show();
}

void Test3::renderStaffHallway() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // Background Logic: Normal vs Late vs ClockedOut
    QString bgPath = Config::Test3::Images::SCENE_HALLWAY_NORMAL;

    // Check if any task is completed
    bool anyTaskDone = false;
    for(const auto &t : gameState.tasks) if(t.isCompleted) anyTaskDone = true;

    if (anyTaskDone) {
        bgPath = Config::Test3::Images::SCENE_HALLWAY_CLOCKED_OUT;
    } else if (isLate) {
        // BUGFIX: Prioritize Late background if late, regardless of clock in (unless tasks done)
        bgPath = Config::Test3::Images::SCENE_HALLWAY_LATE;
    }

    QPixmap pix(bgPath);
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Return to Entrance (Arrow)
    ArrowButton *exitBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_HALLWAY_EXIT);
    exitBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_HALLWAY_EXIT);
    exitBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_ENTRANCE);
    exitBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    exitBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(exitBtn, &QPushButton::clicked, [this]() {
        goToScene(GameScene::Entrance);
    });
    connect(exitBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    exitBtn->show();

    // Clock In/Out (Irregular Area)
    ClickableArea *btnClock = new ClickableArea(rpgCenterPanel);
    btnClock->setPolygon(Config::Test3::Geometry::POLY_HALLWAY_CLOCK());
    QString clockText = gameState.hasClockedIn ? Config::Test3::Texts::BTN_CLOCK_OUT : Config::Test3::Texts::BTN_CLOCK_IN;
    btnClock->setToolTip(clockText);
    connect(btnClock, &ClickableArea::clicked, [this]() {
        if (gameState.hasClockedIn) handleClockOut();
        else handleClockIn();
    });
    connect(btnClock, &ClickableArea::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    btnClock->setGeometry(rpgCenterPanel->rect());
    btnClock->show();

    // Navigation Arrows
    auto createArrow = [&](const QRect &rect, int angle, QString text, auto func) {
        ArrowButton *btn = new ArrowButton(rpgCenterPanel);
        setGeometryCentered(btn, rect);
        btn->setAngle(angle);
        btn->setArrowText(text);
        btn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
        btn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
        connect(btn, &QPushButton::clicked, func);
        connect(btn, &ArrowButton::hovered, [this](bool status, QString t) {
            if (status) { hoverHintLabel->setText(t); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide();
        });
        btn->show();
    };

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_OFFICE,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_OFFICE,
                Config::Test3::Texts::BTN_GO_OFFICE,
                [this]() { goToScene(GameScene::Office); });

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_WAREHOUSE,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_WAREHOUSE,
                Config::Test3::Texts::BTN_GO_WAREHOUSE,
                [this]() { goToScene(GameScene::Warehouse); });

    createArrow(Config::Test3::Geometry::RECT_BTN_HALLWAY_ELEVATOR,
                Config::Test3::Geometry::ANGLE_BTN_HALLWAY_ELEVATOR,
                Config::Test3::Texts::BTN_GO_ELEVATOR,
                [this]() { goToScene(GameScene::ElevatorHall); });
}

void Test3::renderOffice() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_OFFICE);
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Action Button (Report / Get Task) - Kept as regular button
    // Check state
    bool allTasksDone = !gameState.tasks.isEmpty();
    for(const auto &t : gameState.tasks) if (!t.isCompleted) allTasksDone = false;
    if (gameState.tasks.isEmpty()) allTasksDone = false;

    // Report is available if we have tasks and they are done, OR if we want to report errors
    // Simplified: Always show Report button if tasks are done, OR allow it?
    // User requirement: "Can report work... if missed emergency task... text corresponds."
    // So if tasks are generated, we can report.

    QPushButton *actionBtn = new QPushButton(rpgCenterPanel);
    setGeometryCentered(actionBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_ACTION);
    actionBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_OFFICE_ACTION);
    actionBtn->setCursor(Qt::PointingHandCursor);

    if (gameState.hasReceivedTask) {
        actionBtn->setText(Config::Test3::Texts::BTN_REPORT_WORK);
        connect(actionBtn, &QPushButton::clicked, this, &Test3::handleReportWork);
    } else {
        actionBtn->setText(Config::Test3::Texts::BTN_GET_TASK);
        connect(actionBtn, &QPushButton::clicked, this, &Test3::handleGetTask);
    }
    actionBtn->show();

    // Back Arrow
    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_OFFICE_BACK);
    backBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_HALLWAY);
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    backBtn->show();

    if (gameState.hasReported) {
         QLabel *lbl = new QLabel(Config::Test3::Texts::LBL_WORK_REPORTED, rpgCenterPanel);
         setGeometryCentered(lbl, Config::Test3::Geometry::RECT_LBL_OFFICE_MSG);
         lbl->setStyleSheet(Config::Test3::Styles::LBL_SUCCESS_GREEN);
         lbl->show();
    }
}

void Test3::renderWarehouse() {
    // 1. Show Entry view
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_WAREHOUSE_ENTRY);
    if (pix.isNull()) pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 2. Button "拿取布草" -> Go to Shelf View
    QPushButton *takeBtn = new QPushButton(Config::Test3::Texts::BTN_TAKE_LINEN, rpgCenterPanel);
    setGeometryCentered(takeBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_TAKE);
    takeBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_WAREHOUSE_TAKE);
    takeBtn->setCursor(Qt::PointingHandCursor);
    connect(takeBtn, &QPushButton::clicked, [this]() {
         goToScene(GameScene::WarehouseShelf);
    });
    takeBtn->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_WAREHOUSE_BACK);
    backBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_HALLWAY);
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    backBtn->show();
}

void Test3::renderWarehouseShelf() {
    // Shelf View
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_WAREHOUSE_SHELF);
    if (pix.isNull()) pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Helper lambda to create Shelf Area (Infinite Supply)
    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true); // Infinite supply in Warehouse
        area->setSourceType("WarehouseShelf"); // Tag source
        area->setToolTip(name + " (可拿取)");
        connect(area, &ShelfArea::hovered, [this](bool status, QString text) {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide();
        });

        // Add image to shelf area
        QString iconPath = Config::Test3::Images::ITEMS().value(name);
        if (QFile::exists(iconPath)) {
            QPixmap pix(iconPath);
            if (!pix.isNull()) {
                 area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                 area->setAlignment(Qt::AlignCenter);
            }
        }

        // Put back logic: If dropped here, remove from cart
        area->onDropCallback = [this, name](QString item) {
             if (item != name) {
                 QMessageBox::critical(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name));
                 return;
             }
             handleSceneDrop(item, true); // true = warehouse (put back)
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
    backBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY);
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    backBtn->show();
}

void Test3::renderElevatorHall() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ELEVATOR_HALL);
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *callElevator = new QPushButton(Config::Test3::Texts::BTN_ENTER_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(callElevator, Config::Test3::Geometry::RECT_BTN_ELEVATOR_ENTER);
    callElevator->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_ENTER);
    callElevator->setCursor(Qt::PointingHandCursor);
    connect(callElevator, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorInside); });
    callElevator->show();

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_ELEVATOR_BACK);
    backBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_BACK);
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]() {
        if (gameState.currentFloor == 0) goToScene(GameScene::StaffHallway);
        else goToScene(GameScene::FloorCorridor);
    });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    backBtn->show();
}

void Test3::renderElevatorInside() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ELEVATOR_INSIDE);
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Add "Exit Elevator" button
    QPushButton *exitBtn = new QPushButton(Config::Test3::Texts::BTN_EXIT_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_EXIT);
    exitBtn->setStyleSheet(Config::Test3::Styles::STYLE_BTN_ELEVATOR_EXIT);
    exitBtn->setCursor(Qt::PointingHandCursor);
    connect(exitBtn, &QPushButton::clicked, [this]() {
         goToScene(GameScene::ElevatorHall);
    });
    exitBtn->show();
}

void Test3::renderFloorCorridor() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_FLOOR_CORRIDOR);
    if (pix.isNull()) pix = generatePlaceholder("走廊", Qt::cyan, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    ArrowButton *linenRoomBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(linenRoomBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_LINEN);
    linenRoomBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_LINEN);
    linenRoomBtn->setArrowText(Config::Test3::Texts::BTN_GO_LINEN_ROOM);
    linenRoomBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    linenRoomBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(linenRoomBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::LinenRoom); });
    connect(linenRoomBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    linenRoomBtn->show();

    ArrowButton *elevatorBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(elevatorBtn, Config::Test3::Geometry::RECT_BTN_CORRIDOR_ELEVATOR);
    elevatorBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_ELEVATOR);
    elevatorBtn->setArrowText(Config::Test3::Texts::BTN_GO_ELEVATOR_HALL);
    elevatorBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    elevatorBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
    connect(elevatorBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    elevatorBtn->show();
}

void Test3::renderLinenRoom() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_LINEN_ROOM_EMPTY);
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width()/2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height()/2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Check Dirty Linen Status
    if (!gameState.dirtyBagState.contains(gameState.currentFloor)) {
         gameState.dirtyBagState[gameState.currentFloor] = false;
    }

    // Create Shelf Areas
    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setSourceType("LinenRoomShelf"); // Tag as Linen Room source
        area->setToolTip(name + " (拖拽取回/放置)");
        connect(area, &ShelfArea::hovered, [this](bool status, QString text) {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide();
        });

        // Check if item has been placed
        int placedCount = 0;
        // Find task for current floor
        for(const auto &t : gameState.tasks) {
            if(t.targetFloor == gameState.currentFloor && !t.isCompleted) {
                placedCount = t.placedItems.value(name, 0);
                break;
            }
        }

        // Initialize state based on placedCount
        if (placedCount > 0) {
            area->setDraggable(true);
            QString iconPath = Config::Test3::Images::ITEMS().value(name);
            if (QFile::exists(iconPath)) {
                QPixmap pix(iconPath);
                if (!pix.isNull()) {
                     area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                     area->setAlignment(Qt::AlignCenter);
                }
            }
            QLabel *countLbl = new QLabel(QString("x%1").arg(placedCount), area);
            countLbl->setStyleSheet(QString("color: %1; font-weight: bold; font-size: %2px; background: transparent; padding: 2px;")
                                    .arg(Config::Test3::Fonts::COL_LINEN_COUNT)
                                    .arg(Config::Test3::Fonts::SIZE_LINEN_COUNT));
            countLbl->adjustSize();
            countLbl->move((area->width() - countLbl->width())/2, (area->height() - countLbl->height())/2);
            countLbl->show();
        } else {
            area->setDraggable(false);
            area->clear(); // Ensure empty
        }

        area->onDropCallback = [this, name, area](QString item) {
             if (item != name) {
                 QMessageBox::critical(this, "错误", QString("放置失败：不能将 %1 放置在 %2 的位置！").arg(item, name));
                 return;
             }

             bool needed = false;
             for (const auto &t : gameState.tasks) {
                 if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
                      if (t.requiredItems.value(name, 0) > 0) {
                          needed = true;
                      }
                      break;
                 }
             }

             if (!needed) {
                 QMessageBox::warning(this, "提示", "本层不需要此物品，或已放满。");
                 return;
             }

             handleSceneDrop(item, false); // false = linen room (delivery)
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_LINEN_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_LINEN_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_LINEN_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_LINEN_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_LINEN_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_LINEN_TOWEL);

    // If Dirty Linen Present
    if (gameState.dirtyBagState[gameState.currentFloor]) {
        DragSourceLabel *dirty = new DragSourceLabel("脏布草", rpgCenterPanel);
        QPixmap dirtyPix(Config::Test3::Images::UI_DIRTY_LINEN);
        if (!dirtyPix.isNull()) dirty->setPixmap(dirtyPix.scaled(Config::Test3::Geometry::ICON_DIRTY_DRAG));
        setGeometryCentered(dirty, Config::Test3::Geometry::RECT_EVENT_DIRTY_LINEN);
        dirty->show();
    }

    ArrowButton *backBtn = new ArrowButton(rpgCenterPanel);
    setGeometryCentered(backBtn, Config::Test3::Geometry::RECT_BTN_LINEN_BACK);
    backBtn->setAngle(Config::Test3::Geometry::ANGLE_BTN_LINEN_BACK);
    backBtn->setArrowText(Config::Test3::Texts::BTN_RETURN_CORRIDOR);
    backBtn->setArrowTextSize(Config::Test3::Styles::ARROW_TEXT_SIZE);
    backBtn->setColor(QColor(Config::Test3::Styles::ARROW_TEXT_COLOR));
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::FloorCorridor); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    backBtn->show();
}

// --- Action Handlers ---

void Test3::handleInventoryDrop(QString itemName, const QMimeData *mimeData) {
    if (itemName == "脏布草") {
        int cleanCount = 0;
        for (auto c : gameState.inventory.cleanItems) cleanCount += c;

        if (cleanCount > 0) {
            QMessageBox::warning(this, "错误", "推车内有干净布草，不能装脏布草！");
            emit logMessage("错误操作：混装脏布草");
            return;
        }

        gameState.inventory.dirtyItemsCount++;
        gameState.dirtyBagState[gameState.currentFloor] = false;
        emit logMessage("回收脏布草");
        renderScene();
    }
    else {
        if (gameState.inventory.dirtyItemsCount > 0) {
             QMessageBox::warning(this, "错误", "推车内有脏布草，不能装干净布草！");
             emit logMessage("错误操作：混装干净布草");
             return;
        }

        // Logic Check: Source?
        // If we are in Linen Room, assume it comes from Shelf if it's not internal drag
        if (gameState.currentScene == GameScene::LinenRoom) {
             // We need to decrease the count on the shelf (placed items)
             // Find task for current floor
             bool foundTask = false;
             for (auto &t : gameState.tasks) {
                 if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
                     // Check if we have placed this item there
                     if (t.placedItems.value(itemName, 0) > 0) {
                         t.placedItems[itemName]--;
                         t.requiredItems[itemName]++; // Add back to needed? Or just reduce placed?
                         // Logic: "Placed" means it satisfied a requirement.
                         // If we take it back, the requirement is no longer satisfied.
                         // So yes, increase requiredItems (or just rely on the logic that checks placed vs needed)
                         // My struct has `requiredItems` as "Remaining Needed".
                         // So if I take back, `requiredItems` should increase.
                         t.requiredItems[itemName]++;

                         emit logMessage("从布草间货架取回: " + itemName);
                         foundTask = true;
                     }
                     break; // Only one active task per floor usually
                 }
             }
             if (!foundTask) {
                 // Might be just taking from shelf but no task?
                 // Or task completed?
                 // If task completed, can we take back?
                 // Assuming strictly active tasks for now based on logic in renderLinenRoom.
             }
        }

        gameState.inventory.cleanItems[itemName]++;
        emit logMessage("装车: " + itemName);

        // Refresh scene to show updated shelf counts
        if (gameState.currentScene == GameScene::LinenRoom) {
            renderScene();
        }
    }
    refreshInventoryList();
}

void Test3::handleSceneDrop(QString itemName, bool isWarehouse) {
    if (gameState.inventory.cleanItems[itemName] <= 0) return;

    if (isWarehouse) {
        gameState.inventory.cleanItems[itemName]--;
        emit logMessage("放回仓库: " + itemName);
        refreshInventoryList();
        return;
    }

    bool taskFound = false;
    bool needed = false;

    for (int i = 0; i < gameState.tasks.size(); ++i) {
        Task &t = gameState.tasks[i];
        if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
            taskFound = true;
            if (t.requiredItems.contains(itemName) && t.requiredItems[itemName] > 0) {
                needed = true;

                gameState.inventory.cleanItems[itemName]--;
                t.requiredItems[itemName]--;
                t.placedItems[itemName]++;
                emit logMessage("放置 " + itemName + " 到货架");

                bool allDone = true;
                for (auto count : t.requiredItems) if (count > 0) allDone = false;
                if (allDone) {
                    t.isCompleted = true;
                    emit logMessage("任务完成: " + QString::number(gameState.currentFloor) + "楼");

                    // Check Priority Error
                    if (isEmergencyActive) {
                        // Find the emergency task
                        bool emergencyDone = false;
                        for(const auto &et : gameState.tasks) if(et.isEmergency && et.isCompleted) emergencyDone = true;

                        if (!t.isEmergency && !emergencyDone) {
                             errorLog.missedEmergencyPriority = true;
                             emit logMessage("错误: 未优先完成紧急任务");
                        } else if (t.isEmergency) {
                             isEmergencyActive = false; // Cleared
                        }
                    }

                    bool allAllDone = true;
                    for(const auto &checkT : gameState.tasks) if(!checkT.isCompleted) allAllDone = false;
                    if (allAllDone) {
                         QMessageBox::information(this, "提示", "所有任务已完成！请回办公室汇报。");
                    }
                }
                break;
            }
        }
    }

    if (!taskFound) {
         QMessageBox::warning(this, "提示", "本层没有任务。");
    } else if (!needed) {
         QMessageBox::warning(this, "提示", "本层不需要此物品。");
    }

    refreshInventoryList();
    refreshTaskList();
    renderScene();
}

void Test3::checkEmergencyTask() {
}

int Test3::getNormalRandom(int min, int max) {
    double u1 = QRandomGenerator::global()->generateDouble();
    while (u1 <= 0.0) u1 = QRandomGenerator::global()->generateDouble();

    double u2 = QRandomGenerator::global()->generateDouble();
    double randStd = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    int val = std::round(5.0 + 2.0 * randStd);
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

void Test3::handleClockIn() {
    if (latenessTimer->isActive()) {
        latenessTimer->stop();
    }

    if (isLate) {
         errorLog.lateClockIn = true;
         QMessageBox::warning(this, "通知", "打卡成功，但你迟到了！");
         emit logMessage("上班打卡 (迟到)");
    } else {
         QMessageBox::information(this, "通知", "上班打卡成功!");
         emit logMessage("上班打卡 (正常)");
    }

    gameState.hasClockedIn = true;
    renderScene();
}

void Test3::handleClockOut() {
    emit logMessage("已打卡下班");
    gameState.hasClockedIn = false;
    QMessageBox::information(this, "通知", "下班打卡成功! 请回到入口回家。");
    renderScene();
}

void Test3::handleGetTask() {
    if (gameState.hasReceivedTask) return;

    auto generateTask = [this](int floor, bool emergency) -> Task {
        Task t;
        t.targetFloor = floor;
        t.isEmergency = emergency;
        t.isCompleted = false;

        QStringList allTypes = {"大床单", "大被套", "小被套", "枕巾", "晚安巾", "毛巾"};
        for (int i = 0; i < allTypes.size(); ++i) {
            int j = QRandomGenerator::global()->bounded(allTypes.size());
            allTypes.swapItemsAt(i, j);
        }

        // Configurable limits
        int typesCount = QRandomGenerator::global()->bounded(4, Config::Test3::Logic::MAX_TASK_ITEM_TYPES + 1);
        for (int i = 0; i < typesCount; ++i) {
            int count = getNormalRandom(1, Config::Test3::Logic::MAX_TASK_ITEM_COUNT);
            t.requiredItems.insert(allTypes[i], count);
        }
        return t;
    };

    // Fixed Tasks: Floor 6 and 7
    gameState.tasks.append(generateTask(Config::Test3::Logic::TASK_FIXED_FLOOR_1, false));
    gameState.tasks.append(generateTask(Config::Test3::Logic::TASK_FIXED_FLOOR_2, false));

    // Emergency Events Trigger
    if (isEmergencyEnabled) {
        // Decide which event will happen (50/50 split)
        bool isEventB = QRandomGenerator::global()->bounded(2) == 0;

        if (isEventB) {
            // Event B: Dirty Linen
            int targetFloor = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
            gameState.dirtyBagState[targetFloor] = true;
            emit logMessage("突发事件B: 脏布草回收");
        } else {
            // Event A: New Task
            int delay = QRandomGenerator::global()->bounded(20000, 40001); // 20-40s
            QTimer::singleShot(delay, this, [this, generateTask]() {
                bool allDone = true;
                for(const auto &t : gameState.tasks) if(!t.isCompleted) allDone = false;
                if (allDone) return; // Skip if game effectively over

                // Generate random floor 2-10 excluding 6,7
                int floorA;
                do {
                    floorA = QRandomGenerator::global()->bounded(2, 11);
                } while (floorA == 6 || floorA == 7);

                Task t = generateTask(floorA, true);
                gameState.tasks.append(t);
                isEmergencyActive = true;

                // Requirement 6: Custom Text
                QString msg = QString(Config::Test3::Texts::POPUP_EMERGENCY_MANAGER).arg(floorA);
                QMessageBox::warning(this, "突发事件", msg);
                emit logMessage(QString("突发事件A: %1楼").arg(floorA));
                refreshTaskList();
            });
        }
    }

    gameState.hasReceivedTask = true;
    refreshTaskList();
    emit logMessage("领取任务完成");
    // Requirement: "Don't show task sheet popup automatically"
    // showTaskSheet(0);
    renderScene();
}

void Test3::handleReportWork() {
    // Generate Report based on Error Log
    QString msg = Config::Test3::Texts::REPORT_SUCCESS;
    QStringList errors;

    // Check missing tasks
    bool incomplete = false;
    for(const auto &t : gameState.tasks) if(!t.isCompleted) incomplete = true;
    if (incomplete) errors << Config::Test3::Texts::REPORT_ERR_MISSING_TASK;

    if (errorLog.lateClockIn) errors << Config::Test3::Texts::REPORT_ERR_LATE;
    if (errorLog.missedEmergencyPriority) errors << Config::Test3::Texts::REPORT_ERR_PRIORITY;

    if (!errors.isEmpty()) {
        msg = errors.join("\n");
    }

    QMessageBox::information(this, "汇报结果", msg);
    gameState.hasReported = true;
    emit logMessage("已汇报工作: " + msg);
    renderScene();
}

void Test3::handleGoHome() {
    // Check final errors
    if (!gameState.hasReported) errorLog.noReportBeforeHome = true;
    if (gameState.hasClockedIn) errorLog.noClockOutBeforeHome = true; // Still clocked in

    if (errorLog.noReportBeforeHome) emit logMessage("错误: 下班前未汇报");
    if (errorLog.noClockOutBeforeHome) emit logMessage("错误: 下班前未打卡");

    emit levelCompleted();
}

void Test3::handleElevatorButton(int floor) {
    emit logMessage(QString("电梯前往 %1 楼").arg(floor));
    QTimer::singleShot(500, this, [this, floor]() {
        gameState.currentFloor = floor;
        emit logMessage(QString("抵达 %1 楼").arg(floor));
        goToScene(GameScene::ElevatorHall);
    });
}

void Test3::showTaskSheet(int taskIndex) {
    if (gameState.tasks.isEmpty()) {
        QMessageBox::information(this, "提示", "当前没有任务。");
        return;
    }
    if (taskIndex < 0 || taskIndex >= gameState.tasks.size()) taskIndex = 0;
    const Task &t = gameState.tasks[taskIndex];

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle(QString("物资申领表 (任务 %1)").arg(taskIndex + 1));
    dlg->setFixedSize(Config::Test3::Geometry::SHEET_DIALOG);

    // Install filter on dialog for dev coordinates
    installDevFilter(dlg);

    QLabel *bg = new QLabel(dlg);
    QPixmap pix(Config::Test3::Images::UI_TASK_SHEET);
    if (pix.isNull()) {
        pix = QPixmap(Config::Test3::Geometry::SHEET_DIALOG);
        pix.fill(Qt::white);
    } else {
        pix = pix.scaled(Config::Test3::Geometry::SHEET_DIALOG, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPainter painter(&pix);
    painter.setPen(QColor(0, 0, 0));
    painter.setFont(QFont(Config::Test3::Fonts::SHEET_FONT_FAMILY,
                          Config::Test3::Fonts::SHEET_FONT_SIZE,
                          Config::Test3::Fonts::SHEET_FONT_WEIGHT)); // Use new config

    auto drawCenteredText = [&](QPoint center, QString text) {
        int w = Config::Test3::Geometry::SHEET_TEXT_BOX.width();
        int h = Config::Test3::Geometry::SHEET_TEXT_BOX.height();
        QRect rect(center.x() - w/2, center.y() - h/2, w, h);
        painter.drawText(rect, Qt::AlignCenter, text);
    };

    drawCenteredText(Config::Test3::Geometry::TXT_FLOOR, QString::number(t.targetFloor));

    QMap<QString, QPoint> itemCoords;
    itemCoords["大床单"] = Config::Test3::Geometry::TXT_SHEET;
    itemCoords["大被套"] = Config::Test3::Geometry::TXT_DUVET;
    itemCoords["小被套"] = Config::Test3::Geometry::TXT_S_DUVET;
    itemCoords["枕巾"] = Config::Test3::Geometry::TXT_PILLOW;
    itemCoords["晚安巾"] = Config::Test3::Geometry::TXT_GN_TOWEL;
    itemCoords["毛巾"] = Config::Test3::Geometry::TXT_TOWEL;

    for (auto it = itemCoords.begin(); it != itemCoords.end(); ++it) {
        int count = t.requiredItems.value(it.key(), 0);
        drawCenteredText(it.value(), QString::number(count));
    }

    bg->setPixmap(pix);
    bg->setGeometry(0,0,Config::Test3::Geometry::SHEET_DIALOG.width(), Config::Test3::Geometry::SHEET_DIALOG.height());

    dlg->exec();
}

QPixmap Test3::generatePlaceholder(QString text, QColor color, QSize size) {
    emit logMessage("Generating Placeholder for: " + text); // Debug log
    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
