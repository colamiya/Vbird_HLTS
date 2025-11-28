#include "mainwindow.h"
#include <QApplication>

// --- Constructor & Setup ---

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Fixed Window Size
    setFixedSize(1280, 720);
    setWindowTitle("酒店管理学生实训系统"); // Hotel Management Student Training

    setupStyle(); // 应用主题 Apply the theme
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupStyle() {
    // 样式表定义，支持 CSS 语法。可以在这里修改颜色、字体和边框等。
    // Stylesheet definition using CSS syntax. Modify colors, fonts, borders here.
    QString qss = R"(
        /* 全局窗口 Global Window */
        QMainWindow {
            background-color: #f4f6f9;
            font-family: "Microsoft YaHei", "Segoe UI", "Roboto", "Helvetica Neue", Arial, sans-serif; /* Added YaHei for Chinese */
            color: #2c3e50;
        }

        /* 按钮 Buttons */
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-weight: 600;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #1f618d;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
            color: #7f8c8d;
        }

        /* 输入框 Input Fields */
        QLineEdit, QSpinBox, QComboBox {
            background-color: white;
            border: 2px solid #dfe6e9;
            border-radius: 6px;
            padding: 8px;
            font-size: 14px;
            color: #2c3e50;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border: 2px solid #3498db;
        }

        /* 列表控件 List Widgets */
        QListWidget {
            background-color: white;
            border: 2px solid #dfe6e9;
            border-radius: 6px;
            padding: 5px;
            font-size: 14px;
            outline: none;
        }
        QListWidget::item {
            padding: 10px;
            border-radius: 4px;
            margin-bottom: 2px;
            color: #2c3e50;
        }
        QListWidget::item:selected {
            background-color: #e1f0fa;
            color: #2c3e50;
            border: 1px solid #aed6f1;
        }
        QListWidget::item:hover {
            background-color: #f7f9f9;
        }

        /* 标签 Labels */
        QLabel {
            color: #2c3e50;
            font-size: 14px;
        }

        /* 单选按钮 Radio Buttons */
        QRadioButton {
            font-size: 15px;
            padding: 5px;
        }
        QRadioButton::indicator {
            width: 18px;
            height: 18px;
        }
    )";
    this->setStyleSheet(qss);
}

void MainWindow::setupUI() {
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // Module 1
    mainStack->addWidget(createStartPage());

    // Module 2
    mainStack->addWidget(createSlideshowPage());

    // Module 3
    mainStack->addWidget(createQuizPage());

    // Module 4
    mainStack->addWidget(createRPGPage());
}

// --- Utils ---

QPixmap MainWindow::generatePlaceholder(QString text, QColor color, QSize size) {
    // [自定义说明]
    // 这是一个生成纯色占位图片的辅助函数。
    // 如果您想使用真实的图片文件（如 png, jpg），请参考以下方法替换代码：
    // This is a helper to generate solid color placeholder images.
    // To use real image files (png, jpg), replace the code like this:
    /*
       QPixmap pixmap(":/images/background.jpg"); // 使用资源文件 Use resource file
       // 或者 OR
       QPixmap pixmap("C:/path/to/your/image.png"); // 使用绝对路径 Use absolute path

       if (pixmap.isNull()) {
           qDebug() << "Failed to load image";
           // Fallback to placeholder
       }
       return pixmap.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    */

    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold)); // Use YaHei
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}

void MainWindow::logAction(QString action) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, action);
    gameState.logs.append(logEntry);
    qDebug() << logEntry;
}

// --- Module 1: Start Page ---

