#include "mainwindow.h"
#include <QApplication>

// --- Constructor & Setup ---

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Fixed Window Size
    setFixedSize(1280, 720);
    setWindowTitle("酒店管理学生实训系统");

    setupStyle(); // 应用主题
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupStyle() {
    // 样式表定义，支持 CSS 语法。可以在这里修改颜色、字体和边框等。
    QString qss = R"(
        /* 全局窗口 Global Window */
        QMainWindow {
            background-color: #f4f6f9;
            font-family: "Microsoft YaHei", "Segoe UI", "Roboto", "Helvetica Neue", Arial, sans-serif;
            color: #2c3e50;
        }

        /* 按钮 */
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

        /* 输入框 */
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

        /* 列表控件 */
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

        /* 标签 */
        QLabel {
            color: #2c3e50;
            font-size: 14px;
        }

        /* 单选按钮 */
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

    // Module 0: Info
    mainStack->addWidget(createStartPage());

    // Module 1: Main Menu
    mainStack->addWidget(createMainMenu());

    // Module 2: Slideshow
    mainStack->addWidget(createSlideshowPage());

    // Module 3: Quiz
    mainStack->addWidget(createQuizPage());

    // Module 4: RPG
    mainStack->addWidget(createRPGPage());
}

// --- Utils ---

QPixmap MainWindow::generatePlaceholder(QString text, QColor color, QSize size) {
    // [自定义说明]
    // 这是一个生成纯色占位图片的辅助函数。
    // 如果您想使用真实的图片文件（如 png, jpg），请参考以下方法替换代码：
    /*
       QPixmap pixmap(":/images/background.jpg"); // 使用资源文件
       // 或者
       QPixmap pixmap("C:/path/to/your/image.png"); // 使用绝对路径

       if (pixmap.isNull()) {
           qDebug() << "加载图片失败";
           // 回退到占位符
       }
       return pixmap.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    */

    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}

void MainWindow::logAction(QString action) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, action);
    gameState.logs.append(logEntry);
    qDebug() << logEntry;
}

// --- Module 0: Info Page ---

QWidget *MainWindow::createStartPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("学生信息登记");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(title);

    QFormLayout *form = new QFormLayout();
    nameInput = new QLineEdit();
    ageInput = new QSpinBox(); ageInput->setRange(16, 100);
    genderInput = new QComboBox(); genderInput->addItems({"男", "女", "其他"});
    classInput = new QLineEdit();
    durationInput = new QLineEdit();

    form->addRow("姓名:", nameInput);
    form->addRow("年龄:", ageInput);
    form->addRow("性别:", genderInput);
    form->addRow("班级:", classInput);
    form->addRow("时长:", durationInput);

    QWidget *formWidget = new QWidget();
    formWidget->setLayout(form);
    formWidget->setFixedWidth(400);
    layout->addWidget(formWidget);

    QPushButton *startBtn = new QPushButton("开始培训");
    startBtn->setFixedWidth(200);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);

    return page;
}

void MainWindow::onStartTraining() {
    if (nameInput->text().isEmpty() || classInput->text().isEmpty()) {
        QMessageBox::warning(this, "验证失败", "请填写所有字段。");
        return;
    }

    student.name = nameInput->text();
    student.age = ageInput->value();
    student.gender = genderInput->currentText();
    student.className = classInput->text();
    student.duration = durationInput->text();

    QString logInfo = QString("开始培训: 姓名: %1, 年龄: %2, 性别: %3, 班级: %4, 时长: %5")
                      .arg(student.name)
                      .arg(student.age)
                      .arg(student.gender)
                      .arg(student.className)
                      .arg(student.duration);
    logAction(logInfo);

    updateMainMenu();
    mainStack->setCurrentIndex(1); // Go to Main Menu
}

// --- Module 1: Main Menu ---

