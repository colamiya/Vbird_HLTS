#include "test3.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMap>

Test3::Test3(bool isDevMode, QWidget *parent) : QWidget(parent), isDeveloperMode(isDevMode) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // Left Panel
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(192);
    leftPanel->setStyleSheet("background-color: #2c3e50; color: white; border-right: 1px solid #1a252f;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    locationLabel = new QLabel("当前位置: 入口");
    locationLabel->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    cartStatusLabel = new QLabel(); // Image label
    cartStatusLabel->setFixedSize(150, 150);
    cartStatusLabel->setScaledContents(true);

    leftLayout->addWidget(locationLabel);
    leftLayout->addStretch();
    leftLayout->addWidget(cartStatusLabel, 0, Qt::AlignBottom | Qt::AlignHCenter);
    leftLayout->addSpacing(20);
    mainLayout->addWidget(leftPanel);

    // Center Panel
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(896, 720);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    rpgCenterPanel->installEventFilter(this);
    mainLayout->addWidget(rpgCenterPanel);

    // Right Panel
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(192);
    rightPanel->setStyleSheet("background-color: #34495e; color: white; border-left: 1px solid #1a252f;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    QLabel *taskTitle = new QLabel("当前任务:");
    taskTitle->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    rightLayout->addWidget(taskTitle);

    QPushButton *viewTaskSheetBtn = new QPushButton("查看申领表");
    viewTaskSheetBtn->setStyleSheet("font-size: 12px; padding: 5px; background-color: #e67e22;");
    connect(viewTaskSheetBtn, &QPushButton::clicked, this, &Test3::showTaskSheet);
    rightLayout->addWidget(viewTaskSheetBtn);

    taskListWidget = new QListWidget();
    taskListWidget->setStyleSheet("color: black; background: white;");
    rightLayout->addWidget(taskListWidget);

    QLabel *invTitle = new QLabel("推车存货 (拖拽使用):");
    invTitle->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    rightLayout->addWidget(invTitle);

    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(QSize(64, 64)); // Large icons
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
    locationLabel->setText("当前位置:\n" + locStr);

    // Cart Status Image
    QString statusImg;
    int cleanCount = 0;
    for (auto v : gameState.inventory.cleanItems) cleanCount += v;

    if (gameState.inventory.dirtyItemsCount > 0) {
        statusImg = "source/Test3/推车-脏布草.png";
    } else if (cleanCount > 0) {
        statusImg = "source/Test3/推车-布草.png";
    } else {
        statusImg = "source/Test3/推车-空.png";
    }

    QPixmap pix(statusImg);
    if (pix.isNull()) {
        cartStatusLabel->setText(gameState.inventory.dirtyItemsCount > 0 ? "脏布草" : (cleanCount > 0 ? "有布草" : "空车"));
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

            // Load Icon
            QString iconPath = QString("source/Test3/%1.png").arg(it.key());
            if (QFile::exists(iconPath)) {
                item->setIcon(QIcon(iconPath));
            }

            inventoryListWidget->addItem(item);
        }
    }

    // Show Dirty Items
    if (gameState.inventory.dirtyItemsCount > 0) {
         QListWidgetItem *item = new QListWidgetItem("脏布草");
         QString iconPath = "source/Test3/脏布草.jpg";
         if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
         item->setData(Qt::UserRole, "DirtyLinen");
         inventoryListWidget->addItem(item);
    }
    updateRPGStatusLabels();
}