QWidget *MainWindow::createStartPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("学生信息登记"); // Student Info
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(title);

    QFormLayout *form = new QFormLayout();
    nameInput = new QLineEdit();
    ageInput = new QSpinBox(); ageInput->setRange(16, 100);
    genderInput = new QComboBox(); genderInput->addItems({"男", "女", "其他"}); // Male, Female, Other
    classInput = new QLineEdit();
    durationInput = new QLineEdit();

    form->addRow("姓名:", nameInput); // Name
    form->addRow("年龄:", ageInput); // Age
    form->addRow("性别:", genderInput); // Gender
    form->addRow("班级:", classInput); // Class
    form->addRow("时长:", durationInput); // Duration

    QWidget *formWidget = new QWidget();
    formWidget->setLayout(form);
    formWidget->setFixedWidth(400);
    layout->addWidget(formWidget);

    QPushButton *startBtn = new QPushButton("开始培训"); // Start Training
    startBtn->setFixedWidth(200);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);

    return page;
}

void MainWindow::onStartTraining() {
    if (nameInput->text().isEmpty() || classInput->text().isEmpty()) {
        QMessageBox::warning(this, "验证失败", "请填写所有字段。"); // Validation, Please fill in all fields
        return;
    }

    student.name = nameInput->text();
    student.age = ageInput->value();
    student.gender = genderInput->currentText();
    student.className = classInput->text();
    student.duration = durationInput->text();

    logAction("培训开始: " + student.name); // Training Started for
    mainStack->setCurrentIndex(1); // Go to Slideshow
}

// --- Module 2: Slideshow ---

QWidget *MainWindow::createSlideshowPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    slideshowContainer = new QWidget();
    QVBoxLayout *slideLayout = new QVBoxLayout(slideshowContainer);

    slideImageLabel = new QLabel();
    slideImageLabel->setAlignment(Qt::AlignCenter);
    slideLayout->addWidget(slideImageLabel);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton("上一页"); // Previous
    QPushButton *nextBtn = new QPushButton("下一页"); // Next
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(nextBtn);
    slideLayout->addLayout(navLayout);

    connect(prevBtn, &QPushButton::clicked, [this]() {
        if (currentSlideIndex > 0) {
            currentSlideIndex--;
            updateSlide();
        }
    });
    connect(nextBtn, &QPushButton::clicked, [this]() {
        if (currentSlideIndex < totalSlides - 1) {
            currentSlideIndex++;
            updateSlide();
        } else {
            finishSlideshow();
        }
    });

    layout->addWidget(slideshowContainer);

    // Summary Widget (Initially Hidden)
    slideshowSummaryWidget = new QWidget();
    slideshowSummaryWidget->setVisible(false);
    layout->addWidget(slideshowSummaryWidget);
    
    updateSlide();
    return page;
}

void MainWindow::updateSlide() {
    // [自定义说明] 这里加载幻灯片图片。可以修改 "Slide %1" 为您的图片路径逻辑。
    // [Customization] Load slide images here.
    slideImageLabel->setPixmap(generatePlaceholder(
        QString("幻灯片 %1").arg(currentSlideIndex + 1), // Slide
        Qt::blue, 
        QSize(800, 450)
    ));
}

void MainWindow::finishSlideshow() {
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    for (int i = 0; i < totalSlides; ++i) {
        QLabel *thumb = new QLabel();
        thumb->setPixmap(generatePlaceholder(
            QString("图 %1").arg(i + 1), // Img
            Qt::gray, 
            QSize(200, 150)
        ));
        grid->addWidget(thumb, i / 5, i % 5);
    }

    QPushButton *nextModuleBtn = new QPushButton("进入测验"); // Go to Quiz
    connect(nextModuleBtn, &QPushButton::clicked, [this]() {
        mainStack->setCurrentIndex(2);
    });
    grid->addWidget(nextModuleBtn, 2, 0, 1, 5, Qt::AlignCenter);
}

// --- Module 3: Quiz ---