QWidget *MainWindow::createMainMenu() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    QLabel *title = new QLabel("请选择测试模块"); // Select Module
    title->setStyleSheet("font-size: 28px; font-weight: bold; margin-bottom: 20px;");
    layout->addWidget(title, 0, Qt::AlignCenter);

    mainMenuButtons.clear();

    // Test 1 Button
    QPushButton *btn1 = new QPushButton("测试 1: 业务学习 (幻灯片)");
    btn1->setFixedSize(400, 80);
    btn1->setStyleSheet("font-size: 18px;");
    connect(btn1, &QPushButton::clicked, [this](){
        mainStack->setCurrentIndex(2); // Slideshow
    });
    layout->addWidget(btn1, 0, Qt::AlignCenter);
    mainMenuButtons.append(btn1);

    // Test 2 Button
    QPushButton *btn2 = new QPushButton("测试 2: 知识测验");
    btn2->setFixedSize(400, 80);
    btn2->setStyleSheet("font-size: 18px;");
    connect(btn2, &QPushButton::clicked, [this](){
        mainStack->setCurrentIndex(3); // Quiz
    });
    layout->addWidget(btn2, 0, Qt::AlignCenter);
    mainMenuButtons.append(btn2);

    // Test 3 Button
    QPushButton *btn3 = new QPushButton("测试 3: 模拟实训 (RPG)");
    btn3->setFixedSize(400, 80);
    btn3->setStyleSheet("font-size: 18px;");
    connect(btn3, &QPushButton::clicked, [this](){
        mainStack->setCurrentIndex(4); // RPG
    });
    layout->addWidget(btn3, 0, Qt::AlignCenter);
    mainMenuButtons.append(btn3);

    updateMainMenu();
    return page;
}

