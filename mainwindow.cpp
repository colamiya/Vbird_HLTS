#include "mainwindow.h"
#include <QFormLayout>
#include "config.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent, bool devModeDefault)
    : QMainWindow(parent)
{
    isDeveloperMode = devModeDefault;
    resize(Config::Global::WINDOW_SIZE);
    setMinimumSize(800, 600); // 设置最小窗口大小
    setWindowTitle(Config::Global::APP_TITLE);
    setupStyle();
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupStyle()
{
    // 加载全局样式表
    this->setStyleSheet(Config::Global::GLOBAL_STYLESHEET);
}

void MainWindow::setupUI()
{
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // 0: 开始页
    mainStack->addWidget(createStartPage());

    // 1: 主菜单
    mainStack->addWidget(createMainMenu());

    // 2: Test 1 (幻灯片)
    test1Widget = new Test1();
    connect(test1Widget, &Test1::logMessage, this, &MainWindow::onLogMessage);
    connect(test1Widget, &Test1::levelCompleted, [this]()
            { onLevelCompleted(1); });
    connect(test1Widget, &Test1::levelCancelled, [this]()
            { mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test1Widget);

    // 3: Test 2 (测验)
    test2Widget = new Test2();
    connect(test2Widget, &Test2::logMessage, this, &MainWindow::onLogMessage);
    connect(test2Widget, &Test2::levelCompleted, [this]()
            { onLevelCompleted(2); });
    connect(test2Widget, &Test2::levelCancelled, [this]()
            { mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test2Widget);

    // 4: Test 3 (RPG 实训)
    test3Widget = new Test3(isDeveloperMode); // 传递开发者模式状态
    connect(test3Widget, &Test3::logMessage, this, &MainWindow::onLogMessage);
    connect(test3Widget, &Test3::levelCompleted, [this]()
            { onLevelCompleted(3); });
    connect(test3Widget, &Test3::levelCancelled, [this]()
            { mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test3Widget);
}

// --- 开始页 (Start Page) ---
QWidget *MainWindow::createStartPage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    // 标题
    QLabel *title = new QLabel(Config::Global::TITLE_START_PAGE);
    title->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(Config::Global::FONT_SIZE_SUBTITLE));
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 0, Qt::AlignCenter);

    // 表单布局
    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignCenter);

    nameInput = new QLineEdit();

    ageInput = new QSpinBox();
    ageInput->setRange(16, 100);
    ageInput->setButtonSymbols(QAbstractSpinBox::NoButtons); // 移除加减按钮
    ageInput->setAlignment(Qt::AlignCenter);

    genderInput = new QComboBox();
    genderInput->addItems({"男", "女"});

    classInput = new QLineEdit();
    durationInput = new QLineEdit();

    form->addRow("姓名:", nameInput);
    form->addRow("年龄:", ageInput);
    form->addRow("性别:", genderInput);
    form->addRow("班级:", classInput);
    form->addRow("时长:", durationInput);

    QWidget *formWidget = new QWidget();
    formWidget->setLayout(form);
    formWidget->setFixedWidth(Config::Global::SIZE_FORM_WIDTH);

    layout->addWidget(formWidget, 0, Qt::AlignCenter);

    // 开始按钮
    QPushButton *startBtn = new QPushButton(Config::Global::BTN_TEXT_START);
    startBtn->setFixedWidth(Config::Global::SIZE_START_BTN_WIDTH);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);

    // 突发事件开关 (Test 3)
    emergencyToggle = new QCheckBox(Config::Global::CHECKBOX_TEXT_EMERGENCY);
    emergencyToggle->setChecked(false);
    emergencyToggle->setStyleSheet(QString("color: %1; font-size: 12px;").arg(Config::Global::COL_TEXT_DISABLED));

    QWidget *bottomContainer = new QWidget();
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomContainer);
    bottomLayout->addStretch();
    bottomLayout->addWidget(emergencyToggle);
    bottomLayout->addStretch();

    layout->addWidget(bottomContainer);

    return page;
}

