#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(1280, 720);
    setWindowTitle("酒店管理学生实训系统");
    setupStyle();
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupStyle() {
    QString qss = R"(
        QMainWindow { background-color: #f4f6f9; font-family: "Microsoft YaHei"; color: #2c3e50; }
        QPushButton { background-color: #3498db; color: white; border-radius: 6px; padding: 10px 20px; font-weight: 600; font-size: 14px; }
        QPushButton:hover { background-color: #2980b9; }
        QPushButton:disabled { background-color: #bdc3c7; color: #7f8c8d; }
        QLineEdit, QSpinBox, QComboBox { background-color: white; border: 2px solid #dfe6e9; border-radius: 6px; padding: 8px; }
        QListWidget { background-color: white; border: 2px solid #dfe6e9; border-radius: 6px; outline: none; }
        QListWidget::item { padding: 10px; color: #2c3e50; }
        QListWidget::item:selected { background-color: #e1f0fa; color: #2c3e50; }
    )";
    this->setStyleSheet(qss);
}

void MainWindow::setupUI() {
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // 0: Start
    mainStack->addWidget(createStartPage());

    // 1: Menu
    mainStack->addWidget(createMainMenu());

    // 2: Test 1
    test1Widget = new Test1();
    connect(test1Widget, &Test1::logMessage, this, &MainWindow::onLogMessage);
    connect(test1Widget, &Test1::levelCompleted, [this](){ onLevelCompleted(1); });
    mainStack->addWidget(test1Widget);

    // 3: Test 2
    test2Widget = new Test2();
    connect(test2Widget, &Test2::logMessage, this, &MainWindow::onLogMessage);
    connect(test2Widget, &Test2::levelCompleted, [this](){ onLevelCompleted(2); });
    mainStack->addWidget(test2Widget);

    // 4: Test 3
    test3Widget = new Test3(isDeveloperMode); // Pass dev mode
    connect(test3Widget, &Test3::logMessage, this, &MainWindow::onLogMessage);
    connect(test3Widget, &Test3::levelCompleted, [this](){ onLevelCompleted(3); });
    mainStack->addWidget(test3Widget);
}

// --- Start Page ---
QWidget *MainWindow::createStartPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);

    QLabel *title = new QLabel("学生基础信息登记");
    title->setStyleSheet("font-size: 24px; font-weight: bold;");
    layout->addWidget(title);

    QFormLayout *form = new QFormLayout();
    nameInput = new QLineEdit();
    ageInput = new QSpinBox(); ageInput->setRange(16, 100);
    genderInput = new QComboBox(); genderInput->addItems({"男", "女"});
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

    onLogMessage(QString("开始培训: %1, %2").arg(student.name, student.className));
    updateMainMenu();
    mainStack->setCurrentIndex(1);
}

// --- Main Menu ---
QWidget *MainWindow::createMainMenu() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    QLabel *title = new QLabel("请选择测试项");
    title->setStyleSheet("font-size: 28px; font-weight: bold;");
    layout->addWidget(title, 0, Qt::AlignCenter);

    auto addBtn = [&](QString text, int idx) {
        QPushButton *btn = new QPushButton(text);
        btn->setFixedSize(400, 80);
        btn->setStyleSheet("font-size: 18px;");
        connect(btn, &QPushButton::clicked, [this, idx](){ mainStack->setCurrentIndex(idx); });
        layout->addWidget(btn, 0, Qt::AlignCenter);
        mainMenuButtons.append(btn);
    };

    addBtn("测试 1: 业务学习 (幻灯片)", 2);
    addBtn("测试 2: 知识测验 (选择题)", 3);
    addBtn("测试 3: 模拟实训 (RPG)", 4);

    QCheckBox *devCheck = new QCheckBox("开发者模式");
    connect(devCheck, &QCheckBox::stateChanged, this, &MainWindow::toggleDeveloperMode);
    layout->addWidget(devCheck, 0, Qt::AlignCenter);

    return page;
}

void MainWindow::updateMainMenu() {
    for (int i = 0; i < mainMenuButtons.size(); ++i) {
        if (isDeveloperMode || i < progressState) {
            mainMenuButtons[i]->setEnabled(true);
            mainMenuButtons[i]->setToolTip("");
        } else {
            mainMenuButtons[i]->setEnabled(false);
            mainMenuButtons[i]->setToolTip("请先完成前一个测试");
        }
    }
}

void MainWindow::toggleDeveloperMode(int state) {
    isDeveloperMode = (state == Qt::Checked);
    // Propagate to Test3 if it exists
    if (test3Widget) test3Widget->setDeveloperMode(isDeveloperMode);
    updateMainMenu();
    onLogMessage(isDeveloperMode ? "开发者模式已开启" : "开发者模式已关闭");
}

// --- Logic ---

void MainWindow::onLogMessage(QString msg) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fullMsg = QString("[%1] %2").arg(timestamp, msg);
    
    // Immediate write to file
    QString filename = "training_log.txt"; // Use a single log file or rotate
    QFile file(filename);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << fullMsg << "\n";
        file.close();
    }
    qDebug() << fullMsg;
}

void MainWindow::onLevelCompleted(int level) {
    if (progressState < level + 1) {
        progressState = level + 1;
    }
    updateMainMenu();
    mainStack->setCurrentIndex(1); // Return to menu
    
    if (level == 3) {
        QMessageBox::information(this, "恭喜", "您已完成所有实训内容！");
        QApplication::quit();
    }
}