void Test3::refreshTaskList() {
    taskListWidget->clear();
    for (const Task &t : gameState.tasks) {
        if (!t.isCompleted) {
            QString txt = QString("%1楼 %2\n").arg(t.targetFloor).arg(t.isEmergency ? "[紧急]" : "");
            for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
                if (it.value() > 0)
                    txt += QString("- %1: %2\n").arg(it.key()).arg(it.value());
            }
            taskListWidget->addItem(txt);
        }
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
    QPixmap pix("source/Test3/入口.jpg");
    if (pix.isNull()) pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    QPushButton *btn = new QPushButton("进入酒店", rpgCenterPanel);
    btn->setGeometry(350, 600, 200, 50);

    if (gameState.hasReported && !gameState.hasClockedIn) {
        btn->setText("下班回家");
        btn->setStyleSheet("background-color: #27ae60; color: white; font-size: 18px;");
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
    QPixmap pix("source/Test3/员工通道走廊.jpg");
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    if (!gameState.hasClockedIn) {
        if (gameState.hasReported) {
            QLabel *lbl = new QLabel("已打卡下班", rpgCenterPanel);
            lbl->setGeometry(100, 100, 200, 50);
            lbl->setStyleSheet("font-size: 18px; color: green; font-weight: bold;");
            lbl->show();

            QPushButton *exitBtn = new QPushButton("返回入口", rpgCenterPanel);
            exitBtn->setGeometry(350, 600, 200, 50);
            connect(exitBtn, &QPushButton::clicked, [this]() {
                goToScene(GameScene::Entrance);
            });
            exitBtn->show();
        } else {
             QPushButton *clockInBtn = new QPushButton("打卡签到", rpgCenterPanel);
             clockInBtn->setGeometry(100, 100, 150, 50);
             connect(clockInBtn, &QPushButton::clicked, this, &Test3::handleClockIn);
             clockInBtn->show();
        }
    } else {
        if (gameState.hasReported) {
             QPushButton *clockOutBtn = new QPushButton("打卡下班", rpgCenterPanel);
             clockOutBtn->setGeometry(100, 100, 150, 50);
             clockOutBtn->setStyleSheet("background-color: #e74c3c; color: white; font-size: 18px; font-weight: bold;");
             connect(clockOutBtn, &QPushButton::clicked, this, &Test3::handleClockOut);
             clockOutBtn->show();
        }

        QPushButton *officeBtn = new QPushButton("去办公室", rpgCenterPanel);
        officeBtn->setGeometry(50, 300, 150, 50);
        connect(officeBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Office); });
        officeBtn->show();

        QPushButton *warehouseBtn = new QPushButton("去仓库", rpgCenterPanel);
        warehouseBtn->setGeometry(250, 300, 150, 50);
        connect(warehouseBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
        warehouseBtn->show();

        QPushButton *elevatorBtn = new QPushButton("去电梯", rpgCenterPanel);
        elevatorBtn->setGeometry(450, 300, 150, 50);
        connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
        elevatorBtn->show();
    }
}

