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
#include "config_global.h"

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger &instance()
    {
        static Logger _instance;
        return _instance;
    }

    void setStudentInfo(const StudentInfo &info)
    {
        m_student = info;
        initLogs();
    }

    // Write to Detailed Log (Action history)
    void logAction(const QString &module, const QString &action)
    {
        if (!m_initialized)
            return;
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        // Write to CN Log
        if (Config::Csv::ENABLE_OUTPUT_CN)
        {
            QString line = QString("%1,%2,%3").arg(timestamp, module, action);
            QFile file(Config::Csv::FILENAME_DETAILED_CN);
            if (file.open(QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);
                out << line << "\n";
                file.close();
            }
        }

        // Write to EN Log
        if (Config::Csv::ENABLE_OUTPUT_EN)
        {
            // Translating basic action log structure is hard dynamically,
            // but we will keep the content as passed (often mixed) or
            // assume caller handles localization if needed.
            // For now, we log the same content to both, just ensuring files exist.
            QString line = QString("%1,%2,%3").arg(timestamp, module, action);
            QFile file(Config::Csv::FILENAME_DETAILED_EN);
            if (file.open(QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);
                out << line << "\n";
                file.close();
            }
        }
    }

    // Data structure for Test 3 Brief Report
    struct Test3BriefData
    {
        QString clockInStatus = "未打卡";
        QString clockOutStatus = "未打卡";
        bool isLate = false;

        struct FloorStatus
        {
            int floor;
            bool isCorrect;
            QString details; // e.g. "Missing Items" or "Correct"
        };
        QList<FloorStatus> floorStatuses;

        bool emergencyPriorityMet = true; // Default true unless failed
        bool mixedLinen = false;
    };

    // Data structure for Test 2 Brief Report
    struct Test2BriefData
    {
        int score = 0;
        int total = 0;
        struct QuestionResult
        {
            int id;
            QString selection; // A, B, C, D or None
            bool correct;
        };
        QList<QuestionResult> results;
    };

    Test3BriefData test3Data;
    Test2BriefData test2Data;

    // Write the final Brief Report
    void generateBriefReport()
    {
        if (!m_initialized)
            return;

        if (Config::Csv::ENABLE_OUTPUT_CN)
        {
            writeBriefReport(Config::Csv::FILENAME_BRIEF_CN, true);
        }
        if (Config::Csv::ENABLE_OUTPUT_EN)
        {
            writeBriefReport(Config::Csv::FILENAME_BRIEF_EN, false);
        }
    }

