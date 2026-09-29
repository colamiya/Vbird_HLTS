#include "logger.h"
#include <cstdio>
#include <QRegularExpression>
#include <QStringList>

namespace
{
QString csvCell(QString value)
{
    int firstContent = 0;
    while (firstContent < value.size() && value.at(firstContent).isSpace())
        ++firstContent;

    if (firstContent < value.size())
    {
        const QChar first = value.at(firstContent);
        if (first == '=' || first == '+' || first == '-' || first == '@' || first == '\t' || first == '\r' || first == '\n')
            value.prepend('\'');
    }

    value.replace("\"", "\"\"");
    return "\"" + value + "\"";
}

QString csvRow(const QStringList &cells)
{
    QStringList encoded;
    encoded.reserve(cells.size());
    for (const QString &cell : cells)
        encoded << csvCell(cell);
    return encoded.join(',');
}

QString safeFilenamePart(QString value)
{
    value = value.trimmed();
    value.replace(QRegularExpression("[\\\\/:*?\"<>|\\x00-\\x1F\\x7F]"), "_");
    return value.left(64);
}
}

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
        QString line = csvRow({timestamp, module, action});
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
    // Disable file logging for System messages as per requirement
    // Only print to stderr (done by customMessageHandler or here if needed, but handled by caller mainly)
    return;

    /*
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
    */
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
        QString prefix = timestamp;
        if (Config::Csv::INCLUDE_STUDENT_NAME_IN_FILENAME && !m_student.name.trimmed().isEmpty())
        {
            prefix += "_" + safeFilenamePart(m_student.name);
        }
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
        QStringList infoCells;
        if (Config::Csv::LOG_STUDENT_NAME)
            infoCells << "姓名" << m_student.name;
        if (Config::Csv::LOG_STUDENT_AGE)
            infoCells << "年龄" << QString::number(m_student.age);
        if (Config::Csv::LOG_STUDENT_GENDER)
            infoCells << "性别" << m_student.gender;
        if (Config::Csv::LOG_STUDENT_CLASS)
            infoCells << "班级" << m_student.className;
        if (Config::Csv::LOG_SESSION_DURATION)
            infoCells << "时长" << m_student.duration;
        if (!infoCells.isEmpty())
            out << csvRow(infoCells) << "\n";
        out << csvRow({"时间", "模块", "操作/日志内容"}) << "\n";
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
            infoParts << "姓名" << m_student.name;
        }
        if (Config::Csv::LOG_STUDENT_AGE)
        {
            infoParts << "年龄" << QString::number(m_student.age);
        }
        if (Config::Csv::LOG_STUDENT_GENDER)
        {
            infoParts << "性别" << m_student.gender;
        }
        if (Config::Csv::LOG_STUDENT_CLASS)
        {
            infoParts << "班级" << m_student.className;
        }
        if (Config::Csv::LOG_SESSION_DURATION)
        {
            infoParts << "时长" << m_student.duration;
        }

        if (!infoParts.isEmpty())
        {
            out << csvRow(infoParts) << "\n";
        }
        out << csvRow({"----------------------------------------"}) << "\n";

        // --- 2. Test 2 (Quiz) ---
        bool showTest2Header = Config::Csv::LOG_TEST2_SCORE || Config::Csv::LOG_TEST2_DETAILS || Config::Csv::LOG_TEST2_TIME_USED;
        if (showTest2Header) {
            out << csvRow({"【测试2: 知识测验】"}) << "\n";
        }

        if (Config::Csv::LOG_TEST2_SCORE)
        {
            out << csvRow({"总分", QString("%1 / %2").arg(test2Data.score).arg(test2Data.total)}) << "\n";
        }
        if (Config::Csv::LOG_TEST2_TIME_USED)
        {
            out << csvRow({"测试2用时", test2Data.timeUsed}) << "\n";
        }

        if (Config::Csv::LOG_TEST2_DETAILS)
        {
            out << csvRow({"题目", "选择", "结果"}) << "\n";
            for (const auto &q : test2Data.results)
            {
                QString correctStr = q.correct ? "正确" : "错误";
                QString qStr = QString("第%1题").arg(q.id);
                out << csvRow({qStr, q.selection, correctStr}) << "\n";
            }
            out << "\n";
        }

        // --- 3. Test 3 (RPG) ---
        // 'Task List' is now requested.
        bool showTest3Header = Config::Csv::LOG_TEST3_CLOCK || Config::Csv::LOG_TEST3_EMERGENCY || Config::Csv::LOG_TEST3_TASK_STATUS || Config::Csv::LOG_TEST3_TIME_USED;
        if (showTest3Header)
        {
            out << csvRow({"【测试3: 模拟实训】"}) << "\n";
        }

        if (Config::Csv::LOG_TEST3_TIME_USED)
        {
            out << csvRow({"测试3用时", test3Data.timeUsed}) << "\n";
        }

        if (Config::Csv::LOG_TEST3_CLOCK)
        {
            QString lateStr = test3Data.isLate ? "迟到" : "正常";
            out << csvRow({"上班打卡", test3Data.clockInStatus, lateStr}) << "\n";
            out << csvRow({"下班打卡", test3Data.clockOutStatus}) << "\n";
        }

        if (Config::Csv::LOG_TEST3_EMERGENCY)
        {
            QString priorityStr = test3Data.emergencyPriorityMet ? "是" : "否";
            out << csvRow({"紧急任务优先", priorityStr}) << "\n";
        }
        if (Config::Csv::LOG_TEST3_MIXED_LINEN)
        {
            QString mixedStr = test3Data.mixedLinen ? "是" : "否";
            out << csvRow({"布草混装", mixedStr}) << "\n";
        }

        if (Config::Csv::LOG_TEST3_TASK_LIST)
        {
            out << csvRow({"任务清单详情"}) << "\n";
            if (test3Data.detailedTasks.isEmpty()) {
                out << csvRow({"无任务"}) << "\n";
            } else {
                for(int i=0; i<test3Data.detailedTasks.size(); ++i) {
                    const auto& task = test3Data.detailedTasks[i];
                    QString title = QString("任务%1 (%2层)%3").arg(i+1).arg(task.floor).arg(task.isEmergency ? " [紧急]" : "");
                    out << csvRow({title}) << "\n";
                    out << csvRow({"物品", "需求量", "学生标记", "完成结果"}) << "\n";
                    for(const auto& item : task.items) {
                        QString markedStr = item.isMarked ? "是" : "否";
                        out << csvRow({item.name, QString::number(item.required), markedStr, item.resultStatus}) << "\n";
                    }
                    out << "\n"; // Empty line between tasks
                }
            }
        }

        if (Config::Csv::LOG_TEST3_TASK_STATUS)
        {
            out << csvRow({"楼层任务状态 (汇总)"}) << "\n";
            if (test3Data.floorStatuses.isEmpty())
            {
                out << csvRow({"无任务数据"}) << "\n";
            }
            else
            {
                for (const auto &f : test3Data.floorStatuses)
                {
                    QString statusStr = f.isCorrect ? "完成" : "未完成";
                    QString floorStr = QString("%1楼").arg(f.floor);
                    out << csvRow({floorStr, statusStr, f.details}) << "\n";
                }
            }
        }

        file.close();
    }
}