void MainWindow::updateMainMenu() {
    for (int i = 0; i < mainMenuButtons.size(); ++i) {
        QPushButton *btn = mainMenuButtons[i];
        if (i < progressState) {
            btn->setEnabled(true);
            btn->setToolTip("");
        } else {
            btn->setEnabled(false);
            btn->setToolTip("请先完成前一个测试"); // Please finish previous test
        }
    }
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
    QPushButton *prevBtn = new QPushButton("上一页");
    QPushButton *nextBtn = new QPushButton("下一页");
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
    // Load slide images from source/Test1/
    QString imagePath = QString("source/Test1/fig%1.png").arg(currentSlideIndex + 1);
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull()) {
        slideImageLabel->setPixmap(pixmap.scaled(800, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // Fallback
        slideImageLabel->setPixmap(generatePlaceholder(
            QString("幻灯片 %1 (缺失: %2)").arg(currentSlideIndex + 1).arg(imagePath),
            Qt::blue,
            QSize(800, 450)
        ));
    }
}

void MainWindow::finishSlideshow() {
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    for (int i = 0; i < totalSlides; ++i) {
        QLabel *thumb = new QLabel();

        // Use actual source images
        QString imagePath = QString("source/Test1/fig%1.png").arg(i + 1);
        QPixmap pix(imagePath);
        if (pix.isNull()) {
             pix = generatePlaceholder(QString("图 %1").arg(i + 1), Qt::gray, QSize(200, 150));
        } else {
             pix = pix.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        thumb->setPixmap(pix);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setStyleSheet("border: 1px solid #ccc;");
        grid->addWidget(thumb, i / 5, i % 5);
    }

    QPushButton *finishBtn = new QPushButton("完成学习 (返回菜单)");
    connect(finishBtn, &QPushButton::clicked, [this]() {
        if (progressState < 2) progressState = 2; // Unlock Quiz
        updateMainMenu();
        mainStack->setCurrentIndex(1); // Back to Main Menu
    });
    grid->addWidget(finishBtn, 2, 0, 1, 5, Qt::AlignCenter);
}

// --- Module 3: Quiz ---

QWidget *MainWindow::createQuizPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    // Generate Questions with specific text and answers
    struct QuizItem { QString text; int correctIndex; };
    QList<QuizItem> quizData = {
        {"1.上午8:00上班，谁的工作态度正确？", 2}, // C
        {"2.开始工作前主管安排任务。谁的工作态度正确？", 3}, // D
        {"3.工作时，谁的工作方式正确？", 3}, // D
        {"4. 工作时，谁的工作方式正确？", 3}, // D
        {"5. 工作时，谁的工作方式正确？", 0}, // A
        {"6. 进行交接时，谁的工作方式正确？", 0}, // A
        {"7.发现床单破洞了，谁的工作方式正确？", 0}, // A
        {"8. 工作时，谁的工作方式正确？", 0}, // A
        {"9. 遇到了紧急任务，需要送到15楼，谁的工作方式正确？", 0}, // A
        {"10. 下午4:00汇报工作时，谁的方式正确？", 1}, // B
        {"11.推车轮子坏了，谁的处理方式正确？", 3}, // D
        {"12. 工作时，谁的工作方式正确？", 0}, // A
        {"13. 工作时，谁的工作方式正确？", 3}, // D
        {"14.地面湿滑，谁的处理方式正确？", 2}, // C
        {"15. 工作时，谁的工作方式正确？", 2}, // C
        {"16. 房间里的布草不够了，谁的工作方式正确？", 1}, // B
        {"17.和同事相处时，谁的做法错误？", 3}, // D
        {"18.被批评了，谁的态度是对的？", 1}, // B
        {"19.下午17:00下班了，谁做的是对的？", 0}  // A
    };

    for (const auto &item : quizData) {
        Question q;
        q.text = item.text;
        q.options = QStringList() << "选项 A" << "选项 B" << "选项 C" << "选项 D";
        q.correctIndex = item.correctIndex;
        questions.append(q);
    }

    questionLabel = new QLabel();
    questionLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    questionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(questionLabel);

    // Options Area
    optionGroup = new QButtonGroup(this);
    optionsContainer = new QWidget();
    QGridLayout *grid = new QGridLayout(optionsContainer);

    char optionChars[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < 4; ++i) {
        QWidget *optWidget = new QWidget();
        QVBoxLayout *optLayout = new QVBoxLayout(optWidget);

        // Thumbnail Button (Click to preview)
        optionImages[i] = new QPushButton();
        optionImages[i]->setFixedSize(200, 150); // Increased size (Task E)
        optionImages[i]->setFlat(true);
        optionImages[i]->setStyleSheet("border: 1px solid #ccc;");

        // Connect later in loadQuestion to specific path

        // Radio Button (Selection)
        optionRadios[i] = new QRadioButton(QString("选项 %1").arg(optionChars[i]));
        optionGroup->addButton(optionRadios[i], i);

        optLayout->addWidget(optionImages[i], 0, Qt::AlignCenter);
        optLayout->addWidget(optionRadios[i], 0, Qt::AlignCenter);

        grid->addWidget(optWidget, i / 2, i % 2); // 2x2 Grid
    }
    layout->addWidget(optionsContainer);

    QHBoxLayout *navLayout = new QHBoxLayout();

    QPushButton *prevQBtn = new QPushButton("上一题");
    QPushButton *nextQBtn = new QPushButton("下一题 (提交)");
    navLayout->addWidget(prevQBtn);
    navLayout->addWidget(nextQBtn);
    layout->addLayout(navLayout);

    scoreLabel = new QLabel("得分: 0");
    layout->addWidget(scoreLabel);

    connect(prevQBtn, &QPushButton::clicked, [this]() {
        if (currentQuestionIndex > 0) {
            currentQuestionIndex--;
            loadQuestion();
        }
    });

    connect(nextQBtn, &QPushButton::clicked, [this]() {
         checkAnswerAndNext();
    });

    loadQuestion();
    return page;
}

void MainWindow::loadQuestion() {
    Question &q = questions[currentQuestionIndex];
    questionLabel->setText(q.text);

    // Clear previous check
    optionGroup->setExclusive(false);
    for (int i = 0; i < 4; ++i) {
        optionRadios[i]->setChecked(false);
        optionRadios[i]->setText(q.options[i]);

        // Load Image: source/Test2/[QNum][A/B/C/D].jpg
        char suffix = 'A' + i;
        QString imgName = QString("%1%2").arg(currentQuestionIndex + 1).arg(suffix);

        // Try extensions jpg, png
        QString path = QString("source/Test2/%1.jpg").arg(imgName);
        if (!QFile::exists(path)) path = QString("source/Test2/%1.png").arg(imgName);

        QPixmap pix(path);
        if (pix.isNull()) {
            optionImages[i]->setIcon(QIcon());
            optionImages[i]->setText("(无图)");
        } else {
            optionImages[i]->setText("");
            optionImages[i]->setIcon(QIcon(pix));
            optionImages[i]->setIconSize(QSize(190, 140)); // Adjusted for larger button
        }

        // Disconnect previous connections
        optionImages[i]->disconnect();
        // Connect to lightbox
        connect(optionImages[i], &QPushButton::clicked, [this, path]() {
            showImagePreview(path);
        });
    }
    optionGroup->setExclusive(true);
}

void MainWindow::checkAnswerAndNext() {
    int id = optionGroup->checkedId();

    // Validation: Must select an option (Task E)
    if (id == -1) {
        QMessageBox::warning(this, "提示", "请先选择一个选项！");
        return;
    }

    if (id == questions[currentQuestionIndex].correctIndex) {
        quizScore += 1;
    }

    scoreLabel->setText("得分: " + QString::number(quizScore));

    if (currentQuestionIndex < questions.size() - 1) {
        currentQuestionIndex++;
        loadQuestion();
    } else {
         QMessageBox::information(this, "测验结束", QString("最终得分: %1").arg(quizScore));
         if (progressState < 3) progressState = 3; // Unlock RPG
         updateMainMenu();
         mainStack->setCurrentIndex(1); // Back to Main Menu
         logAction("测验结束。得分: " + QString::number(quizScore));
    }
}

void MainWindow::showImagePreview(QString imagePath) {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("图片预览");
    dlg->resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(dlg);
    QScrollArea *scroll = new QScrollArea(dlg);
    QLabel *imgLbl = new QLabel();

    QPixmap pix(imagePath);
    if (!pix.isNull()) {
        imgLbl->setPixmap(pix);
        imgLbl->setScaledContents(false);
    } else {
        imgLbl->setText("无法加载图片");
    }

    scroll->setWidget(imgLbl);
    layout->addWidget(scroll);

    dlg->exec();
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
    locationLabel = new QLabel("当前位置: 入口");
    locationLabel->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    cartStatusLabel = new QLabel("工作车: 0/10");
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
    
    QLabel *taskTitle = new QLabel("当前任务:");
    taskTitle->setStyleSheet("font-weight: bold; color: #ecf0f1; margin-top: 10px;");
    rightLayout->addWidget(taskTitle);

    taskListWidget = new QListWidget();
    // Inherits global list style, but we ensure text is visible against white list bg
    rightLayout->addWidget(taskListWidget);
    
    QLabel *invTitle = new QLabel("库存 (拖拽使用):");
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
    gameState.hasReceivedTask = false;
    gameState.inventory.cartCapacity = 10;

    renderScene();

    return page;
}

void MainWindow::goToScene(GameScene scene) {
    gameState.currentScene = scene;
    logAction("移动到场景: " + QString::number((int)scene));
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
        case GameScene::Entrance: locStr = "入口"; break;
        case GameScene::StaffHallway: locStr = "员工通道"; break;
        case GameScene::Office: locStr = "办公室"; break;
        case GameScene::Warehouse: locStr = "布草仓库"; break;
        case GameScene::ElevatorHall: locStr = "电梯厅"; break;
        case GameScene::ElevatorInside: locStr = "电梯内"; break;
        case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break;
        case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break;
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
// [自定义说明] 场景渲染函数
// 每个 renderX 函数负责绘制一个场景。
// 使用 setGeometry(x, y, width, height) 绝对定位来放置按钮和背景。
// 坐标系以 rpgCenterPanel 左上角 (0,0) 为原点，最大尺寸 896 x 720。

void MainWindow::renderEntrance() {
    // 背景
    // 图片: source/Test3/入口.jpg
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix("source/Test3/入口.jpg");
    if (pix.isNull()) pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // 进入酒店按钮
    QPushButton *btn = new QPushButton("进入酒店", rpgCenterPanel);
    btn->setGeometry(350, 600, 200, 50);
    connect(btn, &QPushButton::clicked, [this]() {
        goToScene(GameScene::StaffHallway);
    });
    btn->show();
}

void MainWindow::renderStaffHallway() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // Image: source/Test3/员工通道走廊.jpg
    QPixmap pix("source/Test3/员工通道走廊.jpg");
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    if (!gameState.hasClockedIn) {
        QPushButton *clockInBtn = new QPushButton("打卡签到", rpgCenterPanel);
        clockInBtn->setGeometry(100, 100, 150, 50);
        connect(clockInBtn, &QPushButton::clicked, this, &MainWindow::handleClockIn);
        clockInBtn->show();
    } else {
        // 导航按钮
        // [坐标自定义] 调整 x,y,w,h 以匹配图片透视

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

void MainWindow::handleClockIn() {
    QMessageBox::information(this, "通知", "打卡成功!");
    gameState.hasClockedIn = true;
    logAction("已打卡");
    renderScene(); // 刷新以显示导航按钮
}

void MainWindow::renderOffice() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // Image: source/Test3/办公室.png
    QPixmap pix("source/Test3/办公室.png");
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::darkBlue, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // 领取任务按钮
    QPushButton *getTaskBtn = new QPushButton("领取任务", rpgCenterPanel);
    getTaskBtn->setGeometry(100, 100, 150, 50);

    // 逻辑：每此只能领取一次任务
    if (gameState.hasReceivedTask) {
        getTaskBtn->setEnabled(false);
        getTaskBtn->setText("任务已领取");
        getTaskBtn->setToolTip("一次只能领取一次任务");
    } else {
        connect(getTaskBtn, &QPushButton::clicked, this, &MainWindow::handleGetTask);
    }
    getTaskBtn->show();

    QPushButton *clockOutBtn = new QPushButton("打卡下班", rpgCenterPanel);
    clockOutBtn->setGeometry(300, 100, 150, 50);
    connect(clockOutBtn, &QPushButton::clicked, this, &MainWindow::handleClockOut);
    clockOutBtn->show();

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel);
    backBtn->setGeometry(100, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::StaffHallway); });
    backBtn->show();
}

