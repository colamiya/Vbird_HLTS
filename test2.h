#ifndef TEST2_H
#define TEST2_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QFile>
#include <QMessageBox>
#include <QTextEdit>
#include <QDialog>
#include <QScrollArea>
#include <QDateTime>

// 问题结构体
struct Question
{
    QString text;           // 问题文本
    QStringList options;    // 选项列表 (A, B, C, D)
    int correctIndex;       // 正确选项索引 (0-3)
    int userSelection = -1; // 用户的选择 (-1 表示未选)
};

class Test2 : public QWidget
{
    Q_OBJECT
public:
    explicit Test2(QWidget *parent = nullptr);

signals:
    void levelCompleted();        // 关卡完成信号
    void levelCancelled();        // 关卡取消信号
    void logMessage(QString msg); // 日志信号

private:
    QList<Question> questions;    // 问题列表
    int currentQuestionIndex = 0; // 当前问题索引
    int quizScore = 0;            // 得分

    // UI 控件
    QLabel *questionLabel;         // 问题显示标签
    QWidget *optionsContainer;     // 选项容器
    QButtonGroup *optionGroup;     // 选项按钮组 (互斥)
    QPushButton *optionButtons[4]; // 选项按钮数组 (A/B/C/D)
    QPushButton *optionImages[4];  // 选项图片按钮数组
    QPushButton *nextQBtn;         // 下一题/提交按钮
    QLabel *scoreLabel;            // 分数标签 (调试或最终显示用)

    // 逻辑函数
    void loadQuestion();                                            // 加载当前问题
    void handleOptionSelect(int index);                             // 处理选项选择
    void handleNextOrSubmit();                                      // 处理下一题或提交
    void showQuizSummary();                                         // 显示测验总结
    void showImagePreview(QString imagePath, const QString &title); // 显示图片大图预览 (Lightbox)

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateLayoutScale();
    QDateTime m_startTime;

    // Scaling Refs
    const QSize REF_SIZE = QSize(1750, 900);
    const int ORIG_FONT_SIZE_Q = 24;
    const int ORIG_FONT_SIZE_OPT = 16;
    QPushButton *prevQBtn;
    QPushButton *returnBtn;
};

#endif // TEST2_H
