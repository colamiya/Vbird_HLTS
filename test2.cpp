#include "test2.h"
#include "config.h"

Test2::Test2(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    // Initial Data
    struct QuizItem { QString text; int correctIndex; };
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
        {"19. 下午17:00下班了，谁做的是对的？", 0}
    };

    for (const auto &item : quizData) {
        Question q;
        q.text = item.text;
        q.options = QStringList() << "选项 A" << "选项 B" << "选项 C" << "选项 D";
        q.correctIndex = item.correctIndex;
        questions.append(q);
    }

    questionLabel = new QLabel();
    questionLabel->setStyleSheet(Config::Test2::STYLE_QUESTION_LBL);
    questionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(questionLabel);

    optionGroup = new QButtonGroup(this);
    optionsContainer = new QWidget();
    QGridLayout *grid = new QGridLayout(optionsContainer);
    char optionChars[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < 4; ++i) {
        QWidget *optWidget = new QWidget();
        QVBoxLayout *optLayout = new QVBoxLayout(optWidget);

        optionImages[i] = new QPushButton();
        optionImages[i]->setFixedSize(Config::Test2::SIZE_OPTION_IMG);
        optionImages[i]->setFlat(true);
        optionImages[i]->setStyleSheet(Config::Test2::STYLE_IMG_BTN);

        optionButtons[i] = new QPushButton(QString("%1%2").arg(Config::Test2::TEXT_OPTION_PREFIX).arg(optionChars[i]));
        optionButtons[i]->setCheckable(true);
        optionButtons[i]->setFixedSize(Config::Test2::SIZE_OPTION_BTN);
        optionButtons[i]->setStyleSheet(Config::Test2::STYLE_OPTION_BTN);
        optionGroup->addButton(optionButtons[i], i);

        connect(optionButtons[i], &QPushButton::clicked, [this, i]() {
            handleOptionSelect(i);
        });

        optLayout->addWidget(optionImages[i], 0, Qt::AlignCenter);
        optLayout->addWidget(optionButtons[i], 0, Qt::AlignCenter);
        grid->addWidget(optWidget, i / 2, i % 2);
    }
    layout->addWidget(optionsContainer);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevQBtn = new QPushButton(Config::Test2::BTN_TEXT_PREV);
    nextQBtn = new QPushButton(Config::Test2::BTN_TEXT_NEXT);
    navLayout->addWidget(prevQBtn);
    navLayout->addWidget(nextQBtn);
    layout->addLayout(navLayout);

    scoreLabel = new QLabel("");
    layout->addWidget(scoreLabel);

    connect(prevQBtn, &QPushButton::clicked, [this]() {
        if (currentQuestionIndex > 0) {
            currentQuestionIndex--;
            loadQuestion();
        }
    });

    connect(nextQBtn, &QPushButton::clicked, [this]() {
         handleNextOrSubmit();
    });

    loadQuestion();
}

void Test2::loadQuestion() {
    Question &q = questions[currentQuestionIndex];
    questionLabel->setText(q.text);

    optionGroup->blockSignals(true);
    optionGroup->setExclusive(false);

    for (int i = 0; i < 4; ++i) {
        optionButtons[i]->setText(q.options[i]);

        if (q.userSelection == i) {
            optionButtons[i]->setChecked(true);
        } else {
            optionButtons[i]->setChecked(false);
        }

        char suffix = 'A' + i;
        QString imgName = QString("%1%2").arg(currentQuestionIndex + 1).arg(suffix);

        // Use config path template
        QString path = Config::Test2::PATH_FMT_JPG.arg(imgName);
        if (!QFile::exists(path)) {
            path = Config::Test2::PATH_FMT_PNG.arg(imgName);
        }

        QPixmap pix(path);
        if (pix.isNull()) {
            optionImages[i]->setIcon(QIcon());
            optionImages[i]->setText(Config::Test2::TEXT_NO_IMAGE);
        } else {
            optionImages[i]->setText("");
            optionImages[i]->setIcon(QIcon(pix));
            optionImages[i]->setIconSize(Config::Test2::SIZE_OPTION_ICON);
        }

        optionImages[i]->disconnect();
        connect(optionImages[i], &QPushButton::clicked, [this, path]() {
            showImagePreview(path);
        });
    }
    optionGroup->setExclusive(true);
    optionGroup->blockSignals(false);

    if (currentQuestionIndex == questions.size() - 1) {
        nextQBtn->setText(Config::Test2::BTN_TEXT_SUBMIT);
        // Use config color for submit
        nextQBtn->setStyleSheet(QString("background-color: %1; color: %2;").arg(Config::Test2::COL_BTN_SUBMIT, Config::Test2::COL_BTN_TEXT_WHITE));
    } else {
        nextQBtn->setText(Config::Test2::BTN_TEXT_NEXT);
        nextQBtn->setStyleSheet("");
    }
}

