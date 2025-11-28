#include "test3.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMap>
#include <QFile>
#include <cmath>
#include <QtMath>
#include "config.h"

Test3::Test3(bool isDevMode, QWidget *parent) : QWidget(parent), isDeveloperMode(isDevMode) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // Left Panel
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    leftPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_LEFT);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    locationLabel = new QLabel(Config::Test3::Texts::LBL_LOCATION_PREFIX + "入口");
    locationLabel->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    cartStatusLabel = new QLabel(); // Image label
    cartStatusLabel->setFixedSize(Config::Test3::Geometry::ICON_CART);
    cartStatusLabel->setScaledContents(true);

    leftLayout->addWidget(locationLabel);
    leftLayout->addStretch();
    leftLayout->addWidget(cartStatusLabel, 0, Qt::AlignBottom | Qt::AlignHCenter);
    leftLayout->addSpacing(20);
    mainLayout->addWidget(leftPanel);

    // Center Panel
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(Config::Test3::Geometry::CENTER_PANEL_SIZE);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    rpgCenterPanel->installEventFilter(this);
    mainLayout->addWidget(rpgCenterPanel);

    // Right Panel
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(taskTitle);

    taskListWidget = new QListWidget();
    taskListWidget->setStyleSheet(Config::Test3::Styles::LIST_WIDGET);
    taskListWidget->setFixedHeight(Config::Test3::Geometry::TASK_LIST_HEIGHT);
    rightLayout->addWidget(taskListWidget);

    QPushButton *viewTaskSheetBtn = new QPushButton(Config::Test3::Texts::BTN_VIEW_TASK_SHEET);
    viewTaskSheetBtn->setStyleSheet(QString("font-size: 12px; padding: 5px; %1").arg(Config::Test3::Styles::BTN_ORANGE));
    connect(viewTaskSheetBtn, &QPushButton::clicked, [this](){
        // Get selected index
        int idx = taskListWidget->currentRow();
        if (idx < 0 && taskListWidget->count() > 0) idx = 0;
        showTaskSheet(idx);
    });
    rightLayout->addWidget(viewTaskSheetBtn);

    QLabel *invTitle = new QLabel(Config::Test3::Texts::LBL_INVENTORY_TITLE);
    invTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE);
    rightLayout->addWidget(invTitle);

    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY); // Large icons
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName) {
        handleInventoryDrop(itemName);
    };
    rightLayout->addWidget(inventoryListWidget);

    mainLayout->addWidget(rightPanel);

    // Init State
    gameState.currentScene = GameScene::Entrance;
    gameState.currentFloor = 0;
    gameState.hasClockedIn = false;
    gameState.hasReceivedTask = false;
    gameState.hasReported = false;

    renderScene();
}

bool Test3::eventFilter(QObject *watched, QEvent *event) {
    if (watched == rpgCenterPanel && event->type() == QEvent::MouseButtonPress && isDeveloperMode) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint pos = mouseEvent->pos();
        QString coordText = QString("DevMode Scene Click: (%1, %2)").arg(pos.x()).arg(pos.y());
        qDebug() << coordText;
        emit logMessage(coordText);
        QMessageBox::information(this, "坐标", coordText);
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

// --- Logic ---

void Test3::goToScene(GameScene scene) {
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
        case GameScene::ElevatorHall: locStr = "电梯厅"; break;
        case GameScene::ElevatorInside: locStr = "电梯内"; break;
        case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break;
        case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break;
    }
    locationLabel->setText(Config::Test3::Texts::LBL_LOCATION_PREFIX + locStr);

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
            QString iconPath = Config::Test3::Images::ITEMS.value(it.key());
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
    // Clear
    QList<QObject*> children = rpgCenterPanel->children();
    for (QObject *child : children) {
        if (child->isWidgetType()) static_cast<QWidget*>(child)->hide();
        child->deleteLater();
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
}

void Test3::renderEntrance() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ENTRANCE);
    if (pix.isNull()) pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *btn = new QPushButton(Config::Test3::Texts::BTN_ENTER_HOTEL, rpgCenterPanel);
    btn->setGeometry(Config::Test3::Geometry::BTN_ENTRANCE_ACTION);

    if (gameState.hasReported && !gameState.hasClockedIn) {
        btn->setText(Config::Test3::Texts::BTN_GO_HOME);
        btn->setStyleSheet(Config::Test3::Styles::BTN_GREEN);
        connect(btn, &QPushButton::clicked, this, &Test3::handleGoHome);
    } else {
        connect(btn, &QPushButton::clicked, [this]() {
            goToScene(GameScene::StaffHallway);
        });
    }
    btn->show();
}

