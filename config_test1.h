#ifndef CONFIG_TEST1_H
#define CONFIG_TEST1_H

#include <QString>
#include <QList>
#include <QSize>
#include <QColor>
#include "config_text.h"

/**
 * @brief 测试1 (幻灯片) 配置
 * 包含图片路径、按钮文字、显示尺寸等。
 * 作用范围: Test1 模块
 */
namespace Config
{
    namespace Test1
    {

        // --- 资源路径 (Image Paths) ---
        // 幻灯片图片列表
        static const QList<QString> &SLIDE_IMAGES()
        {
            static const QList<QString> list = {
                "source/Test1/fig1.png",
                "source/Test1/fig2.png",
                "source/Test1/fig3.png",
                "source/Test1/fig4.png",
                "source/Test1/fig5.png",
                "source/Test1/fig6.png",
                "source/Test1/fig7.png",
                "source/Test1/fig8.png",
                "source/Test1/fig9.png",
                "source/Test1/fig10.png"};
            return list;
        }

        // --- 界面文本 (UI Texts) ---
        // 上一页按钮
        constexpr const char *BTN_TEXT_PREV = "上一页";
        // 下一页按钮
        constexpr const char *BTN_TEXT_NEXT = "下一页";
        // 完成按钮
        constexpr const char *BTN_TEXT_FINISH = "完成学习 (返回主菜单)";
        // 返回主菜单按钮 (右上角)
        constexpr const char *BTN_TEXT_BACK_TO_MENU = "返回主界面";

        // --- 尺寸配置 (Dimensions) ---
        // 幻灯片显示区域大小 (宽, 高)
        const QSize DISPLAY_SIZE(1200, 675);
        // 总结页缩略图大小 (宽, 高)
        const QSize THUMBNAIL_SIZE(300, 225);
        // 返回按钮尺寸 (原始设计尺寸，实际渲染会根据 BTN_HEIGHT_BASE 调整)
        const QSize RETURN_BTN_SIZE(120, 50);

        // --- 按钮高度缩放基准 (Button Height Base) ---
        // 新增: 控制按钮的垂直高度基准值 (Previous/Next/Finish/Return)
        // 实际高度 = BTN_HEIGHT_BASE * scaleFactor
        const int BTN_HEIGHT_BASE = 50; // 原为30，增加到50以防止太窄

        // --- 样式与颜色 (Styles) ---
        // 占位符背景颜色 (当图片缺失时显示)
        const QColor COL_PLACEHOLDER_BG = Qt::blue;
        // 缩略图边框样式
        constexpr const char *STYLE_THUMBNAIL_BORDER = "border: 0px solid #ccc;";
        // 占位符文字大小
        const int FONT_SIZE_PLACEHOLDER = 20;

        // 返回按钮样式 (动态生成)
        static inline QString GET_BTN_RETURN_STYLE() {
            return QString("background-color: #c0392b; color: %1; font-weight: bold; font-size: %2px;")
                   .arg(Config::Text::COLOR_TEST1_RETURN_BTN_TEXT)
                   .arg(Config::Text::SIZE_TEST1_RETURN_BTN);
        }

        // 导航按钮样式 (动态生成)
        static inline QString GET_BTN_NAV_STYLE() {
            return QString("font-size: %1px; color: %2;")
                   .arg(Config::Text::SIZE_TEST1_NAV_BTN)
                   .arg(Config::Text::COLOR_TEST1_NAV_BTN_TEXT);
        }
    }
}

#endif // CONFIG_TEST1_H
