#ifndef CONFIG_TEST2_H
#define CONFIG_TEST2_H

#include <QString>
#include <QSize>

/**
 * @brief 测试2 (选择题) 配置
 * 包含图片模板、按钮样式、文本内容等。
 */
namespace Config {
    namespace Test2 {

        // --- 资源路径模板 ---
        // 选项图片路径格式。 %1 会被替换为题目编号和选项字母 (如 "1A")
        const QString PATH_FMT_JPG = ":/source/Test2/%1.jpg";
        const QString PATH_FMT_PNG = ":/source/Test2/%1.png";

        // --- 界面文本 ---
        const QString BTN_TEXT_PREV = "上一题";
        const QString BTN_TEXT_NEXT = "下一题";
        const QString BTN_TEXT_SUBMIT = "提交测验";
        const QString BTN_TEXT_CLOSE_RESULT = "关闭并返回菜单";
        const QString MSG_WARNING_SELECT = "请先选择一个选项！";
        const QString TEXT_OPTION_PREFIX = "选项 "; // 如 "选项 A"
        const QString TEXT_NO_IMAGE = "(无图)";

        // --- 尺寸配置 ---
        // 选项图片按钮大小
        const QSize SIZE_OPTION_IMG(200, 150);
        // 选项文字按钮大小
        const QSize SIZE_OPTION_BTN(200, 40);
        // 图片图标实际大小 (略小于按钮以留白)
        const QSize SIZE_OPTION_ICON(200, 150);
        // 结果对话框大小
        const QSize SIZE_RESULT_DIALOG(600, 600);
        // 图片预览对话框大小
        const QSize SIZE_PREVIEW_DIALOG(600, 450);

        // --- 颜色与样式 ---
        // 提交按钮背景色 (绿色)
        const QString COL_BTN_SUBMIT = "#27ae60";
        const QString COL_BTN_TEXT_WHITE = "white";

        // 选项按钮样式表
        // 包含默认状态、选中状态、悬停状态
        const QString STYLE_OPTION_BTN =
            "QPushButton { background-color: white; border: 2px solid #ccc; color: #333; }"
            "QPushButton:checked { background-color: #3498db; border-color: #3498db; color: white; }"
            "QPushButton:hover { border-color: #3498db; }";

        // 图片按钮边框
        const QString STYLE_IMG_BTN = "border: 1px solid #ccc;";

        // 题目文字样式
        const QString STYLE_QUESTION_LBL = "font-size: 18px; font-weight: bold;";

        // --- HTML 报告样式 ---
        const QString HTML_TABLE_STYLE = "<style>table { border-collapse: collapse; width: 100%; } th, td { border: 1px solid #ddd; padding: 8px; text-align: left; } th { background-color: #f2f2f2; }</style>";
        const QString HTML_CORRECT_COLOR = "green";
        const QString HTML_WRONG_COLOR = "red";
    }
}

#endif // CONFIG_TEST2_H