void MainWindow::handleGetTask() {
    if (gameState.hasReceivedTask) return; // Double safety

    Task t;
    // 随机楼层 6 或 7
    t.targetFloor = (QRandomGenerator::global()->bounded(2) == 0) ? 6 : 7;
    t.isEmergency = false;
    t.isCompleted = false;
    
    // 物品
    t.requiredItems.insert("毛巾", QRandomGenerator::global()->bounded(1, 4));
    t.requiredItems.insert("床单", QRandomGenerator::global()->bounded(1, 4));
    
    gameState.tasks.append(t);
    gameState.hasReceivedTask = true; // 标记已领取

    refreshTaskList();
    logAction(QString("领取任务: %1楼").arg(t.targetFloor));

    // 刷新场景以禁用按钮
    renderScene();
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
        QMessageBox::information(this, "再见", "培训日志已保存。正在退出...");
        QApplication::quit();
    }
}

void MainWindow::renderWarehouse() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // 图片: source/Test3/仓库1.jpg
    QPixmap pix("source/Test3/仓库1.jpg");
    if (pix.isNull()) pix = generatePlaceholder("仓库 (货架)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // 货架物品
    // 在图片区域上方使用透明按钮
    QPushButton *towelBtn = new QPushButton("拿取毛巾", rpgCenterPanel);
    towelBtn->setGeometry(100, 200, 100, 100);
    // 样式化为半透明或纯文本
    towelBtn->setStyleSheet("background-color: rgba(255, 255, 255, 100); border: 1px solid white;");
    connect(towelBtn, &QPushButton::clicked, [this]() { handleWarehouseItemClick("毛巾"); });
    towelBtn->show();

    QPushButton *sheetBtn = new QPushButton("拿取床单", rpgCenterPanel);
    sheetBtn->setGeometry(250, 200, 100, 100);
    sheetBtn->setStyleSheet("background-color: rgba(255, 255, 255, 100); border: 1px solid white;");
    connect(sheetBtn, &QPushButton::clicked, [this]() { handleWarehouseItemClick("床单"); });
    sheetBtn->show();

    QPushButton *loadCartBtn = new QPushButton("装车 (确认)", rpgCenterPanel);
    loadCartBtn->setGeometry(500, 600, 200, 50);
    connect(loadCartBtn, &QPushButton::clicked, this, &MainWindow::handleLoadCart);
    loadCartBtn->show();

    QPushButton *backBtn = new QPushButton("返回通道", rpgCenterPanel);
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
        // 我们尚未更新库存列表，我们会基于需求逻辑在“装车”时进行，或者立即进行？
        // 提示说：“点击物品 -> 添加到库存 -> 更新‘工作车’显示。逻辑：‘装车’按钮确认选择。”
        // 这意味着工作车显示的视觉更新是立即的，但也允许库存列表稍后更新？
        // 或者“装车”仅仅意味着“我在这里完成了”。为了更好的用户体验，我将立即更新可视列表。
        refreshInventoryList(); 
    } else {
        QMessageBox::warning(this, "已满", "工作车已满!");
    }
}