void Test3::renderOffice() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/办公室.png");
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    bool allTasksDone = !gameState.tasks.isEmpty();
    for(const auto &t : gameState.tasks) {
        if (!t.isCompleted) allTasksDone = false;
    }
    if (gameState.tasks.isEmpty()) allTasksDone = false;

    if (allTasksDone && !gameState.hasReported) {
        QPushButton *reportBtn = new QPushButton("汇报工作", rpgCenterPanel);
        reportBtn->setGeometry(100, 100, 150, 50);
        reportBtn->setStyleSheet("background-color: #f1c40f; color: black;");
        connect(reportBtn, &QPushButton::clicked, this, &Test3::handleReportWork);
        reportBtn->show();
    } else if (gameState.hasReported) {
         QLabel *lbl = new QLabel("工作已汇报，请去走廊下班。", rpgCenterPanel);
         lbl->setGeometry(100, 100, 300, 50);
         lbl->setStyleSheet("font-size: 16px; color: green; font-weight: bold;");
         lbl->show();
    } else {
        QPushButton *getTaskBtn = new QPushButton("领取任务", rpgCenterPanel);
        getTaskBtn->setGeometry(100, 100, 150, 50);
        if (gameState.hasReceivedTask) {
            getTaskBtn->setEnabled(false);
            getTaskBtn->setText("任务进行中...");
        } else {
            connect(getTaskBtn, &QPushButton::clicked, this, &Test3::handleGetTask);
        }
        getTaskBtn->show();
    }

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel);
    backBtn->setGeometry(100, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void Test3::renderWarehouse() {
    // 1. Show Entry view: 仓库1.jpg
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/仓库1.jpg");
    if (pix.isNull()) pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    // 2. Button "拿取布草" -> Go to Shelf View
    QPushButton *takeBtn = new QPushButton("拿取布草", rpgCenterPanel);
    takeBtn->setGeometry(100, 300, 150, 60);
    takeBtn->setStyleSheet("font-size: 18px; background-color: #3498db; color: white;");
    connect(takeBtn, &QPushButton::clicked, [this]() {
         goToScene(GameScene::WarehouseShelf);
    });
    takeBtn->show();

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel);
    backBtn->setGeometry(50, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void Test3::renderWarehouseShelf() {
    // Shelf View: 取布草的货架.jpg
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/取布草的货架.jpg");
    if (pix.isNull()) pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    // Helper lambda to create Shelf Area
    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        area->setGeometry(rect);
        // Put back logic: If dropped here, remove from cart
        area->onDropCallback = [this](QString item) {
             handleSceneDrop(item, true); // true = warehouse (put back)
        };
        area->show();
    };

    createShelfArea("大床单", SHELF_RECT_1);
    createShelfArea("大被套", SHELF_RECT_2);
    createShelfArea("小被套", SHELF_RECT_3);
    createShelfArea("枕巾", SHELF_RECT_4);
    createShelfArea("晚安巾", SHELF_RECT_5);
    createShelfArea("毛巾", SHELF_RECT_6);

    QPushButton *backBtn = new QPushButton("返回入口", rpgCenterPanel);
    backBtn->setGeometry(50, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
    backBtn->show();
}

void Test3::renderElevatorHall() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/电梯厅.jpg");
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    QPushButton *callElevator = new QPushButton("进入电梯", rpgCenterPanel);
    callElevator->setGeometry(350, 300, 200, 100);
    connect(callElevator, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorInside); });
    callElevator->show();

    QPushButton *backBtn = new QPushButton("返回", rpgCenterPanel);
    backBtn->setGeometry(100, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() {
        if (gameState.currentFloor == 0) goToScene(GameScene::StaffHallway);
        else goToScene(GameScene::FloorCorridor);
    });
    backBtn->show();
}

void Test3::renderElevatorInside() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/电梯内.jpg");
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    QMap<int, QString> floors;
    floors[0] = "G";
    floors[6] = "6";
    floors[7] = "7";

    int y = 100;
    for (auto it = floors.begin(); it != floors.end(); ++it) {
        int floor = it.key();
        QPushButton *btn = new QPushButton(it.value(), rpgCenterPanel);
        btn->setGeometry(400, y, 80, 80);
        connect(btn, &QPushButton::clicked, [this, floor]() { handleElevatorButton(floor); });
        btn->show();
        y += 100;
    }
}

void Test3::renderFloorCorridor() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/楼层走廊-前.png");
    if (pix.isNull()) pix = generatePlaceholder("走廊", Qt::cyan, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    QPushButton *linenRoomBtn = new QPushButton("布草间", rpgCenterPanel);
    linenRoomBtn->setGeometry(200, 200, 200, 100);
    connect(linenRoomBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::LinenRoom); });
    linenRoomBtn->show();

    QPushButton *elevatorBtn = new QPushButton("电梯厅", rpgCenterPanel);
    elevatorBtn->setGeometry(500, 200, 200, 100);
    connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
    elevatorBtn->show();
}

