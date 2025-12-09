#ifndef CONFIG_TEST1_H
#define CONFIG_TEST1_H

#include <QString>
#include <QList>
#include <QSize>
#include <QColor>

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
        // 按照顺序播放。可在此处添加或移除图片路径。
        static const QList<QString> &SLIDE_IMAGES()
        {
            static const QList<QString> list = {
                ":/source/Test1/fig1.png",
                ":/source/Test1/fig2.png",
                ":/source/Test1/fig3.png",
                ":/source/Test1/fig4.png",
                ":/source/Test1/fig5.png",
                ":/source/Test1/fig6.png",
                ":/source/Test1/fig7.png",
                ":/source/Test1/fig8.png",
                ":/source/Test1/fig9.png",
                ":/source/Test1/fig10.png"};
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
        const QSize THUMBNAIL_SIZE(300, 225); // Increased size
        // 返回按钮尺寸
        const QSize RETURN_BTN_SIZE(120, 40);

        // --- 样式与颜色 (Styles) ---
        // 占位符背景颜色 (当图片缺失时显示)
        const QColor COL_PLACEHOLDER_BG = Qt::blue;
        // 缩略图边框样式
        constexpr const char *STYLE_THUMBNAIL_BORDER = "border: 0px solid #ccc;";
        // 占位符文字大小
        const int FONT_SIZE_PLACEHOLDER = 20;
        // 返回按钮样式
        constexpr const char *BTN_RETURN_STYLE = "background-color: #c0392b; color: white; font-weight: bold;";
    }
}

#endif // CONFIG_TEST1_H
