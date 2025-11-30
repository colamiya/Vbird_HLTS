#include "mainwindow.h"
#include <QFormLayout>
#include "config.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent, bool devModeDefault)
    : QMainWindow(parent)
{
    isDeveloperMode = devModeDefault;
    setFixedSize(Config::Global::WINDOW_SIZE);
    setWindowTitle(Config::Global::APP_TITLE);
    setupStyle();
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupStyle() {
    this->setStyleSheet(Config::Global::GLOBAL_STYLESHEET);
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
    connect(test1Widget, &Test1::levelCancelled, [this](){ mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test1Widget);

    // 3: Test 2
    test2Widget = new Test2();
    connect(test2Widget, &Test2::logMessage, this, &MainWindow::onLogMessage);
    connect(test2Widget, &Test2::levelCompleted, [this](){ onLevelCompleted(2); });
    connect(test2Widget, &Test2::levelCancelled, [this](){ mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test2Widget);

    // 4: Test 3
    test3Widget = new Test3(isDeveloperMode); // Pass dev mode
    connect(test3Widget, &Test3::logMessage, this, &MainWindow::onLogMessage);
    connect(test3Widget, &Test3::levelCompleted, [this](){ onLevelCompleted(3); });
    connect(test3Widget, &Test3::levelCancelled, [this](){ mainStack->setCurrentIndex(1); });
    mainStack->addWidget(test3Widget);
}

// --- Start Page ---
QWidget *MainWindow::createStartPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(30);

    // Title
    QLabel *title = new QLabel(Config::Global::TITLE_START_PAGE);
    title->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(Config::Global::FONT_SIZE_SUBTITLE));
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 0, Qt::AlignCenter);

    // Form
    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignCenter);

    nameInput = new QLineEdit();

    ageInput = new QSpinBox();
    ageInput->setRange(16, 100);
    ageInput->setButtonSymbols(QAbstractSpinBox::NoButtons); // Remove +/- buttons
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

    // Start Button
    QPushButton *startBtn = new QPushButton(Config::Global::BTN_TEXT_START);
    startBtn->setFixedWidth(Config::Global::SIZE_START_BTN_WIDTH);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartTraining);
    layout->addWidget(startBtn, 0, Qt::AlignCenter);

    // Emergency Event Toggle
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
    enableEmergencyEvents = emergencyToggle->isChecked();

    // Initialize Logger
    Logger::instance().setStudentInfo(student);

    // Pass configuration to Test3
    if (test3Widget) {
        test3Widget->setEmergencyMode(enableEmergencyEvents);
    }

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

    QLabel *title = new QLabel(Config::Global::TITLE_MAIN_MENU);
    title->setStyleSheet(QString("font-size: %1px; font-weight: bold;").arg(Config::Global::FONT_SIZE_TITLE));
    layout->addWidget(title, 0, Qt::AlignCenter);

    auto addBtn = [&](QString text, int idx) {
        QPushButton *btn = new QPushButton(text);
        btn->setFixedSize(Config::Global::SIZE_MENU_BTN);
        btn->setStyleSheet("font-size: 18px;");
        connect(btn, &QPushButton::clicked, [this, idx](){ mainStack->setCurrentIndex(idx); });
        layout->addWidget(btn, 0, Qt::AlignCenter);
        mainMenuButtons.append(btn);
    };

    addBtn(Config::Global::BTN_TEXT_TEST1, 2);
    addBtn(Config::Global::BTN_TEXT_TEST2, 3);
    addBtn(Config::Global::BTN_TEXT_TEST3, 4);

    // 开发者模式切换按钮已移除，请在 main.cpp 中设置 DEV_MODE_DEFAULT
    // Developer mode toggle removed. Set DEV_MODE_DEFAULT in main.cpp instead.
    /*
    QCheckBox *devCheck = new QCheckBox(Config::Global::CHECKBOX_TEXT_DEV_MODE);
    devCheck->setChecked(isDeveloperMode); // Set initial state
    connect(devCheck, &QCheckBox::checkStateChanged, this, &MainWindow::toggleDeveloperMode);
    layout->addWidget(devCheck, 0, Qt::AlignCenter);
    */

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
    
    // Detailed Log via Logger (Module can be inferred or passed, currently using Generic/Main)
    Logger::instance().logAction("Main/System", msg);

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
