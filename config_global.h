#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#include <QString>
#include <QSize>
#include <QColor>

/**
 * @brief 全局通用配置
 * 包含整个软件的主窗口设置、全局样式、通用颜色等。
 */
namespace Config {
    namespace Global {

        // --- 窗口设置 ---
        const QString APP_TITLE = "酒店管理学生实训系统";
        const QSize WINDOW_SIZE(1280, 720);

        // --- 字体设置 ---
        const QString FONT_FAMILY = "Microsoft YaHei";
        const int FONT_SIZE_DEFAULT = 14;
        const int FONT_SIZE_TITLE = 28; // 主菜单标题
        const int FONT_SIZE_SUBTITLE = 24; // 子页面标题

        // --- 颜色主题 ---
        // 主背景色
        const QString COL_BACKGROUND = "#f4f6f9";
        // 默认文字颜色
        const QString COL_TEXT_PRIMARY = "#2c3e50";
        // 按钮默认蓝色
        const QString COL_BTN_PRIMARY = "#3498db";
        // 按钮悬停颜色
        const QString COL_BTN_HOVER = "#2980b9";
        // 按钮禁用颜色
        const QString COL_BTN_DISABLED = "#bdc3c7";
        // 禁用文字颜色
        const QString COL_TEXT_DISABLED = "#7f8c8d";
        // 输入框背景
        const QString COL_INPUT_BG = "white";
        // 边框颜色
        const QString COL_BORDER = "#dfe6e9";
        // 列表选中背景
        const QString COL_LIST_SELECTED = "#e1f0fa";

        // --- 全局样式表 (QSS) ---
        // 注意：这里使用了上面定义的颜色变量的字面量值。
        // 如果修改了上面的颜色常量，请同步修改下面的字符串中的颜色代码。
        // 或者在代码中通过 QString::arg 进行动态替换。
        const QString GLOBAL_STYLESHEET = R"(
            QMainWindow { background-color: #f4f6f9; font-family: "Microsoft YaHei"; color: #2c3e50; }
            QPushButton { background-color: #3498db; color: white; border-radius: 6px; padding: 10px 20px; font-weight: 600; font-size: 14px; }
            QPushButton:hover { background-color: #2980b9; }
            QPushButton:disabled { background-color: #bdc3c7; color: #7f8c8d; }
            QLineEdit, QSpinBox, QComboBox { background-color: white; border: 2px solid #dfe6e9; border-radius: 6px; padding: 8px; }
            QListWidget { background-color: white; border: 2px solid #dfe6e9; border-radius: 6px; outline: none; }
            QListWidget::item { padding: 10px; color: #2c3e50; }
            QListWidget::item:selected { background-color: #e1f0fa; color: #2c3e50; }
        )";

        // --- 首页设置 ---
        const QString TITLE_START_PAGE = "学生基础信息登记";
        const QString BTN_TEXT_START = "开始培训";
        const QString CHECKBOX_TEXT_EMERGENCY = "启用突发事件 (Enable Emergency)";
        // 表单区域宽度
        const int SIZE_FORM_WIDTH = 400;
        // 开始按钮宽度
        const int SIZE_START_BTN_WIDTH = 200;

        // --- 主菜单设置 ---
        const QString TITLE_MAIN_MENU = "请选择测试项";
        const QString BTN_TEXT_TEST1 = "测试 1: 业务学习 (幻灯片)";
        const QString BTN_TEXT_TEST2 = "测试 2: 知识测验 (选择题)";
        const QString BTN_TEXT_TEST3 = "测试 3: 模拟实训 (RPG)";
        const QString CHECKBOX_TEXT_DEV_MODE = "开发者模式";
        const QSize SIZE_MENU_BTN(400, 80);
    }
}

#endif // CONFIG_GLOBAL_H
