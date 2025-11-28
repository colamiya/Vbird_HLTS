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
        // 默认字体家族 (MacOS Style)
        const QString FONT_FAMILY = "Segoe UI, Microsoft YaHei";
        // 默认字体大小
        const int FONT_SIZE_DEFAULT = 14;
        // 主菜单标题字体大小
        const int FONT_SIZE_TITLE = 32;
        // 子页面标题字体大小
        const int FONT_SIZE_SUBTITLE = 24;

        // --- 颜色主题 (Color Theme - MacOS Inspired) ---
        // 主背景色 (Off-white)
        const QString COL_BACKGROUND = "#fbfbfb";
        // 默认文字颜色 (Dark Gray)
        const QString COL_TEXT_PRIMARY = "#333333";
        // 按钮默认颜色 (Apple Blue)
        const QString COL_BTN_PRIMARY = "#007AFF";
        // 按钮悬停颜色 (Darker Blue)
        const QString COL_BTN_HOVER = "#0051a8";
        // 按钮禁用颜色 (Light Gray)
        const QString COL_BTN_DISABLED = "#d1d1d6";
        // 禁用文字颜色
        const QString COL_TEXT_DISABLED = "#8e8e93";
        // 输入框背景色 (White)
        const QString COL_INPUT_BG = "#ffffff";
        // 边框颜色 (Soft Gray)
        const QString COL_BORDER = "#d1d1d6";
        // 列表选中背景色 (Soft Blue)
        const QString COL_LIST_SELECTED = "#e5f1fb";

        // --- 全局样式表 (Global QSS) ---
        // 定义了主窗口、按钮、输入框、列表控件的默认样式。
        // MacOS 风格: 圆角, 扁平化, 柔和阴影 (Shadows are tricky in pure QSS without images, handled via clean borders)
        const QString GLOBAL_STYLESHEET = R"(
            QMainWindow { background-color: #fbfbfb; font-family: "Segoe UI", "Microsoft YaHei"; color: #333333; }

            QPushButton {
                background-color: #007AFF;
                color: white;
                border-radius: 10px;
                padding: 12px 24px;
                font-weight: 600;
                font-size: 15px;
                border: none;
            }
            QPushButton:hover { background-color: #0062cc; }
            QPushButton:pressed { background-color: #0051a8; }
            QPushButton:disabled { background-color: #e5e5ea; color: #8e8e93; }

            QLineEdit, QSpinBox, QComboBox {
                background-color: white;
                border: 1px solid #c7c7cc;
                border-radius: 8px;
                padding: 10px;
                font-size: 14px;
                color: #333333;
            }
            QLineEdit:focus, QSpinBox:focus, QComboBox:focus { border: 2px solid #007AFF; }

            QListWidget {
                background-color: white;
                border: 1px solid #d1d1d6;
                border-radius: 10px;
                outline: none;
            }
            QListWidget::item { padding: 12px; color: #333333; border-bottom: 1px solid #f2f2f7; }
            QListWidget::item:selected { background-color: #007AFF; color: white; border-radius: 6px; }
            QListWidget::item:hover { background-color: #f2f2f7; color: #333333; }
            QListWidget::item:selected:hover { background-color: #0062cc; color: white; }

            QLabel { color: #333333; font-size: 14px; }
        )";

        // --- 首页设置 (Start Page) ---
        // 首页标题文本
        const QString TITLE_START_PAGE = "学生基础信息登记";
        // 开始按钮文本
        const QString BTN_TEXT_START = "开始培训";
        // 突发事件复选框文本
        const QString CHECKBOX_TEXT_EMERGENCY = "启用突发事件";
        // 表单区域宽度 (控制输入框的整体宽度)
        const int SIZE_FORM_WIDTH = 420;
        // 开始按钮宽度
        const int SIZE_START_BTN_WIDTH = 220;

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
        const QSize SIZE_MENU_BTN(420, 90);
    }
}

#endif // CONFIG_GLOBAL_H