QWidget *MainWindow::createQuizPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // Generate Questions
    for (int i = 1; i <= 19; ++i) {
        Question q;
        q.text = QString("问题 %1: 某某操作的正确流程是...?").arg(i); // Question X: What is the correct procedure for...?
        q.options = QStringList() << "选项 A" << "选项 B" << "选项 C" << "选项 D"; // Option A/B/C/D
        q.correctIndex = 1; // Always B for simplicity in this demo
        questions.append(q);
    }

    questionLabel = new QLabel();
    questionLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    questionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(questionLabel);

    // Image for question
    QLabel *qImage = new QLabel();
    qImage->setPixmap(generatePlaceholder("问题示例图", Qt::darkCyan, QSize(400, 200))); // Question Image
    qImage->setAlignment(Qt::AlignCenter);
    layout->addWidget(qImage);

    optionGroup = new QButtonGroup(this);
    QWidget *optionsWidget = new QWidget();
    QVBoxLayout *optsLayout = new QVBoxLayout(optionsWidget);
    for (int i = 0; i < 4; ++i) {
        options[i] = new QRadioButton();
        optionGroup->addButton(options[i], i);
        optsLayout->addWidget(options[i]);
    }
    layout->addWidget(optionsWidget);

    feedbackLabel = new QLabel("");
    feedbackLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    layout->addWidget(feedbackLabel);

    QPushButton *checkBtn = new QPushButton("检查答案"); // Check Answer
    connect(checkBtn, &QPushButton::clicked, this, &MainWindow::checkAnswer);
    layout->addWidget(checkBtn);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevQBtn = new QPushButton("上一题"); // Previous Question
    QPushButton *nextQBtn = new QPushButton("下一题"); // Next Question
    navLayout->addWidget(prevQBtn);
    navLayout->addWidget(nextQBtn);
    layout->addLayout(navLayout);

    scoreLabel = new QLabel("得分: 0"); // Score
    layout->addWidget(scoreLabel);

    connect(prevQBtn, &QPushButton::clicked, [this]() {
        if (currentQuestionIndex > 0) {
            currentQuestionIndex--;
            loadQuestion();
        }
    });

    connect(nextQBtn, &QPushButton::clicked, [this]() {
        if (currentQuestionIndex < questions.size() - 1) {
            currentQuestionIndex++;
            loadQuestion();
        } else {
             // Finish Quiz
             QMessageBox::information(this, "测验结束", QString("最终得分: %1").arg(quizScore)); // Quiz Finished, Final Score
             mainStack->setCurrentIndex(3); // Go to RPG
             logAction("测验结束。得分: " + QString::number(quizScore));
        }
    });

    loadQuestion();
    return page;
}

void MainWindow::loadQuestion() {
    Question &q = questions[currentQuestionIndex];
    questionLabel->setText(q.text);
    for (int i = 0; i < 4; ++i) {
        options[i]->setText(q.options[i]);
        options[i]->setChecked(false);
    }
    feedbackLabel->setText("");
    // Reset radio buttons state if needed, but simple checkAnswer logic is enough
}

