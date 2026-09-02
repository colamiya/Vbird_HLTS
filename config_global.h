#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#include <QString>
#include <QSize>
#include <QColor>
#include "config_text.h" // 引用文本配置

/**
 * @brief 全局通用配置
 * 包含整个软件的主窗口设置、全局样式、通用颜色等。
 * 作用范围: 整个应用程序 (App-wide)
 */
namespace Config
{
    namespace Global
    {

        // --- 窗口设置 (Window Settings) ---
        // 应用程序标题
        constexpr const char *APP_TITLE = "酒店布草员模拟实训系统";
        // 窗口大小 (宽, 高)
        const QSize WINDOW_SIZE(1750, 900);

        // --- 弹窗与对话框配置 (Dialogs) ---
        // 新增: 弹窗按钮的尺寸基准
        // 实际大小通常固定或随内容调整，这里设定最小尺寸以确保不显得太小
        const int DIALOG_BTN_MIN_WIDTH = 100;
        const int DIALOG_BTN_MIN_HEIGHT = 45; // 增加高度 (原系统默认通常较小)
        const int DIALOG_BTN_FONT_SIZE = 16;

        // --- 颜色主题 (Color Theme - MacOS Inspired) ---
        // 主背景色 (Off-white)
        constexpr const char *COL_BACKGROUND = "#fbfbfb";

        // 按钮默认颜色 (Apple Blue)
        constexpr const char *COL_BTN_PRIMARY = "#007AFF";
        // 按钮悬停颜色 (Darker Blue)
        constexpr const char *COL_BTN_HOVER = "#0051a8";
        // 按钮禁用颜色 (Light Gray)
        constexpr const char *COL_BTN_DISABLED = "#d1d1d6";
        // 文本禁用颜色
        constexpr const char *COL_TEXT_DISABLED = "#8e8e93";
        // 输入框背景色 (White)
        constexpr const char *COL_INPUT_BG = "#ffffff";
        // 边框颜色 (Soft Gray)
        constexpr const char *COL_BORDER = "#d1d1d6";
        // 列表选中背景色 (Soft Blue)
        constexpr const char *COL_LIST_SELECTED = "#e5f1fb";

