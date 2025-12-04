#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include <QCheckBox>
#include <QApplication>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>

#include "test1.h"
#include "test2.h"
#include "test3.h"

// --- 辅助结构体 (Helper Structs) ---
// 学生信息结构体
struct StudentInfo
{
    QString name;      // 姓名
    int age;           // 年龄
    QString gender;    // 性别
    QString className; // 班级
    QString duration;  // 实训时长
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, bool devModeDefault = false);
    ~MainWindow();

private slots:
    void onLogMessage(QString msg);   // 处理日志消息
    void onLevelCompleted(int level); // 处理关卡完成

private:
    void setupStyle(); // 设置全局样式
    void setupUI();    // 初始化 UI

    // 模块页面创建函数
    QWidget *createStartPage(); // 创建开始页
    QWidget *createMainMenu();  // 创建主菜单
    void updateMainMenu();      // 更新主菜单状态 (解锁关卡)

    QStackedWidget *mainStack; // 页面堆栈

    // 子模块 (Sub-modules)
    Test1 *test1Widget;
    Test2 *test2Widget;
    Test3 *test3Widget;

    // 状态 (State)
    StudentInfo student;                  // 当前学生信息
    int progressState = 1;                // 当前进度: 1=幻灯片, 2=测验, 3=RPG
    bool isDeveloperMode = false;         // 是否开启开发者模式
    bool enableEmergencyEvents = false;   // 是否开启 Test3 的突发事件
    QList<QPushButton *> mainMenuButtons; // 主菜单按钮列表

    // 信息页输入控件
    QLineEdit *nameInput;
    QSpinBox *ageInput;
    QComboBox *genderInput;
    QLineEdit *classInput;
    QLineEdit *durationInput;
    QCheckBox *emergencyToggle; // 突发事件开关

    void onStartTraining();              // 开始实训
    void onEndTraining();                // 结束实训
    void toggleDeveloperMode(int state); // 切换开发者模式
};

#endif // MAINWINDOW_H