void MainWindow::checkAnswer() {
    int id = optionGroup->checkedId();
    if (id == -1) return;

    if (id == questions[currentQuestionIndex].correctIndex) {
        feedbackLabel->setText("回答正确!"); // Correct!
        feedbackLabel->setStyleSheet("color: green; font-weight: bold;");
        // Simple scoring: only add if not already answered correctly? 
        // For simplicity, just increment and don't worry about re-answering
        quizScore++; 
        scoreLabel->setText("得分: " + QString::number(quizScore));
    } else {
        feedbackLabel->setText("回答错误!"); // Incorrect!
        feedbackLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

// --- Module 4: RPG Simulation ---

QWidget *MainWindow::createRPGPage() {
    QWidget *page = new QWidget();
    // Layout: Left (15%), Center (70%), Right (15%)
    // Since we need absolute positioning for Center, we can still use HBox for the main columns to keep it clean,
    // or just absolute positioning for everything. 
    // Requirement says: "Hardcoded Absolute Positioning: Do not use QVBoxLayout or QHBoxLayout for the game scenes."
    // This implies the Center viewport must use absolute positioning.
    
    // I will use a QHBoxLayout for the high-level 3 panels to respect the "Left/Center/Right" structure easily,
    // but the content of the Center panel will be purely absolute positioned.
    
    QHBoxLayout *mainLayout = new QHBoxLayout(page);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    // Left Panel
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(192); // ~15% of 1280
    // Modern Dark Blue Sidebar
    leftPanel->setStyleSheet("background-color: #2c3e50; color: white; border-right: 1px solid #1a252f;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    locationLabel = new QLabel("当前位置: 入口"); // Location: Entrance
    locationLabel->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    cartStatusLabel = new QLabel("工作车: 0/10"); // Cart
    cartStatusLabel->setStyleSheet("color: #bdc3c7;");
    leftLayout->addWidget(locationLabel);
    leftLayout->addWidget(cartStatusLabel);
    leftLayout->addStretch();
    mainLayout->addWidget(leftPanel);

    // Center Panel
    rpgCenterPanel = new QWidget();
    rpgCenterPanel->setFixedSize(896, 720); // ~70%
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;"); // Soft Gray/White Canvas
    // No layout for rpgCenterPanel, we will use setGeometry in renderScene()
    mainLayout->addWidget(rpgCenterPanel);

    // Right Panel
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(192); // ~15%
    // Slightly lighter dark sidebar for hierarchy
    rightPanel->setStyleSheet("background-color: #34495e; color: white; border-left: 1px solid #1a252f;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QLabel *taskTitle = new QLabel("当前任务:"); // Tasks
    taskTitle->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    rightLayout->addWidget(taskTitle);

    taskListWidget = new QListWidget();
    // Inherits global list style, but we ensure text is visible against white list bg
    rightLayout->addWidget(taskListWidget);
    
    QLabel *invTitle = new QLabel("库存 (拖拽使用):"); // Inventory (Drag)
    invTitle->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    rightLayout->addWidget(invTitle);
    
    // Use the custom DraggableListWidget
    inventoryListWidget = new DraggableListWidget();
    rightLayout->addWidget(inventoryListWidget);
    
    mainLayout->addWidget(rightPanel);

    // Initialize State
    gameState.currentScene = GameScene::Entrance;
    gameState.currentFloor = 0;
    gameState.hasClockedIn = false;
    gameState.inventory.cartCapacity = 10;

    renderScene();

    return page;
}

void MainWindow::goToScene(GameScene scene) {
    gameState.currentScene = scene;
    logAction("移动到场景: " + QString::number((int)scene)); // Moved to scene
    renderScene();
}

void MainWindow::renderScene() {
    // Clear previous widgets in center panel safely
    // We copy the children list to avoid iterator invalidation
    // We use deleteLater() to avoid deleting the sender object while it's executing a slot
    QList<QObject*> children = rpgCenterPanel->children();
    for (QObject *child : children) {
        if (child->isWidgetType()) {
            static_cast<QWidget*>(child)->hide();
        }
        child->deleteLater();
    }

    switch(gameState.currentScene) {
        case GameScene::Entrance: renderEntrance(); break;
        case GameScene::StaffHallway: renderStaffHallway(); break;
        case GameScene::Office: renderOffice(); break;
        case GameScene::Warehouse: renderWarehouse(); break;
        case GameScene::ElevatorHall: renderElevatorHall(); break;
        case GameScene::ElevatorInside: renderElevatorInside(); break;
        case GameScene::FloorCorridor: renderFloorCorridor(); break;
        case GameScene::LinenRoom: renderLinenRoom(); break;
    }
    updateRPGStatusLabels();
}

void MainWindow::updateRPGStatusLabels() {
    // Update Location Label
    QString locStr;
    switch(gameState.currentScene) {
        case GameScene::Entrance: locStr = "入口"; break; // Entrance
        case GameScene::StaffHallway: locStr = "员工通道"; break; // Staff Hallway
        case GameScene::Office: locStr = "办公室"; break; // Office
        case GameScene::Warehouse: locStr = "布草仓库"; break; // Warehouse
        case GameScene::ElevatorHall: locStr = "电梯厅"; break; // Elevator Hall
        case GameScene::ElevatorInside: locStr = "电梯内"; break; // Elevator
        case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break; // Floor X Corridor
        case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break; // Floor X Linen Room
    }
    locationLabel->setText("当前位置: " + locStr);

    // Update Cart
    int count = 0;
    for (auto val : gameState.inventory.currentItems) count += val;
    cartStatusLabel->setText(QString("工作车: %1/%2").arg(count).arg(gameState.inventory.cartCapacity));
}

// --- Specific Scene Renderers ---

// [自定义说明] 场景渲染函数
// 每个 renderX 函数负责绘制一个场景。
// 使用 setGeometry(x, y, width, height) 绝对定位来放置按钮和背景。
// 坐标系以 rpgCenterPanel 左上角 (0,0) 为原点，最大尺寸 896 x 720。
// [Customization] Scene Render Functions
// Each function draws a scene.
// Uses setGeometry(x, y, width, height) for absolute positioning.
// Origin (0,0) is top-left of rpgCenterPanel. Max size 896 x 720.

void MainWindow::renderEntrance() {
    // Background
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size())); // Entrance
    bg->setGeometry(0, 0, 896, 720);

    // Input Name -> Auto transition
    // Requirement: "Input Name -> Auto-transition to Hallway"
    // Since we already input name in Module 1, we can just display a button to enter or auto enter.
    // Let's make a button "Enter Hotel".
    QPushButton *btn = new QPushButton("进入酒店", rpgCenterPanel); // Enter Hotel
    btn->setGeometry(350, 300, 200, 50); // x=350, y=300, w=200, h=50
    connect(btn, &QPushButton::clicked, [this]() {
        goToScene(GameScene::StaffHallway);
    });
    btn->show();
}

void MainWindow::renderStaffHallway() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size())); // Staff Hallway
    bg->setGeometry(0, 0, 896, 720);

    if (!gameState.hasClockedIn) {
        QPushButton *clockInBtn = new QPushButton("打卡签到", rpgCenterPanel); // Clock In
        clockInBtn->setGeometry(100, 100, 150, 50);
        connect(clockInBtn, &QPushButton::clicked, this, &MainWindow::handleClockIn);
        clockInBtn->show();
    } else {
        // Links to Office, Warehouse, Elevator Hall
        QPushButton *officeBtn = new QPushButton("去办公室", rpgCenterPanel); // Go to Office
        officeBtn->setGeometry(100, 200, 150, 50);
        connect(officeBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Office); });
        officeBtn->show();

        QPushButton *warehouseBtn = new QPushButton("去仓库", rpgCenterPanel); // Go to Warehouse
        warehouseBtn->setGeometry(300, 200, 150, 50);
        connect(warehouseBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Warehouse); });
        warehouseBtn->show();

        QPushButton *elevatorBtn = new QPushButton("去电梯", rpgCenterPanel); // Go to Elevator
        elevatorBtn->setGeometry(500, 200, 150, 50);
        connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
        elevatorBtn->show();
    }
}

