#ifndef CONFIG_TEST2_H
#define CONFIG_TEST2_H

#include <QString>
#include <QSize>

/**
 * @brief 测试2 (选择题) 配置
 * 包含图片模板、按钮样式、文本内容等。
 * 作用范围: Test2 模块
 */
namespace Config {
    namespace Test2 {

        // --- 资源路径模板 (Path Templates) ---
        // 选项图片路径格式。 %1 会被替换为题目编号和选项字母 (如 "1A")
        // 例如: ":/source/Test2/1A.jpg"
        constexpr const char* PATH_FMT_JPG = ":/source/Test2/%1.jpg";

        // --- 界面文本 (UI Texts) ---
        // 导航按钮
        constexpr const char* BTN_TEXT_PREV = "上一题";
        constexpr const char* BTN_TEXT_NEXT = "下一题";
        constexpr const char* BTN_TEXT_SUBMIT = "提交测验";
        constexpr const char* BTN_TEXT_CLOSE_RESULT = "关闭并返回菜单";
        // 返回主菜单按钮 (右上角)
        constexpr const char* BTN_TEXT_BACK_TO_MENU = "返回主界面";
        // 提示信息
        constexpr const char* MSG_WARNING_SELECT = "请先选择一个选项！";
        constexpr const char* TEXT_OPTION_PREFIX = "选项 "; // 如 "选项 A"
        constexpr const char* TEXT_NO_IMAGE = "(无图)";

        // --- 尺寸配置 (Dimensions) ---
        // 选项图片按钮大小 (放大显示)
        const QSize SIZE_OPTION_IMG(360, 270);
        // 选项文字按钮大小 (宽度需与图片一致)
        const QSize SIZE_OPTION_BTN(360, 50);
        // 图片图标实际大小 (与按钮保持一致或略小)
        const QSize SIZE_OPTION_ICON(350, 260);
        // 结果对话框大小 (放大)
        const QSize SIZE_RESULT_DIALOG(800, 700);
        // 图片预览对话框大小 (灯箱)
        const QSize SIZE_PREVIEW_DIALOG(800, 600);
        // 返回按钮尺寸
        const QSize RETURN_BTN_SIZE(120, 40);

        // --- 颜色与样式 (Colors & Styles) ---
        // 提交按钮背景色 (绿色)
        constexpr const char* COL_BTN_SUBMIT = "#27ae60";
        // 白色文字
        constexpr const char* COL_BTN_TEXT_WHITE = "white";

        // 选项按钮样式表
        // 包含默认状态、选中状态、悬停状态
        constexpr const char* STYLE_OPTION_BTN =
            "QPushButton { background-color: white; border: 2px solid #ccc; color: #333; font-size: 16px; }"
            "QPushButton:checked { background-color: #3498db; border-color: #3498db; color: white; }"
            "QPushButton:hover { border-color: #3498db; }";

        // 图片按钮样式 (透明背景，无边框)
        constexpr const char* STYLE_IMG_BTN = "border: none; background-color: transparent;";

        // 题目文字样式
        constexpr const char* STYLE_QUESTION_LBL = "font-size: 24px; font-weight: bold; margin-bottom: 20px;";
        // 返回按钮样式
        constexpr const char* BTN_RETURN_STYLE = "background-color: #c0392b; color: white; font-weight: bold;";

        // --- HTML 报告样式 (HTML Report) ---
        constexpr const char* HTML_TABLE_STYLE = "<style>table { border-collapse: collapse; width: 100%; font-size: 16px; } th, td { border: 1px solid #ddd; padding: 12px; text-align: center; } th { background-color: #f2f2f2; }</style>";

        // 结果颜色代码
        constexpr const char* HTML_CORRECT_COLOR = "green";
        constexpr const char* HTML_WRONG_COLOR = "red";

        // 分数颜色等级
        constexpr const char* COL_SCORE_LOW = "#e74c3c";    // 红色 (低分)
        constexpr const char* COL_SCORE_MID = "#f39c12";    // 橙色 (中等)
        constexpr const char* COL_SCORE_HIGH = "#27ae60";   // 绿色 (满分/高分)
    }
}

#endif // CONFIG_TEST2_H