void Test3::renderStaffHallway() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_HALLWAY);
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    if (!gameState.hasClockedIn) {
        if (gameState.hasReported) {
            QLabel *lbl = new QLabel(Config::Test3::Texts::LBL_CLOCKED_OFF, rpgCenterPanel);
            lbl->setGeometry(Config::Test3::Geometry::LBL_HALLWAY_STATUS);
            lbl->setStyleSheet(Config::Test3::Styles::LBL_SUCCESS_GREEN);
            lbl->show();

            QPushButton *exitBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_ENTRANCE, rpgCenterPanel);
            exitBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_EXIT);
            connect(exitBtn, &QPushButton::clicked, [this]() {
                goToScene(GameScene::Entrance);
            });
            exitBtn->show();
        } else {
             QPushButton *clockInBtn = new QPushButton(Config::Test3::Texts::BTN_CLOCK_IN, rpgCenterPanel);
             clockInBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_CLOCK_ACTION);
             connect(clockInBtn, &QPushButton::clicked, this, &Test3::handleClockIn);
             clockInBtn->show();
        }
    } else {
        if (gameState.hasReported) {
             QPushButton *clockOutBtn = new QPushButton(Config::Test3::Texts::BTN_CLOCK_OUT, rpgCenterPanel);
             clockOutBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_CLOCK_ACTION);
             clockOutBtn->setStyleSheet(Config::Test3::Styles::BTN_RED);
             connect(clockOutBtn, &QPushButton::clicked, this, &Test3::handleClockOut);
             clockOutBtn->show();
        }

        QPushButton *officeBtn = new QPushButton(Config::Test3::Texts::BTN_GO_OFFICE, rpgCenterPanel);
        officeBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_GO_OFFICE);
        connect(officeBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Office); });
        officeBtn->show();

        QPushButton *warehouseBtn = new QPushButton(Config::Test3::Texts::BTN_GO_WAREHOUSE, rpgCenterPanel);
        warehouseBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_GO_WAREHOUSE);
        connect(warehouseBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
        warehouseBtn->show();

        QPushButton *elevatorBtn = new QPushButton(Config::Test3::Texts::BTN_GO_ELEVATOR, rpgCenterPanel);
        elevatorBtn->setGeometry(Config::Test3::Geometry::BTN_HALLWAY_GO_ELEVATOR);
        connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
        elevatorBtn->show();
    }
}