void MainWindow::handleClockIn() {
    QMessageBox::information(this, "通知", "打卡成功!"); // Info, Clocked In Successfully
    gameState.hasClockedIn = true;
    logAction("已打卡");
    renderScene(); // Refresh to show navigation buttons
}

void MainWindow::renderOffice() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size())); // Office
    bg->setGeometry(0, 0, 896, 720);

    QPushButton *getTaskBtn = new QPushButton("领取任务", rpgCenterPanel); // Get Task
    getTaskBtn->setGeometry(100, 100, 150, 50);
    connect(getTaskBtn, &QPushButton::clicked, this, &MainWindow::handleGetTask);
    getTaskBtn->show();

    QPushButton *clockOutBtn = new QPushButton("打卡下班", rpgCenterPanel); // Clock Out
    clockOutBtn->setGeometry(300, 100, 150, 50);
    connect(clockOutBtn, &QPushButton::clicked, this, &MainWindow::handleClockOut);
    clockOutBtn->show();

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel); // Back to Hallway
    backBtn->setGeometry(100, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void MainWindow::handleGetTask() {
    Task t;
    // Random floor 6 or 7
    t.targetFloor = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
    t.isEmergency = false;
    t.isCompleted = false;
    
    // Items
    t.requiredItems.insert("毛巾", QRandomGenerator::global()->bounded(1, 4)); // Towel
    t.requiredItems.insert("床单", QRandomGenerator::global()->bounded(1, 4)); // Sheet
    
    gameState.tasks.append(t);
    refreshTaskList();
    logAction(QString("领取任务: %1楼").arg(t.targetFloor));
}

void MainWindow::handleClockOut() {
    logAction("已打卡下班");
    
    QString filename = QString("training_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << "Student: " << student.name << "\n";
        out << "Logs:\n";
        for (const QString &log : gameState.logs) {
            out << log << "\n";
        }
        file.close();
        QMessageBox::information(this, "再见", "培训日志已保存。正在退出..."); // Goodbye
        QApplication::quit();
    }
}

void MainWindow::renderWarehouse() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("仓库 (货架)", Qt::darkYellow, rpgCenterPanel->size())); // Warehouse (Shelves)
    bg->setGeometry(0, 0, 896, 720);

    // Shelves with items
    QPushButton *towelBtn = new QPushButton("拿取毛巾", rpgCenterPanel); // Take Towel
    towelBtn->setGeometry(100, 200, 100, 100);
    // Overlay transparent button on image technically, but here just a button
    connect(towelBtn, &QPushButton::clicked, [this]() { handleWarehouseItemClick("毛巾"); }); // Towel
    towelBtn->show();

    QPushButton *sheetBtn = new QPushButton("拿取床单", rpgCenterPanel); // Take Sheet
    sheetBtn->setGeometry(250, 200, 100, 100);
    connect(sheetBtn, &QPushButton::clicked, [this]() { handleWarehouseItemClick("床单"); }); // Sheet
    sheetBtn->show();

    QPushButton *loadCartBtn = new QPushButton("装车 (确认)", rpgCenterPanel); // Load Cart (Confirm)
    loadCartBtn->setGeometry(500, 500, 200, 50);
    connect(loadCartBtn, &QPushButton::clicked, this, &MainWindow::handleLoadCart);
    loadCartBtn->show();

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel); // Back to Hallway
    backBtn->setGeometry(50, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void MainWindow::handleWarehouseItemClick(QString itemName) {
    int currentCount = 0;
    for (auto val : gameState.inventory.currentItems) currentCount += val;
    
    if (currentCount < gameState.inventory.cartCapacity) {
        gameState.inventory.currentItems[itemName]++;
        logAction("捡起 " + itemName);
        updateRPGStatusLabels();
        // We don't update inventory list yet, we do that on "Load Cart" based on requirements logic or maybe immediately?
        // Prompt says: "Click item -> Add to Inventory -> Update "Cart" display. Logic: Button "Load Cart" confirms selection."
        // This implies visual update of cart display is immediate, but maybe inventory list update is later? 
        // Or "Load Cart" just means "I'm done here". I'll update the visual list immediately for better UX.
        refreshInventoryList(); 
    } else {
        QMessageBox::warning(this, "已满", "工作车已满!"); // Full
    }
}

void MainWindow::handleLoadCart() {
    logAction("确认装车");
    goToScene(GameScene::StaffHallway);
}

void MainWindow::renderElevatorHall() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size())); // Elevator Hall
    bg->setGeometry(0, 0, 896, 720);

    // If floor is 0 (Staff), we can go to Hallway. If 6 or 7, we can go to Floor Corridor.
    
    QPushButton *callElevator = new QPushButton("进入电梯", rpgCenterPanel); // Enter Elevator
    callElevator->setGeometry(350, 300, 200, 100);
    connect(callElevator, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorInside); });
    callElevator->show();

    QPushButton *backBtn = new QPushButton("返回", rpgCenterPanel); // Back
    backBtn->setGeometry(100, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() {
        if (gameState.currentFloor == 0) goToScene(GameScene::StaffHallway);
        else goToScene(GameScene::FloorCorridor);
    });
    backBtn->show();
}

