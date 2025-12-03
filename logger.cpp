#include "logger.h"

Logger::Logger() : m_initialized(false)
{
}

// Write to Detailed Log (Action history)
void Logger::logAction(const QString &module, const QString &action)
{
    if (!m_initialized)
        return;
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // Write to CN Log
    if (Config::Csv::ENABLE_OUTPUT_CN && !m_detailedFnCn.isEmpty())
    {
        QString line = QString("%1,%2,%3").arg(timestamp, module, action);
        QFile file(m_detailedFnCn);
        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&file);
            out << line << "\n";
            file.close();
        }
    }

    // Write to EN Log
    if (Config::Csv::ENABLE_OUTPUT_EN && !m_detailedFnEn.isEmpty())
    {
        QString line = QString("%1,%2,%3").arg(timestamp, module, action);
        QFile file(m_detailedFnEn);
        if (file.open(QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&file);
            out << line << "\n";
            file.close();
        }
    }
}

void Logger::generateBriefReport()
{
    if (!m_initialized)
        return;

    if (Config::Csv::ENABLE_OUTPUT_CN && !m_briefFnCn.isEmpty())
    {
        writeBriefReport(m_briefFnCn, true);
    }
    if (Config::Csv::ENABLE_OUTPUT_EN && !m_briefFnEn.isEmpty())
    {
        writeBriefReport(m_briefFnEn, false);
    }
}

void Logger::startNewSession()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");

    // Helper to insert timestamp before extension
    auto insertTimestamp = [&](const char *filename) -> QString
    {
        QString fn = QString(filename);
        int dotIndex = fn.lastIndexOf('.');
        if (dotIndex != -1)
        {
            return fn.left(dotIndex) + "_" + timestamp + fn.mid(dotIndex);
        }
        return fn + "_" + timestamp;
    };

    if (Config::Csv::ENABLE_OUTPUT_CN)
    {
        m_detailedFnCn = insertTimestamp(Config::Csv::FILENAME_DETAILED_CN);
        m_briefFnCn = insertTimestamp(Config::Csv::FILENAME_BRIEF_CN);
    }
    if (Config::Csv::ENABLE_OUTPUT_EN)
    {
        m_detailedFnEn = insertTimestamp(Config::Csv::FILENAME_DETAILED_EN);
        m_briefFnEn = insertTimestamp(Config::Csv::FILENAME_BRIEF_EN);
    }

    initLogs();
}

void Logger::initLogs()
{
    m_initialized = true;

    if (Config::Csv::ENABLE_OUTPUT_CN && !m_detailedFnCn.isEmpty())
    {
        initDetailedLog(m_detailedFnCn, true);
    }
    if (Config::Csv::ENABLE_OUTPUT_EN && !m_detailedFnEn.isEmpty())
    {
        initDetailedLog(m_detailedFnEn, false);
    }
}

void Logger::initDetailedLog(const QString &filename, bool isCn)
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

void Logger::writeBriefReport(const QString &filename, bool isCn)
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
