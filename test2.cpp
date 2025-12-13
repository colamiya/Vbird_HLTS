#include "test2.h"
#include "config.h"
#include "utils.h" // Include custom dialogs
#include <QMessageBox>
#include "logger.h"
#include <QRegularExpression>

Test2::Test2(QWidget *parent) : QWidget(parent)
{
    // 主布局 (网格布局，方便右上角放置按钮)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setAlignment(Qt::AlignCenter);

    // 返回按钮 (右上角)
    returnBtn = new QPushButton(Config::Test2::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test2::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test2::GET_BTN_RETURN_STYLE());
    connect(returnBtn, &QPushButton::clicked, [this]()
            {
        // Replace QMessageBox with Custom Confirm Dialog
        if (Utils::ShowCustomConfirmDialog(this, "确认退出", "确定要退出当前测验并返回主菜单吗？\n当前进度将不会保留。")) {
             emit levelCancelled();
        } });
    mainGrid->addWidget(returnBtn, 0, 1, Qt::AlignRight | Qt::AlignTop);

    // 主内容容器
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);
    layout->setAlignment(Qt::AlignCenter);
    mainGrid->addWidget(contentWidget, 1, 0, 1, 2);

    // --- 初始化数据 ---
    // 内部结构体用于临时构建数据
    struct QuizItem
    {
        QString text;
        int correctIndex;
    };
    QList<QuizItem> quizData = {
        {"1. 上午8:00上班，谁的工作态度正确？", 2},
        {"2. 开始工作前主管安排任务给小明、小刚、小花和小红，其中谁的工作态度正确？", 3},
        {"3. 谁的工作做的最好", 3},
        {"4. 送布草时，谁的工作方式正确？", 3},
        {"5. 工作时，谁放置布草的方式正确？", 0},
        {"6. 发现床单损坏了，谁的处理方式正确？", 0},
        {"7. 工作时，谁的处理脏布草的方式正确？", 0},
        {"8. 遇到了紧急任务，需送布草到15楼，谁的工作方式正确？", 0},
        {"9. 下午4:00汇报工作时，谁汇报工作的方式正确？", 1},
        {"10. 推车轮子坏了，谁的处理方式正确？", 3},
        {"11. 工作时，谁的工作方式正确？", 0},
        {"12. 打包布草时，谁的工作方式正确？", 3},
        {"13. 地面湿滑，谁的处理方式正确？", 2},
        {"14. 整理布草时，谁的工作方式正确？", 3},
        {"15. 房间里的布草不够了，谁的工作方式正确？", 2},
        {"16. 和同事相处时，谁的做法错误？", 3},
        {"17. 被经理批评时，谁的态度是对的？", 1},
        {"18. 下午17:00是下班时间，谁做的是对的？", 0}};

    // 构建问题列表
    // 1. 添加引导页 (Intro Slide)
    Question intro;
    intro.text = "这里有四位酒店的员工，请你先认识他们。在你认识他们后，我们一起来看看他们是怎么工作的";
    intro.options = QStringList() << "" << "" << "" << ""; // 占位
    intro.correctIndex = -1; // 不计分
    questions.append(intro);

    // 2. 添加正式题目
    for (const auto &item : quizData)
    {
        Question q;
        q.text = item.text;
        q.options = QStringList() << "选项 A" << "选项 B" << "选项 C" << "选项 D";
        q.correctIndex = item.correctIndex;
        questions.append(q);
    }

    // --- UI 构建 ---
    // 问题标签
    questionLabel = new QLabel();
    questionLabel->setStyleSheet(Config::Test2::GET_STYLE_QUESTION_LBL());
    questionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(questionLabel);

    // 选项组
    optionGroup = new QButtonGroup(this);
    optionsContainer = new QWidget();
    QGridLayout *grid = new QGridLayout(optionsContainer);
    grid->setSpacing(20); // 增加间距
    char optionChars[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < 4; ++i)
    {
        QWidget *optWidget = new QWidget();
        QVBoxLayout *optLayout = new QVBoxLayout(optWidget);
        optLayout->setSpacing(10); // 图片和按钮之间的间距

        // 图片按钮 (点击可放大)
        optionImages[i] = new QPushButton();
        optionImages[i]->setFixedSize(Config::Test2::SIZE_OPTION_IMG);
        optionImages[i]->setFlat(true);
        optionImages[i]->setStyleSheet(Config::Test2::STYLE_IMG_BTN);

        // 选项按钮 (A/B/C/D)
        optionButtons[i] = new QPushButton(QString("%1%2").arg(Config::Test2::TEXT_OPTION_PREFIX).arg(optionChars[i]));
        optionButtons[i]->setCheckable(true);
        optionButtons[i]->setFixedSize(Config::Test2::SIZE_OPTION_BTN);
        optionButtons[i]->setStyleSheet(Config::Test2::GET_STYLE_OPTION_BTN());
        optionGroup->addButton(optionButtons[i], i); // 添加到按钮组，ID 为索引

        connect(optionButtons[i], &QPushButton::clicked, [this, i]()
                { handleOptionSelect(i); });

        optLayout->addWidget(optionImages[i], 0, Qt::AlignCenter);
        optLayout->addWidget(optionButtons[i], 0, Qt::AlignCenter);
        grid->addWidget(optWidget, i / 2, i % 2); // 2x2 网格布局
    }
    layout->addWidget(optionsContainer);

    // 导航按钮
    QHBoxLayout *navLayout = new QHBoxLayout();
    prevQBtn = new QPushButton(Config::Test2::BTN_TEXT_PREV);
    nextQBtn = new QPushButton(Config::Test2::BTN_TEXT_NEXT);

    // Apply initial style
    prevQBtn->setStyleSheet(Config::Test2::GET_ACTION_BTN_STYLE());
    nextQBtn->setStyleSheet(Config::Test2::GET_ACTION_BTN_STYLE());

    navLayout->addWidget(prevQBtn);
    navLayout->addWidget(nextQBtn);
    layout->addLayout(navLayout);

    scoreLabel = new QLabel("");
    layout->addWidget(scoreLabel);

    // 连接导航信号
    connect(prevQBtn, &QPushButton::clicked, [this]()
            {
        if (currentQuestionIndex > 0) {
            currentQuestionIndex--;
            loadQuestion();
        } });

    connect(nextQBtn, &QPushButton::clicked, [this]()
            { handleNextOrSubmit(); });

    loadQuestion();
}

