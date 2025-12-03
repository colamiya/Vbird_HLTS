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
    void generateBriefReport();

private:
    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    StudentInfo m_student;
    bool m_initialized = false;

    // Dynamic filenames
    QString m_detailedFnCn;
    QString m_detailedFnEn;
    QString m_briefFnCn;
    QString m_briefFnEn;

    void initLogs();
    void initDetailedLog(const QString &filename, bool isCn);
    void writeBriefReport(const QString &filename, bool isCn);
};

#endif // LOGGER_H
