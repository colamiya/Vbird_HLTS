#ifndef CONFIG_TEST1_H
#define CONFIG_TEST1_H

#include <QString>
#include <QList>
#include <QSize>
#include <QColor>

/**
 * @brief 测试1 (幻灯片) 配置
 * 包含图片路径、按钮文字、显示尺寸等。
 */
namespace Config {
    namespace Test1 {

        // --- 资源路径 ---
        // 幻灯片图片列表
        // 按照顺序播放。可在此处添加或移除图片路径。
        const QList<QString> SLIDE_IMAGES = {
            ":/source/Test1/fig1.png",
            ":/source/Test1/fig2.png",
            ":/source/Test1/fig3.png",
            ":/source/Test1/fig4.png",
            ":/source/Test1/fig5.png",
            ":/source/Test1/fig6.png",
            ":/source/Test1/fig7.png",
            ":/source/Test1/fig8.png",
            ":/source/Test1/fig9.png",
            ":/source/Test1/fig10.png"
        };

        // --- 界面文本 ---
        const QString BTN_TEXT_PREV = "上一页";
        const QString BTN_TEXT_NEXT = "下一页";
        const QString BTN_TEXT_FINISH = "完成学习 (返回主菜单)";

        // --- 尺寸配置 ---
        // 幻灯片显示区域大小 (宽, 高)
        const QSize DISPLAY_SIZE(800, 450);
        // 总结页缩略图大小 (宽, 高)
        const QSize THUMBNAIL_SIZE(200, 150);

        // --- 样式与颜色 ---
        // 占位符颜色 (当图片缺失时显示)
        const QColor COL_PLACEHOLDER_BG = Qt::blue;
        // 缩略图边框样式
        const QString STYLE_THUMBNAIL_BORDER = "border: 1px solid #ccc;";
        // 占位符文字大小
        const int FONT_SIZE_PLACEHOLDER = 20;
    }
}

#endif // CONFIG_TEST1_H