void MainWindow::handleLoadCart() {
    logAction("确认装车");
    goToScene(GameScene::StaffHallway);
}

void MainWindow::renderElevatorHall() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // 图片: source/Test3/电梯厅.jpg
    QPixmap pix("source/Test3/电梯厅.jpg");
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // 如果楼层为 0（员工），我们可以去通道。如果是 6 或 7，我们可以去楼层走廊。
    
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

void MainWindow::renderElevatorInside() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // 图片: source/Test3/电梯内.jpg
    QPixmap pix("source/Test3/电梯内.jpg");
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // G, 6, 7 楼层按钮
    QMap<int, QString> floors;
    floors[0] = "G";
    floors[6] = "6";
    floors[7] = "7";

    int y = 100;
    for (auto it = floors.begin(); it != floors.end(); ++it) {
        int floor = it.key();
        QPushButton *btn = new QPushButton(it.value(), rpgCenterPanel);
        btn->setGeometry(400, y, 80, 80); // 根据需要调整 Y 以匹配面板
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
    // 图片: source/Test3/楼层走廊-前.png
    QPixmap pix("source/Test3/楼层走廊-前.png");
    if (pix.isNull()) pix = generatePlaceholder(QString("%1楼 走廊").arg(gameState.currentFloor), Qt::cyan, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    QPushButton *linenRoomBtn = new QPushButton("布草间", rpgCenterPanel);
    linenRoomBtn->setGeometry(200, 200, 200, 100);
    connect(linenRoomBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::LinenRoom); });
    linenRoomBtn->show();

    QPushButton *elevatorBtn = new QPushButton("电梯厅", rpgCenterPanel);
    elevatorBtn->setGeometry(500, 200, 200, 100);
    connect(elevatorBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::ElevatorHall); });
    elevatorBtn->show();
}