void MainWindow::onStartTraining()
{
    if (nameInput->text().isEmpty() || classInput->text().isEmpty())
    {
        QMessageBox::warning(this, "验证失败", "请填写所有字段。");
        return;
    }
    student.name = nameInput->text();
    student.age = ageInput->value();
    student.gender = genderInput->currentText();
    student.className = classInput->text();
    student.duration = durationInput->text();
    enableEmergencyEvents = emergencyToggle->isChecked();

    // 初始化日志记录器 (Set info, then start session)
    Logger::instance().setStudentInfo(student);
    Logger::instance().startNewSession(); // Create timestamped files

    // 将配置传递给 Test3
    if (test3Widget)
    {
        test3Widget->setEmergencyMode(enableEmergencyEvents);
    }

    onLogMessage(QString("开始培训: %1, %2").arg(student.name, student.className));
    updateMainMenu();
    mainStack->setCurrentIndex(1);
}

void MainWindow::onEndTraining()
{
    Logger::instance().generateBriefReport();
    QMessageBox::information(this, "实训结束", "报表已生成。");
    close();
}

// --- 主菜单 (Main Menu) ---
QWidget *MainWindow::createMainMenu()
{
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    QLabel *title = new QLabel(Config::Global::TITLE_MAIN_MENU);
    title->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(Config::Global::FONT_SIZE_TITLE));
    layout->addWidget(title, 0, Qt::AlignCenter);

    // 动态添加按钮的 Lambda
    auto addBtn = [&](QString text, int idx)
    {
        QPushButton *btn = new QPushButton(text);
        btn->setFixedSize(Config::Global::SIZE_MENU_BTN);
        btn->setStyleSheet("font-size: 18px;");
        connect(btn, &QPushButton::clicked, [this, idx]()
                { mainStack->setCurrentIndex(idx); });
        layout->addWidget(btn, 0, Qt::AlignCenter);
        mainMenuButtons.append(btn);
    };

    addBtn(Config::Global::BTN_TEXT_TEST1, 2);
    addBtn(Config::Global::BTN_TEXT_TEST2, 3);
    addBtn(Config::Global::BTN_TEXT_TEST3, 4);

    // 添加结束训练按钮
    QPushButton *exitBtn = new QPushButton("结束训练");
    exitBtn->setFixedSize(Config::Global::SIZE_MENU_BTN);
    exitBtn->setStyleSheet("background-color: #e74c3c; font-size: 18px;");
    connect(exitBtn, &QPushButton::clicked, this, &MainWindow::onEndTraining);
    layout->addWidget(exitBtn, 0, Qt::AlignCenter);

    return page;
}

void MainWindow::updateMainMenu()
{
    // Task 4: Free selection logic (Unlock all)
    // "取消主界面的测试顺序限制，可以自由选择测试1 2 3"
    for (int i = 0; i < mainMenuButtons.size(); ++i)
    {
        mainMenuButtons[i]->setEnabled(true);
        mainMenuButtons[i]->setToolTip("");
    }
}

void MainWindow::toggleDeveloperMode(int state)
{
    isDeveloperMode = (state == Qt::Checked);
    // 传播到 Test3
    if (test3Widget)
        test3Widget->setDeveloperMode(isDeveloperMode);
    updateMainMenu();
    onLogMessage(isDeveloperMode ? "开发者模式已开启" : "开发者模式已关闭");
}

// --- 逻辑 (Logic) ---

void MainWindow::onLogMessage(QString msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fullMsg = QString("[%1] %2").arg(timestamp, msg);

    // 记录详细日志
    Logger::instance().logAction("Main/System", msg);

    // 已通过 Logger 记录，不再调用 qDebug 以避免文件日志重复
    // (qDebug output is redirected to the log file via qInstallMessageHandler)
}

void MainWindow::onLevelCompleted(int level)
{
    // 更新进度状态 (虽然现在自由选择，但保留逻辑无害)
    if (progressState < level + 1)
    {
        progressState = level + 1;
    }
    updateMainMenu();
    mainStack->setCurrentIndex(1); // 返回主菜单 (All tests return to Main Menu)

    if (level == 3)
    {
        QMessageBox::information(this, "恭喜", "本轮实训已结束！");
    }
}
