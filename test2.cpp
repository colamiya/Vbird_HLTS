#include "test2.h"
#include "config.h"
#include <QMessageBox>
#include "logger.h"

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
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认退出", "确定要退出当前测验并返回主菜单吗？\n当前进度将不会保留。",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
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
        {"2. 开始工作前主管安排任务。谁的工作态度正确？", 3},
        {"3. 工作时，谁的工作方式正确？", 3},
        {"4. 工作时，谁的工作方式正确？", 3},
        {"5. 工作时，谁的工作方式正确？", 0},
        {"6. 进行交接时，谁的工作方式正确？", 0},
        {"7. 发现床单破洞了，谁的工作方式正确？", 0},
        {"8. 工作时，谁的工作方式正确？", 0},
        {"9. 遇到了紧急任务，需要送到15楼，谁的工作方式正确？", 0},
        {"10. 下午4:00汇报工作时，谁的方式正确？", 1},
        {"11. 推车轮子坏了，谁的处理方式正确？", 3},
        {"12. 工作时，谁的工作方式正确？", 0},
        {"13. 工作时，谁的工作方式正确？", 3},
        {"14. 地面湿滑，谁的处理方式正确？", 2},
        {"15. 工作时，谁的工作方式正确？", 2},
        {"16. 房间里的布草不够了，谁的工作方式正确？", 1},
        {"17. 和同事相处时，谁的做法错误？", 3},
        {"18. 被批评了，谁的态度是对的？", 1},
        {"19. 下午17:00下班了，谁做的是对的？", 0}};

    // 构建问题列表
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

        char suffix = 'A' + i;
        QString imgName = QString("%1%2").arg(currentQuestionIndex + 1).arg(suffix);

        // 使用配置中的路径模板加载图片
        QString path = QString(Config::Test2::PATH_FMT_JPG).arg(imgName);

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
        QString title = QString("第 %1 题 - 选项 %2").arg(currentQuestionIndex + 1).arg(suffix);
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

    // 计算总分
    for (const auto &q : questions)
    {
        if (q.userSelection == q.correctIndex)
            quizScore++;
    }

    // 填充 Logger 数据
    Logger::Test2BriefData data;
    data.score = quizScore;
    data.total = questions.size();

    // Calculate Duration
    qint64 secs = m_startTime.secsTo(QDateTime::currentDateTime());
    int mins = secs / 60;
    int s = secs % 60;
    data.timeUsed = QString("%1分%2秒").arg(mins).arg(s);

    char optionChars[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < questions.size(); ++i)
    {
        const Question &q = questions[i];
        bool isCorrect = (q.userSelection == q.correctIndex);

        Logger::Test2BriefData::QuestionResult r;
        r.id = i + 1;
        r.selection = (q.userSelection != -1) ? QString(optionChars[q.userSelection]) : "未作答";
        r.correct = isCorrect;
        data.results.append(r);

        emit logMessage(QString("题目%1: 选%2 -> %3").arg(i + 1).arg(r.selection).arg(isCorrect ? "正确" : "错误"));
    }

    Logger::instance().test2Data = data;
    emit logMessage(QString("测验完成. 得分: %1 / %2").arg(quizScore).arg(questions.size()));

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
    int btnFontSize = static_cast<int>(Config::Text::SIZE_TEST2_ACTION_BTN * m_currentScale);
    if(btnFontSize < 10) btnFontSize = 10;

    // For simple buttons without complex selectors, direct font-size setting works,
    // or regex replace if using the getter style.
    // GET_ACTION_BTN_STYLE returns "font-size: %1px; color: %2;" (no selector wrapper)
    // So appending works fine here, or replacement.
    QString navBtnStyle = Config::Test2::GET_ACTION_BTN_STYLE();
    navBtnStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(btnFontSize));

    if(prevQBtn) prevQBtn->setStyleSheet(navBtnStyle);
    if(nextQBtn) {
        if(currentQuestionIndex == questions.size() - 1) {
             // Submit button style
             QString submitStyle = QString("background-color: %1; color: %2; font-size: %3px;")
                .arg(Config::Test2::COL_BTN_SUBMIT)
                .arg(Config::Test2::COL_BTN_TEXT_WHITE)
                .arg(btnFontSize);
             nextQBtn->setStyleSheet(submitStyle);
        } else {
             nextQBtn->setStyleSheet(navBtnStyle);
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