void MainWindow::renderElevatorInside() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size())); // Elevator Inside
    bg->setGeometry(0, 0, 896, 720);

    // Buttons for G, 6, 7
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

void MainWindow::handleElevatorButton(int floor) {
    logAction(QString("电梯前往 %1 楼").arg(floor));
    // Animation/Timer simulation
    QTimer::singleShot(1000, this, [this, floor]() {
        gameState.currentFloor = floor;
        logAction(QString("抵达 %1 楼").arg(floor));
        goToScene(GameScene::ElevatorHall);
        
        // Check for emergency task trigger if arriving at a floor
        // "After delivering 1st floor items..." - Simplified logic: Random chance on arrival
        // checkEmergencyTask(); // Call this maybe after completion of a task, or randomly here.
    });
}

void MainWindow::renderFloorCorridor() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder(QString("%1楼 走廊").arg(gameState.currentFloor), Qt::cyan, rpgCenterPanel->size())); // Corridor Floor X
    bg->setGeometry(0, 0, 896, 720);

    QPushButton *linenRoomBtn = new QPushButton("布草间", rpgCenterPanel); // Linen Room
    linenRoomBtn->setGeometry(200, 200, 200, 100);
    connect(linenRoomBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::LinenRoom); });
    linenRoomBtn->show();

    QPushButton *elevatorBtn = new QPushButton("电梯厅", rpgCenterPanel); // Elevator Hall
    elevatorBtn->setGeometry(500, 200, 200, 100);
    connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
    elevatorBtn->show();
}

