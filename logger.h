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
    }

    // Start a new session with timestamped log files
    void startNewSession();

    // Write to Detailed Log (Action history)
    void logAction(const QString &module, const QString &action);

    // Write raw system message to Detailed Log (for qDebug redirection)
    void logSystemMessage(const QString &msg);

    // Data structure for Test 3 Brief Report
    struct Test3BriefData
    {
        QString clockInStatus = "未打卡";
        QString clockOutStatus = "未打卡";
        bool isLate = false;

        // Task Results
        struct FloorStatus
        {
            int floor;
            bool isCorrect;
            QString details;
        };
        QList<FloorStatus> floorStatuses;

        QString taskList;            // Assigned tasks summary
        QString timeUsed;            // Duration string
        bool emergencyPriorityMet = true;
        bool mixedLinen = false;
    };

    // Data structure for Test 2 Brief Report
    struct Test2BriefData
    {
        int score = 0;
        int total = 0;
        QString timeUsed; // Duration string
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
    void generateBriefReport();

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    StudentInfo m_student;
    bool m_initialized = false;

    // Dynamic filenames
    QString m_detailedFnCn;
    QString m_briefFnCn;

    void initLogs();
    void initDetailedLog(const QString &filename);
    void writeBriefReport(const QString &filename);
};

#endif // LOGGER_H