void Test3::renderOffice() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_OFFICE);
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    bool allTasksDone = !gameState.tasks.isEmpty();
    for(const auto &t : gameState.tasks) {
        if (!t.isCompleted) allTasksDone = false;
    }
    if (gameState.tasks.isEmpty()) allTasksDone = false;

    if (allTasksDone && !gameState.hasReported) {
        QPushButton *reportBtn = new QPushButton(Config::Test3::Texts::BTN_REPORT_WORK, rpgCenterPanel);
        reportBtn->setGeometry(Config::Test3::Geometry::BTN_OFFICE_MAIN_ACTION);
        reportBtn->setStyleSheet(Config::Test3::Styles::BTN_YELLOW);
        connect(reportBtn, &QPushButton::clicked, this, &Test3::handleReportWork);
        reportBtn->show();
    } else if (gameState.hasReported) {
         QLabel *lbl = new QLabel(Config::Test3::Texts::LBL_WORK_REPORTED, rpgCenterPanel);
         lbl->setGeometry(Config::Test3::Geometry::LBL_OFFICE_MSG);
         lbl->setStyleSheet(Config::Test3::Styles::LBL_SUCCESS_GREEN);
         lbl->show();
    } else {
        QPushButton *getTaskBtn = new QPushButton(Config::Test3::Texts::BTN_GET_TASK, rpgCenterPanel);
        getTaskBtn->setGeometry(Config::Test3::Geometry::BTN_OFFICE_MAIN_ACTION);
        if (gameState.hasReceivedTask) {
            getTaskBtn->setEnabled(false);
            getTaskBtn->setText(Config::Test3::Texts::BTN_TASK_IN_PROGRESS);
        } else {
            connect(getTaskBtn, &QPushButton::clicked, this, &Test3::handleGetTask);
        }
        getTaskBtn->show();
    }

    QPushButton *backBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_HALLWAY, rpgCenterPanel);
    backBtn->setGeometry(Config::Test3::Geometry::BTN_OFFICE_BACK);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void Test3::renderWarehouse() {
    // 1. Show Entry view: 仓库1.jpg
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_WAREHOUSE_ENTRY);
    if (pix.isNull()) pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 2. Button "拿取布草" -> Go to Shelf View
    QPushButton *takeBtn = new QPushButton(Config::Test3::Texts::BTN_TAKE_LINEN, rpgCenterPanel);
    takeBtn->setGeometry(Config::Test3::Geometry::BTN_WAREHOUSE_TAKE);
    takeBtn->setStyleSheet(Config::Test3::Styles::BTN_BLUE);
    connect(takeBtn, &QPushButton::clicked, [this]() {
         goToScene(GameScene::WarehouseShelf);
    });
    takeBtn->show();

    QPushButton *backBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_HALLWAY, rpgCenterPanel);
    backBtn->setGeometry(Config::Test3::Geometry::BTN_WAREHOUSE_BACK);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void Test3::renderWarehouseShelf() {
    // Shelf View: 取布草的货架.jpg
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_WAREHOUSE_SHELF);
    if (pix.isNull()) pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Helper lambda to create Shelf Area
    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        area->setGeometry(rect);

        // Set style for transparent grey background
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);

        // Add image to shelf area
        QString iconPath = Config::Test3::Images::ITEMS.value(name);
        if (QFile::exists(iconPath)) {
            QPixmap pix(iconPath);
            if (!pix.isNull()) {
                 area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                 area->setAlignment(Qt::AlignCenter);
            }
        }

        // Put back logic: If dropped here, remove from cart
        area->onDropCallback = [this, name](QString item) {
             // Validation: Check if item matches shelf
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

    QPushButton *backBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY, rpgCenterPanel);
    backBtn->setGeometry(Config::Test3::Geometry::BTN_SHELF_BACK);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
    backBtn->show();
}

void Test3::renderElevatorHall() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ELEVATOR_HALL);
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *callElevator = new QPushButton(Config::Test3::Texts::BTN_ENTER_ELEVATOR, rpgCenterPanel);
    callElevator->setGeometry(Config::Test3::Geometry::BTN_ELEVATOR_ENTER);
    connect(callElevator, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorInside); });
    callElevator->show();

    QPushButton *backBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_BACK, rpgCenterPanel);
    backBtn->setGeometry(Config::Test3::Geometry::BTN_ELEVATOR_BACK);
    connect(backBtn, &QPushButton::clicked, [this]() {
        if (gameState.currentFloor == 0) goToScene(GameScene::StaffHallway);
        else goToScene(GameScene::FloorCorridor);
    });
    backBtn->show();
}

void Test3::renderElevatorInside() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_ELEVATOR_INSIDE);
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QMap<int, QString> floors;
    floors[0] = "G";
    floors[6] = "6";
    floors[7] = "7";

    int y = Config::Test3::Geometry::ELEVATOR_BTN_START_Y;
    for (auto it = floors.begin(); it != floors.end(); ++it) {
        int floor = it.key();
        QPushButton *btn = new QPushButton(it.value(), rpgCenterPanel);
        btn->setGeometry(Config::Test3::Geometry::ELEVATOR_BTN_X, y, Config::Test3::Geometry::ELEVATOR_BTN_SIZE.width(), Config::Test3::Geometry::ELEVATOR_BTN_SIZE.height());
        connect(btn, &QPushButton::clicked, [this, floor]() { handleElevatorButton(floor); });
        btn->show();
        y += Config::Test3::Geometry::ELEVATOR_BTN_SPACING;
    }
}

void Test3::renderFloorCorridor() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_FLOOR_CORRIDOR);
    if (pix.isNull()) pix = generatePlaceholder("走廊", Qt::cyan, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    QPushButton *linenRoomBtn = new QPushButton(Config::Test3::Texts::BTN_GO_LINEN_ROOM, rpgCenterPanel);
    linenRoomBtn->setGeometry(Config::Test3::Geometry::BTN_CORRIDOR_LINEN);
    connect(linenRoomBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::LinenRoom); });
    linenRoomBtn->show();

    QPushButton *elevatorBtn = new QPushButton(Config::Test3::Texts::BTN_GO_ELEVATOR_HALL, rpgCenterPanel);
    elevatorBtn->setGeometry(Config::Test3::Geometry::BTN_CORRIDOR_ELEVATOR);
    connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
    elevatorBtn->show();
}