void MainWindow::renderLinenRoom() {
    QLabel *bg = new QLabel(rpgCenterPanel);
    // 图片: source/Test3/布草间-空.jpg
    QPixmap pix("source/Test3/布草间-空.jpg");
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setGeometry(0, 0, 896, 720);

    // 持久化/初始化该楼层的脏布草袋状态
    bool dirtyBagPresent = false;
    if (!gameState.dirtyBagState.contains(gameState.currentFloor)) {
        // 第一次访问此楼层，决定随机几率 (30%)
         if (QRandomGenerator::global()->bounded(100) < 30) {
            gameState.dirtyBagState[gameState.currentFloor] = true;
         } else {
            gameState.dirtyBagState[gameState.currentFloor] = false;
         }
    }
    dirtyBagPresent = gameState.dirtyBagState[gameState.currentFloor];

    // 脏布草袋控件
    QLabel *dirtyBag = nullptr;
    if (dirtyBagPresent) {
        dirtyBag = new QLabel(rpgCenterPanel);
        // 图片: source/Test3/脏布草.jpg (或布草袋图片)
        QPixmap bagPix("source/Test3/脏布草.jpg");
        if (bagPix.isNull()) bagPix = generatePlaceholder("脏布草袋", Qt::darkRed, QSize(100, 100));
        dirtyBag->setPixmap(bagPix.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        dirtyBag->setGeometry(100, 400, 100, 100);
        dirtyBag->show();
        
        QPushButton *removeBtn = new QPushButton("移走脏布草袋", rpgCenterPanel);
        removeBtn->setGeometry(100, 510, 140, 40); // 稍大一点
        removeBtn->setStyleSheet("background-color: #e74c3c; color: white; border-radius: 4px;");
        
        // 使用指向自身的指针安全地按值捕获或通过成员访问
        connect(removeBtn, &QPushButton::clicked, [this, dirtyBag, removeBtn]() {
            dirtyBag->hide();
            removeBtn->hide();
            
            // 更新状态
            gameState.dirtyBagState[gameState.currentFloor] = false;
            logAction("移走脏布草袋");
            
            // 重新渲染场景以更新货架状态（启用它）
            renderScene();
        });
        removeBtn->show();
    }

    // 货架放置目标
    DropLabel *shelf = new DropLabel("将布草拖到此处\n(货架)", rpgCenterPanel);
    shelf->setGeometry(400, 200, 200, 300);
    
    // 逻辑：如果脏布草袋存在，禁用货架
    if (dirtyBagPresent) {
        shelf->setText("先移走脏布草袋!");
        // 更新以匹配新样式，但使用错误颜色
        shelf->setStyleSheet(
            "border: 2px dashed #e74c3c;"
            "border-radius: 8px;"
            "background-color: #fadbd8;"
            "color: #c0392b;"
            "font-weight: bold;"
            "font-size: 14px;"
        );
        shelf->setAcceptDrops(false); // 禁用拖放
    } else {
        shelf->onDropCallback = [this](QString item) {
            handleLinenDrop(item);
        };
    }
    shelf->show();

    QPushButton *backBtn = new QPushButton("返回走廊", rpgCenterPanel);
    backBtn->setGeometry(50, 600, 150, 50);
    connect(backBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::FloorCorridor); });
    backBtn->show();
}