void MainWindow::renderLinenRoom() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    bg->setPixmap(generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size())); // Linen Room
    bg->setGeometry(0, 0, 896, 720);

    // Persist/Initialize dirty bag state for this floor
    bool dirtyBagPresent = false;
    if (!gameState.dirtyBagState.contains(gameState.currentFloor)) {
        // First time visiting this floor, decide random chance (30%)
         if (QRandomGenerator::global()->bounded(100) < 30) {
            gameState.dirtyBagState[gameState.currentFloor] = true;
         } else {
            gameState.dirtyBagState[gameState.currentFloor] = false;
         }
    }
    dirtyBagPresent = gameState.dirtyBagState[gameState.currentFloor];

    // Dirty Bag Widget
    QLabel *dirtyBag = nullptr;
    if (dirtyBagPresent) {
        dirtyBag = new QLabel(rpgCenterPanel);
        dirtyBag->setPixmap(generatePlaceholder("脏布草袋", Qt::darkRed, QSize(100, 100))); // Dirty Bag
        dirtyBag->setGeometry(100, 400, 100, 100);
        dirtyBag->show();
        
        QPushButton *removeBtn = new QPushButton("移走脏布草袋", rpgCenterPanel); // Remove Dirty Bag
        removeBtn->setGeometry(100, 510, 140, 40); // Slightly larger
        removeBtn->setStyleSheet("background-color: #e74c3c; color: white; border-radius: 4px;");
        
        // Use a pointer to self to capture by value safely or access via member
        connect(removeBtn, &QPushButton::clicked, [this, dirtyBag, removeBtn]() {
            dirtyBag->hide();
            removeBtn->hide();
            
            // Update State
            gameState.dirtyBagState[gameState.currentFloor] = false;
            logAction("移走脏布草袋");
            
            // Re-render scene to update shelf status (enable it)
            renderScene();
        });
        removeBtn->show();
    }

    // Shelf Drop Target
    DropLabel *shelf = new DropLabel("将布草拖到此处\n(货架)", rpgCenterPanel); // Drop Linens Here (Shelf)
    shelf->setGeometry(400, 200, 200, 300);
    
    // Logic: Disable shelf if dirty bag is present
    if (dirtyBagPresent) {
        shelf->setText("先移走脏布草袋!"); // Remove Dirty Bag First!
        // Update to match new style but with error colors
        shelf->setStyleSheet(
            "border: 2px dashed #e74c3c;"
            "border-radius: 8px;"
            "background-color: #fadbd8;"
            "color: #c0392b;"
            "font-weight: bold;"
            "font-size: 14px;"
        );
        shelf->setAcceptDrops(false); // Disable drops
    } else {
        shelf->onDropCallback = [this](QString item) {
            handleLinenDrop(item);
        };
    }
    shelf->show();

    QPushButton *backBtn = new QPushButton("返回走廊", rpgCenterPanel); // Back to Corridor
    backBtn->setGeometry(50, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::FloorCorridor); });
    backBtn->show();
}