        // --- 全局样式表 (Global QSS) ---
        // 动态生成样式表以支持 Config::Text 中的配置
        static inline QString GET_GLOBAL_STYLESHEET()
        {
            return QString(R"(
                QMainWindow {
                    background-color: %1;
                    font-family: "%2";
                    color: %3;
                    font-size: %4px;
                }

                QPushButton {
                    background-color: %5;
                    color: white;
                    border-radius: 6px;
                    padding: 0px;
                    font-weight: 600;
                    border: none;
                }
                QPushButton:hover { background-color: %6; }
                QPushButton:pressed { background-color: %7; }
                QPushButton:disabled { background-color: %8; color: %9; }

                QLineEdit, QSpinBox, QComboBox {
                    background-color: %10;
                    border: 1px solid %11;
                    border-radius: 8px;
                    padding: 10px;
                    font-size: %12px;
                    color: %13;
                }
                QLineEdit:focus, QSpinBox:focus, QComboBox:focus { border: 2px solid %5; }

                QListWidget {
                    background-color: white;
                    border: 1px solid %11;
                    border-radius: 10px;
                    outline: none;
                }
                QListWidget::item { padding: 12px; color: %3; border-bottom: 1px solid #f2f2f7; }
                QListWidget::item:selected { background-color: %5; color: white; border-radius: 6px; }
                QListWidget::item:hover { background-color: #f2f2f7; color: %3; }
                QListWidget::item:selected:hover { background-color: %6; color: white; }

                QLabel { color: %3; font-size: %4px; }
            )")
            .arg(COL_BACKGROUND)                             // 1
            .arg(Config::Text::FONT_FAMILY)                  // 2
            .arg(Config::Text::COLOR_DEFAULT)                // 3
            .arg(Config::Text::SIZE_DEFAULT)                 // 4
            .arg(COL_BTN_PRIMARY)                            // 5
            .arg(COL_BTN_HOVER)                              // 6
            .arg(COL_BTN_HOVER)                              // 7 (Pressed)
            .arg(COL_BTN_DISABLED)                           // 8
            .arg(Config::Text::COLOR_DISABLED)               // 9
            .arg(COL_INPUT_BG)                               // 10
            .arg(COL_BORDER)                                 // 11
            .arg(Config::Text::SIZE_START_INPUT)             // 12 (Use input size default)
            .arg(Config::Text::COLOR_START_INPUT);           // 13
        }

        // --- 首页设置 (Start Page) ---
        // 首页标题文本
        constexpr const char *TITLE_START_PAGE = "学生基础信息登记";
        // 开始按钮文本
        constexpr const char *BTN_TEXT_START = "开始培训";
        // 突发事件复选框文本
        constexpr const char *CHECKBOX_TEXT_EMERGENCY = "启用突发事件";
        // 表单区域宽度 (控制输入框的整体宽度)
        const int SIZE_FORM_WIDTH = 320;
        // 开始按钮宽度
        const int SIZE_START_BTN_WIDTH = 220;

        // --- 主菜单设置 (Main Menu) ---
        // 主菜单标题
        constexpr const char *TITLE_MAIN_MENU = "请选择测试项";
        // 测试1 按钮文本
        constexpr const char *BTN_TEXT_TEST1 = "测试 1: 业务学习 (幻灯片)";
        // 测试2 按钮文本
        constexpr const char *BTN_TEXT_TEST2 = "测试 2: 知识测验 (选择题)";
        // 测试3 按钮文本
        constexpr const char *BTN_TEXT_TEST3 = "测试 3: 模拟实训 (RPG)";
        // 开发者模式复选框文本
        constexpr const char *CHECKBOX_TEXT_DEV_MODE = "开发者模式";
        // 菜单按钮尺寸 (宽, 高)
        const QSize SIZE_MENU_BTN(420, 90);
    }

    /**
     * @brief CSV 报表配置
     * 控制报表的生成语言、文件名以及包含的字段。
     */
    namespace Csv
    {
        // --- 输出控制 (Output Control) ---
        // 是否生成中文报表 (带BOM, 适合Excel打开不乱码)
        const bool ENABLE_OUTPUT_CN = true;

        // --- 文件名配置 (Filenames) ---
        // 中文简报文件名
        constexpr const char *FILENAME_BRIEF_CN = "brief_report.csv";
        // 中文详细日志文件名
        constexpr const char *FILENAME_DETAILED_CN = "detailed_report.csv";

        // --- 字段开关 (Field Toggles) ---
        // 学生基础信息
        const bool LOG_STUDENT_NAME = false;    // 姓名（涉及个人信息，默认关闭）
        const bool LOG_STUDENT_AGE = false;     // 年龄（涉及个人信息，默认关闭）
        const bool LOG_STUDENT_GENDER = false;  // 性别（涉及个人信息，默认关闭）
        const bool LOG_STUDENT_CLASS = false;   // 班级（涉及个人信息，默认关闭）
        const bool LOG_SESSION_DURATION = true; // 时长 (总体)
        const bool INCLUDE_STUDENT_NAME_IN_FILENAME = false; // 文件名默认匿名

        // 测试2 (知识测验)
        const bool LOG_TEST2_SCORE = true;       // 总分
        const bool LOG_TEST2_DETAILS = true;     // 题目详情
        const bool LOG_TEST2_TIME_USED = true;   // 测试2耗时 (新增)

        // 测试3 (实训考核)
        const bool LOG_TEST3_CLOCK = true;          // 打卡状态 (是否正常/迟到/忘记打卡)
        const bool LOG_TEST3_TASK_STATUS = true;    // 任务完成情况 (是否完成)
        const bool LOG_TEST3_TASK_LIST = true;      // 任务清单 (具体分配的物品) (新增)
        const bool LOG_TEST3_MIXED_LINEN = true;    // 是否有脏布草混装 (新增细化)
        const bool LOG_TEST3_EMERGENCY = true;      // 紧急任务是否优先完成 (新增细化)
        const bool LOG_TEST3_TIME_USED = true;      // 测试3总用时 (新增)
    }
}

#endif // CONFIG_GLOBAL_H