void Test2::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_startTime = QDateTime::currentDateTime();
    emit logMessage("Test 2 started at " + m_startTime.toString("HH:mm:ss"));
}

void Test2::loadQuestion()
{
    Question &q = questions[currentQuestionIndex];
    questionLabel->setText(q.text);

    // 暂时阻塞信号以防止在设置状态时触发逻辑
    optionGroup->blockSignals(true);
    optionGroup->setExclusive(false); // 暂时允许无选择状态 (虽然这里逻辑上总是单选，但为了重置显示)

    for (int i = 0; i < 4; ++i)
    {
        QString path;
        QString title;

        // 特殊处理引导页 (Index 0)
        if (currentQuestionIndex == 0) {
            optionButtons[i]->setVisible(false); // 隐藏选项按钮

            // 加载 01.jpg - 04.jpg
            QString imgName = QString("0%1").arg(i + 1);
            path = QString(Config::Test2::PATH_FMT_JPG).arg(imgName);
            title = QString("员工介绍 %1").arg(i + 1);
        }
        else {
            optionButtons[i]->setVisible(true); // 显示选项按钮
            optionButtons[i]->setText(q.options[i]);

            // 恢复之前的选择
            if (q.userSelection == i)
            {
                optionButtons[i]->setChecked(true);
            }
            else
            {
                optionButtons[i]->setChecked(false);
            }

            // 加载题目图片
            // 因为第0页是引导页，所以第1页(Q1)对应图片 1A/B/C/D
            // 公式直接使用 currentQuestionIndex 即可 (Index 1 -> "1")
            char suffix = 'A' + i;
            QString imgName = QString("%1%2").arg(currentQuestionIndex).arg(suffix);
            path = QString(Config::Test2::PATH_FMT_JPG).arg(imgName);
            title = QString("第 %1 题 - 选项 %2").arg(currentQuestionIndex).arg(suffix);
        }

        QPixmap pix(path);
        if (pix.isNull())
        {
            optionImages[i]->setIcon(QIcon());
            optionImages[i]->setText(Config::Test2::TEXT_NO_IMAGE);
        }
        else
        {
            optionImages[i]->setText("");
            optionImages[i]->setIcon(QIcon(pix));
            // 使用配置中的大图标尺寸
            optionImages[i]->setIconSize(Config::Test2::SIZE_OPTION_ICON);
        }

        // 断开旧连接并连接新的预览信号
        optionImages[i]->disconnect();
        connect(optionImages[i], &QPushButton::clicked, [this, path, title]()
                { showImagePreview(path, title); });
    }

    // 更新布局缩放，确保新加载的图标/文本大小正确
    updateLayoutScale();

    optionGroup->setExclusive(true);
    optionGroup->blockSignals(false);

    // 更新按钮文本 (最后一题显示提交)
    if (currentQuestionIndex == questions.size() - 1)
    {
        nextQBtn->setText(Config::Test2::BTN_TEXT_SUBMIT);
        // 使用配置颜色 + 字体大小
        QString submitStyle = QString("background-color: %1; color: %2; font-size: %3px;")
                .arg(Config::Test2::COL_BTN_SUBMIT)
                .arg(Config::Test2::COL_BTN_TEXT_WHITE)
                .arg(Config::Text::SIZE_TEST2_ACTION_BTN);
        nextQBtn->setStyleSheet(submitStyle);
    }
    else if (currentQuestionIndex == 0)
    {
        nextQBtn->setText("开始"); // 引导页显示 "开始"
        nextQBtn->setStyleSheet(Config::Test2::GET_ACTION_BTN_STYLE());
    }
    else
    {
        nextQBtn->setText(Config::Test2::BTN_TEXT_NEXT);
        nextQBtn->setStyleSheet(Config::Test2::GET_ACTION_BTN_STYLE());
    }
}