void Test3::renderLinenRoom() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix(Config::Test3::Images::SCENE_LINEN_ROOM_EMPTY);
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // Check Dirty Linen Status
    if (!gameState.dirtyBagState.contains(gameState.currentFloor)) {
         gameState.dirtyBagState[gameState.currentFloor] = false;
    }

    // Create Drop Targets
    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        area->setGeometry(rect);

        // Set style for transparent grey background
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);

        // Add image to shelf area (Grey background, item icon inside)
        QString iconPath = Config::Test3::Images::ITEMS.value(name);
        if (QFile::exists(iconPath)) {
            QPixmap pix(iconPath);
            if (!pix.isNull()) {
                 area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                 area->setAlignment(Qt::AlignCenter);
            }
        }

        area->onDropCallback = [this, name](QString item) {
             // Validation: Check if item matches shelf
             if (item != name) {
                 QMessageBox::critical(this, "错误", QString("放置失败：不能将 %1 放置在 %2 的位置！").arg(item, name));
                 return;
             }
             handleSceneDrop(item, false); // false = linen room (delivery)
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_TOWEL);

    // If Dirty Linen Present
    if (gameState.dirtyBagState[gameState.currentFloor]) {
        DragSourceLabel *dirty = new DragSourceLabel("脏布草", rpgCenterPanel);
        QPixmap dirtyPix(Config::Test3::Images::UI_DIRTY_LINEN);
        if (!dirtyPix.isNull()) dirty->setPixmap(dirtyPix.scaled(Config::Test3::Geometry::ICON_DIRTY_DRAG));
        dirty->setText(Config::Test3::Texts::LBL_DIRTY_LINEN_DRAG);
        dirty->setGeometry(Config::Test3::Geometry::LBL_DIRTY_SOURCE);
        dirty->show();
    }

    QPushButton *backBtn = new QPushButton(Config::Test3::Texts::BTN_RETURN_CORRIDOR, rpgCenterPanel);
    backBtn->setGeometry(Config::Test3::Geometry::BTN_LINEN_BACK);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::FloorCorridor); });
    backBtn->show();
}

// --- Action Handlers ---

void Test3::handleInventoryDrop(QString itemName) {
    // Dropped INTO Inventory (Cart)
    // Source was Warehouse or Dirty Linen

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

        gameState.inventory.cleanItems[itemName]++;
        emit logMessage("装车: " + itemName);
    }

    refreshInventoryList();
}

void Test3::handleSceneDrop(QString itemName, bool isWarehouse) {
    // Dropped ONTO Scene (Shelf)
    // Source was Cart

    if (gameState.inventory.cleanItems[itemName] <= 0) return;

    if (isWarehouse) {
        // Warehouse: Putting back (Returning to stock)
        gameState.inventory.cleanItems[itemName]--;
        emit logMessage("放回仓库: " + itemName);
        refreshInventoryList();
        return;
    }

    // Linen Room: Delivery
    bool taskFound = false;
    bool needed = false;

    // Check against all tasks for current floor
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        Task &t = gameState.tasks[i];
        if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
            taskFound = true;
            if (t.requiredItems.contains(itemName) && t.requiredItems[itemName] > 0) {
                needed = true;

                gameState.inventory.cleanItems[itemName]--;
                t.requiredItems[itemName]--;
                emit logMessage("放置 " + itemName + " 到货架");

                bool allDone = true;
                for (auto count : t.requiredItems) if (count > 0) allDone = false;
                if (allDone) {
                    t.isCompleted = true;
                    emit logMessage("任务完成: " + QString::number(gameState.currentFloor) + "楼");

                    bool allAllDone = true;
                    for(const auto &checkT : gameState.tasks) if(!checkT.isCompleted) allAllDone = false;
                    if (allAllDone) {
                         QMessageBox::information(this, "提示", "所有任务已完成！请回办公室汇报。");
                    }
                }
                break; // Found the task that needs it, stop checking others to avoid double decrement if multiple tasks same floor (unlikely but safe)
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
}

void Test3::checkEmergencyTask() {
    // Deprecated in favor of handleGetTask events
}

int Test3::getNormalRandom(int min, int max) {
    // Approximate normal distribution
    double u1 = QRandomGenerator::global()->generateDouble();
    while (u1 <= 0.0) u1 = QRandomGenerator::global()->generateDouble(); // Guard against log(0)

    double u2 = QRandomGenerator::global()->generateDouble();
    double randStd = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);
    int val = std::round(5.0 + 2.0 * randStd); // Mean 5, StdDev 2
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}