void Test2::handleOptionSelect(int index) {
    questions[currentQuestionIndex].userSelection = index;
}

void Test2::handleNextOrSubmit() {
    if (questions[currentQuestionIndex].userSelection == -1) {
        QMessageBox::warning(this, "提示", Config::Test2::MSG_WARNING_SELECT);
        return;
    }

    if (currentQuestionIndex < questions.size() - 1) {
        currentQuestionIndex++;
        loadQuestion();
    } else {
        showQuizSummary();
    }
}

void Test2::showQuizSummary() {
    quizScore = 0;
    QString summaryText;
    summaryText += Config::Test2::HTML_TABLE_STYLE;
    summaryText += "<h3>测验结果详情</h3>";
    summaryText += "<table><tr><th>题目</th><th>您的选择</th><th>正确答案</th><th>结果</th></tr>";

    char optionChars[] = {'A', 'B', 'C', 'D'};

    for (int i = 0; i < questions.size(); ++i) {
        const Question &q = questions[i];
        bool isCorrect = (q.userSelection == q.correctIndex);
        if (isCorrect) quizScore++;

        QString resultStr = isCorrect ? QString("<font color='%1'>正确</font>").arg(Config::Test2::HTML_CORRECT_COLOR)
                                      : QString("<font color='%1'>错误</font>").arg(Config::Test2::HTML_WRONG_COLOR);

        QString userStr = (q.userSelection != -1) ? QString("%1%2").arg(Config::Test2::TEXT_OPTION_PREFIX).arg(optionChars[q.userSelection]) : "未作答";
        QString correctStr = QString("%1%2").arg(Config::Test2::TEXT_OPTION_PREFIX).arg(optionChars[q.correctIndex]);

        summaryText += QString("<tr><td>%1...</td><td>%2</td><td>%3</td><td>%4</td></tr>")
                       .arg(q.text.left(10))
                       .arg(userStr)
                       .arg(correctStr)
                       .arg(resultStr);

        emit logMessage(QString("题目%1: 选%2 (正确%3) -> %4").arg(i+1).arg(userStr).arg(correctStr).arg(isCorrect ? "Win" : "Fail"));
    }
    summaryText += "</table>";

    QString finalScoreStr = QString("最终得分: %1 / %2").arg(quizScore).arg(questions.size());
    summaryText += QString("<h2>%1</h2>").arg(finalScoreStr);
    emit logMessage("测验完成. " + finalScoreStr);

    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("测验结果");
    dlg->resize(Config::Test2::SIZE_RESULT_DIALOG);
    QVBoxLayout *layout = new QVBoxLayout(dlg);

    QTextEdit *edit = new QTextEdit();
    edit->setHtml(summaryText);
    edit->setReadOnly(true);
    layout->addWidget(edit);

    QPushButton *closeBtn = new QPushButton(Config::Test2::BTN_TEXT_CLOSE_RESULT);
    connect(closeBtn, &QPushButton::clicked, [this, dlg]() {
        dlg->accept();
        emit levelCompleted();
    });
    layout->addWidget(closeBtn);

    dlg->exec();
}

void Test2::showImagePreview(QString imagePath) {
    QDialog *dlg = new QDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setWindowTitle("图片预览");
    dlg->resize(Config::Test2::SIZE_PREVIEW_DIALOG);

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