void MainWindow::handleLinenDrop(QString itemName) {
    // Validate Item Type match against tasks for current floor
    bool taskFound = false;
    bool needed = false;

    // Find task for current floor
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        Task &t = gameState.tasks[i];
        if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
            taskFound = true;
            if (t.requiredItems.contains(itemName) && t.requiredItems[itemName] > 0) {
                needed = true;
                // Decrement inventory
                if (gameState.inventory.currentItems[itemName] > 0) {
                    gameState.inventory.currentItems[itemName]--;
                    // Decrement task requirement
                    t.requiredItems[itemName]--;
                    
                    logAction("配送 " + itemName + " 到 " + QString::number(gameState.currentFloor) + "楼"); // Delivered ... to Floor ...
                    
                    // Check if task completed
                    bool allDone = true;
                    for (auto count : t.requiredItems) {
                        if (count > 0) allDone = false;
                    }
                    if (allDone) {
                        t.isCompleted = true;
                        logAction("任务完成: " + QString::number(gameState.currentFloor) + "楼");
                        checkEmergencyTask(); // Trigger emergency chance
                    }
                } else {
                    QMessageBox::warning(this, "错误", "库存中没有此物品!"); // Error
                }
            }
        }
    }

    if (!taskFound) {
         QMessageBox::warning(this, "提示", "本层没有任务。"); // Info, No task
    } else if (!needed) {
         QMessageBox::warning(this, "提示", "本层不需要此物品。"); // Item not needed
    }

    refreshInventoryList();
    refreshTaskList();
    updateRPGStatusLabels();
}

void MainWindow::checkEmergencyTask() {
    if (QRandomGenerator::global()->bounded(100) < 50) { // 50% chance
        QMessageBox::information(this, "紧急情况", "收到新的紧急请求!"); // Emergency
        Task t;
        t.targetFloor = (gameState.currentFloor == 6) ? 7 : 6;
        t.isEmergency = true;
        t.isCompleted = false;
        t.requiredItems.insert("毛巾", 2);
        gameState.tasks.append(t);
        refreshTaskList();
        logAction("创建紧急任务");
    }
}

void MainWindow::refreshInventoryList() {
    inventoryListWidget->clear();
    for (auto it = gameState.inventory.currentItems.begin(); it != gameState.inventory.currentItems.end(); ++it) {
        if (it.value() > 0) {
            QListWidgetItem *item = new QListWidgetItem(QString("%1 (%2)").arg(it.key()).arg(it.value()));
            // Store item type in data for drag
            item->setData(Qt::UserRole, it.key());
            inventoryListWidget->addItem(item);
        }
    }
}

void MainWindow::refreshTaskList() {
    taskListWidget->clear();
    for (const Task &t : gameState.tasks) {
        if (!t.isCompleted) {
            QString txt = QString("%1楼 %2\n").arg(t.targetFloor).arg(t.isEmergency ? "[紧急]" : ""); // Floor X [URGENT]
            for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
                if (it.value() > 0)
                    txt += QString("- %1: %2\n").arg(it.key()).arg(it.value());
            }
            taskListWidget->addItem(txt);
        }
    }
}
