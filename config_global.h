#ifndef CONFIG_GLOBAL_H
#define CONFIG_GLOBAL_H

#include <QString>
#include <QSize>
#include <QColor>

/**
 * @brief 全局通用配置
 * 包含整个软件的主窗口设置、全局样式、通用颜色等。
 * 作用范围: 整个应用程序 (App-wide)
 */
namespace Config {
    namespace Global {

        // --- 窗口设置 (Window Settings) ---
        // 应用程序标题
        const QString APP_TITLE = "XXX培训系统";
        // 窗口大小 (宽, 高)
        const QSize WINDOW_SIZE(1440, 900);

        // --- 字体设置 (Fonts) ---
        // 默认字体家族
        const QString FONT_FAMILY = "Segoe UI";
        // 默认字体大小
        const int FONT_SIZE_DEFAULT = 14;
        // 主菜单标题字体大小
        const int FONT_SIZE_TITLE = 28;
        // 子页面标题字体大小
        const int FONT_SIZE_SUBTITLE = 24;

        // --- 颜色主题 (Color Theme) ---
        // 主背景色 (浅灰)
        const QString COL_BACKGROUND = "#f4f6f9";
        // 默认文字颜色 (深蓝灰)
        const QString COL_TEXT_PRIMARY = "#2c3e50";
        // 按钮默认颜色 (蓝色)
        const QString COL_BTN_PRIMARY = "#3498db";
        // 按钮悬停颜色 (深蓝)
        const QString COL_BTN_HOVER = "#2980b9";
        // 按钮禁用颜色 (灰色)
        const QString COL_BTN_DISABLED = "#bdc3c7";
        // 禁用文字颜色
        const QString COL_TEXT_DISABLED = "#7f8c8d";
        // 输入框背景色 (白色)
        const QString COL_INPUT_BG = "white";
        // 边框颜色 (淡灰)
        const QString COL_BORDER = "#dfe6e9";
        // 列表选中背景色 (淡蓝)
        const QString COL_LIST_SELECTED = "#e1f0fa";

        // --- 全局样式表 (Global QSS) ---
        // 定义了主窗口、按钮、输入框、列表控件的默认样式。
        // 如需修改整体风格，请在此处调整 CSS 代码。
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

        // --- 首页设置 (Start Page) ---
        // 首页标题文本
        const QString TITLE_START_PAGE = "学生基础信息登记";
        // 开始按钮文本
        const QString BTN_TEXT_START = "开始培训";
        // 突发事件复选框文本
        const QString CHECKBOX_TEXT_EMERGENCY = "启用突发事件";
        // 表单区域宽度 (控制输入框的整体宽度)
        const int SIZE_FORM_WIDTH = 400;
        // 开始按钮宽度
        const int SIZE_START_BTN_WIDTH = 200;

        // --- 主菜单设置 (Main Menu) ---
        // 主菜单标题
        const QString TITLE_MAIN_MENU = "请选择测试项";
        // 测试1 按钮文本
        const QString BTN_TEXT_TEST1 = "测试 1: 业务学习 (幻灯片)";
        // 测试2 按钮文本
        const QString BTN_TEXT_TEST2 = "测试 2: 知识测验 (选择题)";
        // 测试3 按钮文本
        const QString BTN_TEXT_TEST3 = "测试 3: 模拟实训 (RPG)";
        // 开发者模式复选框文本
        const QString CHECKBOX_TEXT_DEV_MODE = "开发者模式";
        // 菜单按钮尺寸 (宽, 高)
        const QSize SIZE_MENU_BTN(400, 80);
    }
}

#endif // CONFIG_GLOBAL_H