private:
    Logger() {}
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    StudentInfo m_student;
    bool m_initialized = false;

    void initLogs()
    {
        m_initialized = true;

        if (Config::Csv::ENABLE_OUTPUT_CN)
        {
            initDetailedLog(Config::Csv::FILENAME_DETAILED_CN, true);
        }
        if (Config::Csv::ENABLE_OUTPUT_EN)
        {
            initDetailedLog(Config::Csv::FILENAME_DETAILED_EN, false);
        }
    }

    void initDetailedLog(const QString &filename, bool isCn)
    {
        QFile dFile(filename);
        if (dFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // Add BOM for Chinese (UTF-8 with BOM)
            if (isCn)
            {
                const char bom[] = {(char)0xEF, (char)0xBB, (char)0xBF};
                dFile.write(bom, 3);
            }

            QTextStream out(&dFile);
            // Header
            QString headerLine;
            if (isCn)
            {
                headerLine = QString("姓名,%1,年龄,%2,性别,%3,班级,%4,时长,%5\n")
                                 .arg(m_student.name)
                                 .arg(m_student.age)
                                 .arg(m_student.gender)
                                 .arg(m_student.className)
                                 .arg(m_student.duration);
                out << headerLine;
                out << "时间,模块,操作\n";
            }
            else
            {
                headerLine = QString("Name,%1,Age,%2,Gender,%3,Class,%4,Duration,%5\n")
                                 .arg(m_student.name) // Assuming student info is entered in user's language
                                 .arg(m_student.age)
                                 .arg(m_student.gender)
                                 .arg(m_student.className)
                                 .arg(m_student.duration);
                out << headerLine;
                out << "Time,Module,Action\n";
            }
            dFile.close();
        }
    }

    void writeBriefReport(const QString &filename, bool isCn)
    {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // BOM for CN
            if (isCn)
            {
                const char bom[] = {(char)0xEF, (char)0xBB, (char)0xBF};
                file.write(bom, 3);
            }

            QTextStream out(&file);

            // --- 1. Student Info ---
            QStringList infoParts;
            if (Config::Csv::LOG_STUDENT_NAME)
            {
                infoParts << (isCn ? QString("姓名,%1").arg(m_student.name) : QString("Name,%1").arg(m_student.name));
            }
            if (Config::Csv::LOG_STUDENT_AGE)
            {
                infoParts << (isCn ? QString("年龄,%1").arg(m_student.age) : QString("Age,%1").arg(m_student.age));
            }
            if (Config::Csv::LOG_STUDENT_GENDER)
            {
                infoParts << (isCn ? QString("性别,%1").arg(m_student.gender) : QString("Gender,%1").arg(m_student.gender));
            }
            if (Config::Csv::LOG_STUDENT_CLASS)
            {
                infoParts << (isCn ? QString("班级,%1").arg(m_student.className) : QString("Class,%1").arg(m_student.className));
            }
            if (Config::Csv::LOG_SESSION_DURATION)
            {
                infoParts << (isCn ? QString("时长,%1").arg(m_student.duration) : QString("Duration,%1").arg(m_student.duration));
            }

            if (!infoParts.isEmpty())
            {
                out << infoParts.join(",") << "\n";
            }
            out << "----------------------------------------\n";

            // --- 2. Test 2 (Quiz) ---
            if (Config::Csv::LOG_TEST2_SCORE)
            {
                out << (isCn ? "【测试2: 理论考核】\n" : "[Test 2: Theory Quiz]\n");
                out << (isCn ? QString("总分: %1 / %2\n").arg(test2Data.score).arg(test2Data.total)
                             : QString("Total Score: %1 / %2\n").arg(test2Data.score).arg(test2Data.total));
            }

            if (Config::Csv::LOG_TEST2_DETAILS)
            {
                out << (isCn ? "题目,选择,结果\n" : "Question,Selection,Result\n");
                for (const auto &q : test2Data.results)
                {
                    QString correctStr = isCn ? (q.correct ? "正确" : "错误") : (q.correct ? "Correct" : "Incorrect");
                    QString qStr = isCn ? QString("第%1题").arg(q.id) : QString("Q%1").arg(q.id);
                    out << QString("%1,%2,%3\n").arg(qStr).arg(q.selection).arg(correctStr);
                }
                out << "\n";
            }

            // --- 3. Test 3 (RPG) ---
            bool showTest3Header = Config::Csv::LOG_TEST3_CLOCK || Config::Csv::LOG_TEST3_EVENTS || Config::Csv::LOG_TEST3_FLOOR_STATUS;
            if (showTest3Header)
            {
                out << (isCn ? "【测试3: 实训考核】\n" : "[Test 3: Practical Training]\n");
            }

            if (Config::Csv::LOG_TEST3_CLOCK)
            {
                QString lateStr = isCn ? (test3Data.isLate ? "迟到" : "正常") : (test3Data.isLate ? "Late" : "On Time");
                out << (isCn ? QString("上班打卡: %1 (%2)\n").arg(test3Data.clockInStatus).arg(lateStr)
                             : QString("Clock In: %1 (%2)\n").arg(test3Data.clockInStatus).arg(lateStr));
                out << (isCn ? QString("下班打卡: %1\n").arg(test3Data.clockOutStatus)
                             : QString("Clock Out: %1\n").arg(test3Data.clockOutStatus));
            }

            if (Config::Csv::LOG_TEST3_EVENTS)
            {
                QString priorityStr = isCn ? (test3Data.emergencyPriorityMet ? "是" : "否") : (test3Data.emergencyPriorityMet ? "Yes" : "No");
                QString mixedStr = isCn ? (test3Data.mixedLinen ? "是" : "否") : (test3Data.mixedLinen ? "Yes" : "No");
                out << (isCn ? QString("紧急任务优先: %1\n").arg(priorityStr) : QString("Emergency Priority Met: %1\n").arg(priorityStr));
                out << (isCn ? QString("布草混装: %1\n").arg(mixedStr) : QString("Mixed Linen: %1\n").arg(mixedStr));
            }

            if (Config::Csv::LOG_TEST3_FLOOR_STATUS)
            {
                out << (isCn ? "楼层任务状态:\n" : "Floor Task Status:\n");
                if (test3Data.floorStatuses.isEmpty())
                {
                    out << (isCn ? "无任务数据\n" : "No Task Data\n");
                }
                else
                {
                    for (const auto &f : test3Data.floorStatuses)
                    {
                        QString statusStr = isCn ? (f.isCorrect ? "完成" : "未完成") : (f.isCorrect ? "Complete" : "Incomplete");
                        // Assuming 'details' is passed in somewhat generic way or already localized.
                        // For now, we print it as is, or we could add specific translations if 'details' has fixed values.
                        QString floorStr = isCn ? QString("%1楼").arg(f.floor) : QString("%1F").arg(f.floor);
                        out << QString("%1: %2 (%3)\n").arg(floorStr).arg(statusStr).arg(f.details);
                    }
                }
            }

            file.close();
        }
    }
};

#endif // LOGGER_H
