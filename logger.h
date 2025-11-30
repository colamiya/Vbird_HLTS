#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QObject>
#include <QDir>
#include <QDebug>
#include "mainwindow.h" // For StudentInfo

class Logger : public QObject {
    Q_OBJECT
public:
    static Logger& instance() {
        static Logger _instance;
        return _instance;
    }

    void setStudentInfo(const StudentInfo& info) {
        m_student = info;
        initLogs();
    }

    // Write to Detailed Log (Action history)
    void logAction(const QString& module, const QString& action) {
        if (!m_initialized) return;
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QString line = QString("%1,%2,%3").arg(timestamp, module, action);

        QFile file(m_detailedPath);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << line << "\n";
            file.close();
        }
    }

    // Data structure for Test 3 Brief Report
    struct Test3BriefData {
        QString clockInStatus = "未打卡";
        QString clockOutStatus = "未打卡";
        bool isLate = false;

        struct FloorStatus {
            int floor;
            bool isCorrect;
            QString details; // e.g. "Missing Items" or "Correct"
        };
        QList<FloorStatus> floorStatuses;

        bool emergencyPriorityMet = true; // Default true unless failed
        bool mixedLinen = false;
    };

    // Data structure for Test 2 Brief Report
    struct Test2BriefData {
        int score = 0;
        int total = 0;
        struct QuestionResult {
            int id;
            QString selection; // A, B, C, D or None
            bool correct;
        };
        QList<QuestionResult> results;
    };

    Test3BriefData test3Data;
    Test2BriefData test2Data;

    // Write the final Brief Report
    void generateBriefReport() {
        if (!m_initialized) return;

        QFile file(m_briefPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) { // Overwrite or append? Usually overwrite for a session
             QTextStream out(&file);
             // Header
             out << QString("姓名,%1,年龄,%2,性别,%3,班级,%4,时长,%5\n")
                    .arg(m_student.name)
                    .arg(m_student.age)
                    .arg(m_student.gender)
                    .arg(m_student.className)
                    .arg(m_student.duration);
             out << "----------------------------------------\n";

             // Test 2
             out << "【测试2: 理论考核】\n";
             out << QString("总分: %1 / %2\n").arg(test2Data.score).arg(test2Data.total);
             out << "题目,选择,结果\n";
             for (const auto& q : test2Data.results) {
                 out << QString("第%1题,%2,%3\n")
                        .arg(q.id)
                        .arg(q.selection)
                        .arg(q.correct ? "正确" : "错误");
             }
             out << "\n";

             // Test 3
             out << "【测试3: 实训考核】\n";
             out << QString("上班打卡: %1 (%2)\n").arg(test3Data.clockInStatus).arg(test3Data.isLate ? "迟到" : "正常");
             out << QString("下班打卡: %1\n").arg(test3Data.clockOutStatus);
             out << QString("紧急任务优先: %1\n").arg(test3Data.emergencyPriorityMet ? "是" : "否");
             out << QString("布草混装: %1\n").arg(test3Data.mixedLinen ? "是" : "否");

             out << "楼层任务状态:\n";
             if (test3Data.floorStatuses.isEmpty()) {
                 out << "无任务数据\n";
             } else {
                 for (const auto& f : test3Data.floorStatuses) {
                     out << QString("%1楼: %2 (%3)\n").arg(f.floor).arg(f.isCorrect ? "完成" : "未完成").arg(f.details);
                 }
             }

             file.close();
        }
    }

private:
    Logger() {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    StudentInfo m_student;
    QString m_briefPath = "brief_report.csv";
    QString m_detailedPath = "detailed_report.csv";
    bool m_initialized = false;

    void initLogs() {
        m_initialized = true;

        // Init Detailed Log with Header
        QFile dFile(m_detailedPath);
        if (dFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&dFile);
            out << QString("姓名,%1,年龄,%2,性别,%3,班级,%4,时长,%5\n")
                   .arg(m_student.name)
                   .arg(m_student.age)
                   .arg(m_student.gender)
                   .arg(m_student.className)
                   .arg(m_student.duration);
            out << "时间,模块,操作\n";
            dFile.close();
        }
    }
};

#endif // LOGGER_H
