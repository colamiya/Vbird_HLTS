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

// --- Helper Structs ---
struct StudentInfo {
    QString name;
    int age;
    QString gender;
    QString className;
    QString duration;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLogMessage(QString msg);
    void onLevelCompleted(int level);

private:
    void setupStyle();
    void setupUI();

    // Modules
    QWidget *createStartPage();
    QWidget *createMainMenu();
    void updateMainMenu();

    QStackedWidget *mainStack;
    
    // Sub-modules
    Test1 *test1Widget;
    Test2 *test2Widget;
    Test3 *test3Widget;

    // State
    StudentInfo student;
    int progressState = 1; // 1=Slideshow, 2=Quiz, 3=RPG
    bool isDeveloperMode = false;
    QList<QPushButton*> mainMenuButtons;

    // Info Page Inputs
    QLineEdit *nameInput;
    QSpinBox *ageInput;
    QComboBox *genderInput;
    QLineEdit *classInput;
    QLineEdit *durationInput;

    void onStartTraining();
    void toggleDeveloperMode(int state);
};

#endif // MAINWINDOW_H
