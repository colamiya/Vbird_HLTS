#include "logger.h"
#include <cstdio>

// Static handler for redirection
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Write to file via Logger instance
    Logger::instance().logSystemMessage(msg);

    // Also print to standard output (so we don't lose console output)
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData());
}

Logger::Logger() : m_initialized(false)
{
}

// Write to Detailed Log (Action history)
void Logger::logAction(const QString &module, const QString &action)
{
    if (!m_initialized)
        return;
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    if (Config::Csv::ENABLE_OUTPUT_CN && !m_detailedFnCn.isEmpty())
    {
        // Format: Time,Module,Action
        QString line = QString("%1,%2,%3").arg(timestamp, module, action);
        QFile file(m_detailedFnCn);
        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&file);
            out << line << "\n";
            file.close();
        }
    }
}

void Logger::logSystemMessage(const QString &msg)
{
    if (!m_initialized || !Config::Csv::ENABLE_OUTPUT_CN || m_detailedFnCn.isEmpty())
        return;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    // Format for system logs: Time,System,Message
    // We treat "System" as the module for raw debug output
    QString line = QString("%1,System,%2").arg(timestamp, msg);

    QFile file(m_detailedFnCn);
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << line << "\n";
        file.close();
    }
}


void Logger::generateBriefReport()
{
    if (!m_initialized)
        return;

    if (Config::Csv::ENABLE_OUTPUT_CN && !m_briefFnCn.isEmpty())
    {
        writeBriefReport(m_briefFnCn);
    }
}

void Logger::startNewSession()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    // Helper to insert timestamp and name before filename
    auto insertTimestamp = [&](const char *filename) -> QString
    {
        QString fn = QString(filename);
        QString prefix = timestamp + "_" + m_student.name; // Format: Time_Name
        return prefix + "_" + fn;
    };

    if (Config::Csv::ENABLE_OUTPUT_CN)
    {
        m_detailedFnCn = insertTimestamp(Config::Csv::FILENAME_DETAILED_CN);
        m_briefFnCn = insertTimestamp(Config::Csv::FILENAME_BRIEF_CN);
    }

    initLogs();

    // Install the message handler to redirect qDebug to the new log file
    qInstallMessageHandler(customMessageHandler);
}

void Logger::initLogs()
{
    m_initialized = true;

    if (Config::Csv::ENABLE_OUTPUT_CN && !m_detailedFnCn.isEmpty())
    {
        initDetailedLog(m_detailedFnCn);
    }
}

void Logger::initDetailedLog(const QString &filename)
{
    QFile dFile(filename);
    if (dFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Add BOM for Chinese (UTF-8 with BOM)
        const char bom[] = {(char)0xEF, (char)0xBB, (char)0xBF};
        dFile.write(bom, 3);

        QTextStream out(&dFile);
        // Header
        QString headerLine;
        headerLine = QString("姓名,%1,年龄,%2,性别,%3,班级,%4,时长,%5\n")
                             .arg(m_student.name)
                             .arg(m_student.age)
                             .arg(m_student.gender)
                             .arg(m_student.className)
                             .arg(m_student.duration);
        out << headerLine;
        out << "时间,模块,操作/日志内容\n";
        dFile.close();
    }
}

void Logger::writeBriefReport(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // BOM for CN
        const char bom[] = {(char)0xEF, (char)0xBB, (char)0xBF};
        file.write(bom, 3);

        QTextStream out(&file);

        // --- 1. Student Info ---
        QStringList infoParts;
        if (Config::Csv::LOG_STUDENT_NAME)
        {
            infoParts << QString("姓名,%1").arg(m_student.name);
        }
        if (Config::Csv::LOG_STUDENT_AGE)
        {
            infoParts << QString("年龄,%1").arg(m_student.age);
        }
        if (Config::Csv::LOG_STUDENT_GENDER)
        {
            infoParts << QString("性别,%1").arg(m_student.gender);
        }
        if (Config::Csv::LOG_STUDENT_CLASS)
        {
            infoParts << QString("班级,%1").arg(m_student.className);
        }
        if (Config::Csv::LOG_SESSION_DURATION)
        {
            infoParts << QString("时长,%1").arg(m_student.duration);
        }

        if (!infoParts.isEmpty())
        {
            out << infoParts.join(",") << "\n";
        }
        out << "----------------------------------------\n";

        // --- 2. Test 2 (Quiz) ---
        bool showTest2Header = Config::Csv::LOG_TEST2_SCORE || Config::Csv::LOG_TEST2_DETAILS || Config::Csv::LOG_TEST2_TIME_USED;
        if (showTest2Header) {
            out << "【测试2: 知识测验】\n";
        }

        if (Config::Csv::LOG_TEST2_SCORE)
        {
            out << QString("总分: %1 / %2\n").arg(test2Data.score).arg(test2Data.total);
        }
        if (Config::Csv::LOG_TEST2_TIME_USED)
        {
            out << QString("测试2用时: %1\n").arg(test2Data.timeUsed);
        }

        if (Config::Csv::LOG_TEST2_DETAILS)
        {
            out << "题目,选择,结果\n";
            for (const auto &q : test2Data.results)
            {
                QString correctStr = q.correct ? "正确" : "错误";
                QString qStr = QString("第%1题").arg(q.id);
                out << QString("%1,%2,%3\n").arg(qStr).arg(q.selection).arg(correctStr);
            }
            out << "\n";
        }

        // --- 3. Test 3 (RPG) ---
        // 'Task List' is now requested.
        bool showTest3Header = Config::Csv::LOG_TEST3_CLOCK || Config::Csv::LOG_TEST3_EMERGENCY || Config::Csv::LOG_TEST3_TASK_STATUS || Config::Csv::LOG_TEST3_TIME_USED;
        if (showTest3Header)
        {
            out << "【测试3: 模拟实训】\n";
        }

        if (Config::Csv::LOG_TEST3_TIME_USED)
        {
            out << QString("测试3用时: %1\n").arg(test3Data.timeUsed);
        }

        if (Config::Csv::LOG_TEST3_CLOCK)
        {
            QString lateStr = test3Data.isLate ? "迟到" : "正常";
            out << QString("上班打卡: %1 (%2)\n").arg(test3Data.clockInStatus).arg(lateStr);
            out << QString("下班打卡: %1\n").arg(test3Data.clockOutStatus);
        }

        if (Config::Csv::LOG_TEST3_EMERGENCY)
        {
            QString priorityStr = test3Data.emergencyPriorityMet ? "是" : "否";
            out << QString("紧急任务优先: %1\n").arg(priorityStr);
        }
        if (Config::Csv::LOG_TEST3_MIXED_LINEN)
        {
            QString mixedStr = test3Data.mixedLinen ? "是" : "否";
            out << QString("布草混装: %1\n").arg(mixedStr);
        }

        if (Config::Csv::LOG_TEST3_TASK_LIST)
        {
            out << "任务清单:\n";
            if (test3Data.taskList.isEmpty()) {
                out << "无任务\n";
            } else {
                out << test3Data.taskList << "\n";
            }
        }

        if (Config::Csv::LOG_TEST3_TASK_STATUS)
        {
            out << "楼层任务状态:\n";
            if (test3Data.floorStatuses.isEmpty())
            {
                out << "无任务数据\n";
            }
            else
            {
                for (const auto &f : test3Data.floorStatuses)
                {
                    QString statusStr = f.isCorrect ? "完成" : "未完成";
                    QString floorStr = QString("%1楼").arg(f.floor);
                    out << QString("%1: %2 (%3)\n").arg(floorStr).arg(statusStr).arg(f.details);
                }
            }
        }

        file.close();
    }
}