void Test2::handleOptionSelect(int index)
{
    questions[currentQuestionIndex].userSelection = index;
}

void Test2::handleNextOrSubmit()
{
    // 允许不选直接下一题 (按用户需求: 移除必选提示)
    // if (questions[currentQuestionIndex].userSelection == -1) { ... return; }

    if (currentQuestionIndex < questions.size() - 1)
    {
        currentQuestionIndex++;
        loadQuestion();
    }
    else
    {
        showQuizSummary();
    }
}

void Test2::showQuizSummary()
{
    quizScore = 0;

    // 计算总分 (跳过 Index 0 的引导页)
    for (int i = 1; i < questions.size(); ++i)
    {
        const Question &q = questions[i];
        if (q.userSelection == q.correctIndex)
            quizScore++;
    }

    // 填充 Logger 数据
    Logger::Test2BriefData data;
    data.score = quizScore;
    data.total = questions.size() - 1; // 减去引导页

    // Calculate Duration
    qint64 secs = m_startTime.secsTo(QDateTime::currentDateTime());
    int mins = secs / 60;
    int s = secs % 60;
    data.timeUsed = QString("%1分%2秒").arg(mins).arg(s);

    char optionChars[] = {'A', 'B', 'C', 'D'};

    // 记录结果 (从 Index 1 开始)
    for (int i = 1; i < questions.size(); ++i)
    {
        const Question &q = questions[i];
        bool isCorrect = (q.userSelection == q.correctIndex);

        Logger::Test2BriefData::QuestionResult r;
        r.id = i; // 题目ID从1开始 (对应 questions[1])
        r.selection = (q.userSelection != -1) ? QString(optionChars[q.userSelection]) : "未作答";
        r.correct = isCorrect;
        data.results.append(r);

        emit logMessage(QString("题目%1: 选%2 -> %3").arg(r.id).arg(r.selection).arg(isCorrect ? "正确" : "错误"));
    }

    Logger::instance().test2Data = data;
    emit logMessage(QString("测验完成. 得分: %1 / %2").arg(quizScore).arg(data.total));

    // 直接完成关卡，不弹窗
    emit levelCompleted();
}