void Test3::renderLinenRoom() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/布草间-空.jpg");
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);
    bg->show();

    // Check Dirty Linen Status
    if (!gameState.dirtyBagState.contains(gameState.currentFloor)) {
         gameState.dirtyBagState[gameState.currentFloor] = false;
    }

    // Create Drop Targets (Using DropLabel logic, but we can reuse ShelfArea for consistency if we wanted,
    // but DropLabel was already defined for this purpose. Let's stick to DropLabel for LinenRoom
    // OR switch to ShelfArea which handles Drag too if we want to take back?
    // User only said "Put". But usually we might want to take back if we put wrong.
    // Let's use ShelfArea for consistency and power!

    auto createShelfArea = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        area->setGeometry(rect);
        area->onDropCallback = [this](QString item) {
             handleSceneDrop(item, false); // false = linen room (delivery)
        };
        area->show();
    };

    createShelfArea("大床单", SHELF_RECT_1);
    createShelfArea("大被套", SHELF_RECT_2);
    createShelfArea("小被套", SHELF_RECT_3);
    createShelfArea("枕巾", SHELF_RECT_4);
    createShelfArea("晚安巾", SHELF_RECT_5);
    createShelfArea("毛巾", SHELF_RECT_6);

    // If Dirty Linen Present
    if (gameState.dirtyBagState[gameState.currentFloor]) {
        DragSourceLabel *dirty = new DragSourceLabel("脏布草", rpgCenterPanel);
        QPixmap dirtyPix("source/Test3/脏布草.jpg");
        if (!dirtyPix.isNull()) dirty->setPixmap(dirtyPix.scaled(100, 100));
        dirty->setText("脏布草(拖拽)");
        dirty->setGeometry(700, 400, 100, 100);
        dirty->show();
    }

    QPushButton *backBtn = new QPushButton("返回走廊", rpgCenterPanel);
    backBtn->setGeometry(50, 600, 150, 50);
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
                    checkEmergencyTask();
                }
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
    if (QRandomGenerator::global()->bounded(100) < 50) {
        QMessageBox::information(this, "紧急情况", "收到新的紧急请求：回收脏布草！");
        int otherFloor = (gameState.currentFloor == 6) ? 7 : 6;
        gameState.dirtyBagState[otherFloor] = true;
        emit logMessage("生成紧急任务: " + QString::number(otherFloor) + "楼 脏布草");
    }
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

    Task t;
    t.targetFloor = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
    t.isEmergency = false;
    t.isCompleted = false;

    QStringList allTypes = {"大床单", "大被套", "小被套", "枕巾", "晚安巾", "毛巾"};
    for (int i = 0; i < allTypes.size(); ++i) {
        int j = QRandomGenerator::global()->bounded(allTypes.size());
        allTypes.swapItemsAt(i, j);
    }
    int typesCount = QRandomGenerator::global()->bounded(4, 7);
    for (int i = 0; i < typesCount; ++i) {
        t.requiredItems.insert(allTypes[i], QRandomGenerator::global()->bounded(1, 6));
    }

    gameState.tasks.append(t);
    gameState.hasReceivedTask = true;

    refreshTaskList();
    emit logMessage(QString("领取任务: %1楼").arg(t.targetFloor));
    showTaskSheet();
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

void Test3::showTaskSheet() {
    if (gameState.tasks.isEmpty()) {
        QMessageBox::information(this, "提示", "当前没有任务。");
        return;
    }
    const Task &t = gameState.tasks.last();

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("物资申领表");
    dlg->setFixedSize(600, 800);

    if (isDeveloperMode) {
        dlg->installEventFilter(this);
    }

    QLabel *bg = new QLabel(dlg);
    QPixmap pix("source/Test3/申领表.png");
    if (pix.isNull()) {
        pix = QPixmap(600, 800);
        pix.fill(Qt::white);
    } else {
        pix = pix.scaled(600, 800, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPainter painter(&pix);
    painter.setPen(QColor(0, 0, 0));
    painter.setFont(QFont("Arial", 16, QFont::Bold));

    painter.drawText(COORD_FLOOR, QString::number(t.targetFloor));

    QMap<QString, QPoint> itemCoords;
    itemCoords["大床单"] = COORD_ITEM_1;
    itemCoords["大被套"] = COORD_ITEM_2;
    itemCoords["小被套"] = COORD_ITEM_3;
    itemCoords["枕巾"] = COORD_ITEM_4;
    itemCoords["晚安巾"] = COORD_ITEM_5;
    itemCoords["毛巾"] = COORD_ITEM_6;

    for (auto it = itemCoords.begin(); it != itemCoords.end(); ++it) {
        int count = t.requiredItems.value(it.key(), 0);
        painter.drawText(it.value(), QString::number(count));
    }

    bg->setPixmap(pix);
    bg->setGeometry(0,0,600,800);

    if (isDeveloperMode) {
        ClickableArea *overlay = new ClickableArea(dlg);
        overlay->setGeometry(0, 0, 600, 800);
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