void Test3::handleClockIn() {
    QMessageBox::information(this, "通知", "上班打卡成功!");
    gameState.hasClockedIn = true;
    emit logMessage("已上班打卡");
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
        // Randomize
        for (int i = 0; i < allTypes.size(); ++i) {
            int j = QRandomGenerator::global()->bounded(allTypes.size());
            allTypes.swapItemsAt(i, j);
        }
        int typesCount = QRandomGenerator::global()->bounded(4, 7); // 4-6 types
        for (int i = 0; i < typesCount; ++i) {
            int count = getNormalRandom(1, 10);
            t.requiredItems.insert(allTypes[i], count);
        }
        return t;
    };

    // Task 1
    int floor1 = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
    gameState.tasks.append(generateTask(floor1, false));

    // Emergency
    if (isEmergencyEnabled) {
        bool isEventA = QRandomGenerator::global()->bounded(2) == 0;
        if (isEventA) {
            // Event A: Extra Task
            int floor2 = QRandomGenerator::global()->bounded(2, 13);
            while (floor2 == floor1) floor2 = QRandomGenerator::global()->bounded(2, 13);

            gameState.tasks.append(generateTask(floor2, true));
            QMessageBox::warning(this, "突发事件", QString("突发事件A：新增 %1 楼任务！").arg(floor2));
            emit logMessage(QString("突发事件A: %1楼").arg(floor2));
        } else {
            // Event B: Dirty Linen
            gameState.dirtyBagState[floor1] = true;
            QMessageBox::warning(this, "突发事件", "突发事件B：目标楼层布草间有脏布草需回收！");
            emit logMessage("突发事件B: 脏布草回收");
        }
    }

    gameState.hasReceivedTask = true;
    refreshTaskList();
    emit logMessage("领取任务完成");
    showTaskSheet(0);
    renderScene();
}

void Test3::handleReportWork() {
    QMessageBox::information(this, "汇报", "工作汇报完成！");
    gameState.hasReported = true;
    emit logMessage("已汇报工作");
    renderScene();
}

void Test3::handleGoHome() {
    emit levelCompleted();
}

void Test3::handleElevatorButton(int floor) {
    emit logMessage(QString("电梯前往 %1 楼").arg(floor));
    QTimer::singleShot(1000, this, [this, floor]() {
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

    if (isDeveloperMode) {
        dlg->installEventFilter(this);
    }

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
    painter.setFont(QFont("Arial", 16, QFont::Bold));

    painter.drawText(Config::Test3::Geometry::TXT_FLOOR, QString::number(t.targetFloor));

    QMap<QString, QPoint> itemCoords;
    itemCoords["大床单"] = Config::Test3::Geometry::TXT_SHEET;
    itemCoords["大被套"] = Config::Test3::Geometry::TXT_DUVET;
    itemCoords["小被套"] = Config::Test3::Geometry::TXT_S_DUVET;
    itemCoords["枕巾"] = Config::Test3::Geometry::TXT_PILLOW;
    itemCoords["晚安巾"] = Config::Test3::Geometry::TXT_GN_TOWEL;
    itemCoords["毛巾"] = Config::Test3::Geometry::TXT_TOWEL;

    for (auto it = itemCoords.begin(); it != itemCoords.end(); ++it) {
        int count = t.requiredItems.value(it.key(), 0);
        painter.drawText(it.value(), QString::number(count));
    }

    bg->setPixmap(pix);
    bg->setGeometry(0,0,Config::Test3::Geometry::SHEET_DIALOG.width(), Config::Test3::Geometry::SHEET_DIALOG.height());

    if (isDeveloperMode) {
        ClickableArea *overlay = new ClickableArea(dlg);
        overlay->setGeometry(0, 0, Config::Test3::Geometry::SHEET_DIALOG.width(), Config::Test3::Geometry::SHEET_DIALOG.height());
        overlay->setStyleSheet("background: transparent;");
        // No explicit connection needed, mousePressEvent handles it
    }

    dlg->exec();
}

QPixmap Test3::generatePlaceholder(QString text, QColor color, QSize size) {
    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