void Test2::showImagePreview(QString imagePath, const QString &title)
{
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动释放内存
    dlg->setWindowTitle(title);
    dlg->resize(Config::Test2::SIZE_PREVIEW_DIALOG);

    QVBoxLayout *layout = new QVBoxLayout(dlg);
    QScrollArea *scroll = new QScrollArea(dlg);
    QLabel *imgLbl = new QLabel();
    imgLbl->setAlignment(Qt::AlignCenter);

    QPixmap pix(imagePath);
    if (!pix.isNull())
    {
        imgLbl->setPixmap(pix);
        // Lightbox 行为: 显示全图，由 ScrollArea 提供滚动
        imgLbl->setScaledContents(false);

        // 如果图片过大，适当缩放到对话框大小，避免打开时过于夸张
        if (pix.width() > Config::Test2::SIZE_PREVIEW_DIALOG.width())
        {
            imgLbl->setPixmap(pix.scaled(Config::Test2::SIZE_PREVIEW_DIALOG - QSize(50, 50), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    else
    {
        imgLbl->setText("无法加载图片");
    }

    scroll->setWidget(imgLbl);
    scroll->setWidgetResizable(true); // 允许居中
    layout->addWidget(scroll);

    dlg->exec();
}

void Test2::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLayoutScale();
    // Force immediate update
    this->update();
}

void Test2::updateLayoutScale()
{
    // Calculate scale based on current size vs reference size
    float scaleX = (float)width() / REF_SIZE.width();
    float scaleY = (float)height() / REF_SIZE.height();
    m_currentScale = qMin(scaleX, scaleY); // Maintain aspect ratio for content elements

    // 1. Scale Question Label (Font)
    if(questionLabel) {
        int newSize = static_cast<int>(Config::Text::SIZE_TEST2_QUESTION * m_currentScale);
        if(newSize < 12) newSize = 12;
        // Re-construct stylesheet with dynamic font size
        QString newStyle = QString("font-size: %1px; font-weight: bold; color: %2; margin-bottom: 20px;")
                   .arg(newSize)
                   .arg(Config::Text::COLOR_TEST2_QUESTION);
        questionLabel->setStyleSheet(newStyle);
    }

    // 2. Scale Options (Images & Buttons)
    QSize newImgSize = Config::Test2::SIZE_OPTION_IMG * m_currentScale;
    QSize newBtnSize = Config::Test2::SIZE_OPTION_BTN * m_currentScale;
    QSize newIconSize = Config::Test2::SIZE_OPTION_ICON * m_currentScale;

    int newOptFontSize = static_cast<int>(Config::Text::SIZE_TEST2_OPTION * m_currentScale);
    if(newOptFontSize < 10) newOptFontSize = 10;

    // Use string replacement for font-size in the dynamically retrieved style
    QString baseOptStyle = Config::Test2::GET_STYLE_OPTION_BTN();
    QString dynamicOptStyle = baseOptStyle;
    // Replace existing font-size inside the block using regex to ensure validity
    dynamicOptStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(newOptFontSize));

    for(int i=0; i<4; ++i) {
        if(optionImages[i]) {
            optionImages[i]->setFixedSize(newImgSize);
            optionImages[i]->setIconSize(newIconSize);
        }
        if(optionButtons[i]) {
            optionButtons[i]->setFixedSize(newBtnSize);
            optionButtons[i]->setStyleSheet(dynamicOptStyle);
        }
    }

    // 3. Scale Nav Buttons
    // Adopt logic from Test 1: use BTN_HEIGHT_BASE * scale for height
    // And dynamic font size

    int btnH = Config::Test1::BTN_HEIGHT_BASE * m_currentScale; // Use Test1 Base Height for consistency
    if (btnH < 30) btnH = 30;

    int btnFontSize = static_cast<int>(Config::Text::SIZE_TEST2_ACTION_BTN * m_currentScale);
    if(btnFontSize < 10) btnFontSize = 10;

    QString navBtnStyle = Config::Test2::GET_ACTION_BTN_STYLE();
    navBtnStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(btnFontSize));

    if(prevQBtn) {
        prevQBtn->setStyleSheet(navBtnStyle);
        prevQBtn->setMinimumHeight(btnH);
    }
    if(nextQBtn) {
        if(currentQuestionIndex == questions.size() - 1) {
             // Submit button style
             QString submitStyle = QString("background-color: %1; color: %2; font-size: %3px;")
                .arg(Config::Test2::COL_BTN_SUBMIT)
                .arg(Config::Test2::COL_BTN_TEXT_WHITE)
                .arg(btnFontSize);
             nextQBtn->setStyleSheet(submitStyle);
             nextQBtn->setMinimumHeight(btnH);
        } else {
             nextQBtn->setStyleSheet(navBtnStyle);
             nextQBtn->setMinimumHeight(btnH);
        }
    }

    // 4. Return Button
    // remove fixed width constraint to allow text to fit, set min size instead
    QSize scaledRetBtnMin = Config::Test2::RETURN_BTN_SIZE * m_currentScale;
    if (scaledRetBtnMin.width() < 120) scaledRetBtnMin.setWidth(120); // ensure minimum readability width
    if (scaledRetBtnMin.height() < 30) scaledRetBtnMin.setHeight(30);

    if(returnBtn) {
        // Reset fixed size to allow expansion if needed, set min size
        returnBtn->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)); // Clear fixed size
        returnBtn->setMinimumSize(scaledRetBtnMin);
        returnBtn->setMaximumSize(scaledRetBtnMin.width() * 2, scaledRetBtnMin.height()); // Constrain max width a bit

        // Ensure font scales but doesn't get too small
        int retBtnFontSize = static_cast<int>(Config::Text::SIZE_TEST1_RETURN_BTN * m_currentScale);
        if (retBtnFontSize < 10) retBtnFontSize = 10;

        // GET_BTN_RETURN_STYLE returns simple style string without selector wrapper
        QString retStyle = Config::Test2::GET_BTN_RETURN_STYLE();
        retStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(retBtnFontSize));
        returnBtn->setStyleSheet(retStyle);
    }
}