void MainWindow::handleLinenDrop(QString itemName) {
    // 验证物品类型是否匹配当前楼层的任务
    bool taskFound = false;
    bool needed = false;

    // 查找当前楼层的任务
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        Task &t = gameState.tasks[i];
        if (t.targetFloor == gameState.currentFloor && !t.isCompleted) {
            taskFound = true;
            if (t.requiredItems.contains(itemName) && t.requiredItems[itemName] > 0) {
                needed = true;
                // 减少库存
                if (gameState.inventory.currentItems[itemName] > 0) {
                    gameState.inventory.currentItems[itemName]--;
                    // 减少任务需求
                    t.requiredItems[itemName]--;
                    
                    logAction("配送 " + itemName + " 到 " + QString::number(gameState.currentFloor) + "楼");
                    
                    // 检查任务是否完成
                    bool allDone = true;
                    for (auto count : t.requiredItems) {
                        if (count > 0) allDone = false;
                    }
                    if (allDone) {
                        t.isCompleted = true;
                        logAction("任务完成: " + QString::number(gameState.currentFloor) + "楼");
                        checkEmergencyTask(); // 触发紧急任务几率
                    }
                } else {
                    QMessageBox::warning(this, "错误", "库存中没有此物品!");
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
    updateRPGStatusLabels();
}

void MainWindow::checkEmergencyTask() {
    if (QRandomGenerator::global()->bounded(100) < 50) { // 50% chance
        QMessageBox::information(this, "紧急情况", "收到新的紧急请求!");
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
            QString txt = QString("%1楼 %2\n").arg(t.targetFloor).arg(t.isEmergency ? "[紧急]" : "");
            for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
                if (it.value() > 0)
                    txt += QString("- %1: %2\n").arg(it.key()).arg(it.value());
            }
            taskListWidget->addItem(txt);
        }
    }
}
