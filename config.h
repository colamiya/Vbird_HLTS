#ifndef CONFIG_H
#define CONFIG_H

#include <QString>
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QMap>
#include <QList>

/**
 * @brief 全局配置命名空间
 * 包含所有模块的图片路径、UI尺寸、坐标等常量配置。
 * 修改此文件中的值即可调整软件的显示和行为。
 */
namespace Config {

    namespace MainWindow {
        // 主窗口标题
        const QString TITLE = "酒店管理学生实训系统";
        // 主窗口默认大小 (宽, 高)
        const QSize WINDOW_SIZE(1280, 720);

        // 开始页标题
        const QString START_TITLE = "学生基础信息登记";
        // 开始页表单宽度
        const int FORM_WIDTH = 400;
        // 开始页按钮宽度
        const int BTN_WIDTH = 200;

        // 主菜单按钮大小 (宽, 高)
        const QSize MENU_BTN_SIZE(400, 80);
    }

    namespace Test1 {
        // 幻灯片图片列表
        // 如果需要增加幻灯片，在此处添加新的路径即可
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

        // 幻灯片显示区域大小 (宽, 高) - 保持长宽比缩放
        const QSize SLIDE_DISPLAY_SIZE(800, 450);

        // 总结页略缩图大小 (宽, 高)
        const QSize THUMBNAIL_SIZE(200, 150);
    }

    namespace Test2 {
        // 选项图片默认路径模板 (代码中会根据题目索引组合文件名)
        // 例如: :/source/Test2/1A.jpg
        // 若要修改路径格式，请修改此字符串。 %1=文件名(如1A)
        const QString IMG_PATH_FMT_JPG = ":/source/Test2/%1.jpg";
        const QString IMG_PATH_FMT_PNG = ":/source/Test2/%1.png";

        // 选项图片按钮大小 (宽, 高)
        const QSize OPTION_IMG_SIZE(200, 150);
        // 选项文字按钮大小 (宽, 高)
        const QSize OPTION_BTN_SIZE(200, 40);
        // 图片图标尺寸 (宽, 高) - 略小于按钮大小以留边距
        const QSize OPTION_ICON_SIZE(190, 140);
    }

    namespace Test3 {
        // --- 图片路径 ---
        namespace Images {
            // 场景背景图
            const QString SCENE_ENTRANCE = ":/source/Test3/入口.jpg";
            const QString SCENE_HALLWAY = ":/source/Test3/员工通道走廊.jpg";
            const QString SCENE_OFFICE = ":/source/Test3/办公室.png";
            const QString SCENE_WAREHOUSE_ENTRY = ":/source/Test3/仓库1.jpg";
            const QString SCENE_WAREHOUSE_SHELF = ":/source/Test3/取布草的货架.jpg";
            const QString SCENE_ELEVATOR_HALL = ":/source/Test3/电梯厅.jpg";
            const QString SCENE_ELEVATOR_INSIDE = ":/source/Test3/电梯内.jpg";
            const QString SCENE_FLOOR_CORRIDOR = ":/source/Test3/楼层走廊-前.png";
            const QString SCENE_LINEN_ROOM_EMPTY = ":/source/Test3/布草间-空.jpg";

            // 道具/UI图片
            const QString UI_CART_DIRTY = ":/source/Test3/推车-脏布草.png";
            const QString UI_CART_CLEAN = ":/source/Test3/推车-布草.png";
            const QString UI_CART_EMPTY = ":/source/Test3/推车-空.png";
            const QString UI_DIRTY_LINEN = ":/source/Test3/脏布草.jpg";
            const QString UI_TASK_SHEET = ":/source/Test3/申领表.png";

            // 物品图标映射 (物品名 -> 图片路径)
            const QMap<QString, QString> ITEMS = {
                {"大床单", ":/source/Test3/大床单.png"},
                {"大被套", ":/source/Test3/大被套.png"},
                {"小被套", ":/source/Test3/小被套.png"},
                {"枕巾", ":/source/Test3/枕巾.png"},
                {"晚安巾", ":/source/Test3/晚安巾.png"},
                {"毛巾", ":/source/Test3/毛巾.png"},
                {"脏布草", ":/source/Test3/脏布草.jpg"} // 特殊项
            };
        }

        // --- 坐标与尺寸 ---
        namespace Geometry {
            // RPG中心显示区域大小
            const QSize CENTER_PANEL_SIZE(896, 720);
            // 侧边栏宽度
            const int SIDEBAR_WIDTH = 192;
            // 任务列表高度
            const int TASK_LIST_HEIGHT = 100;

            // 推车状态图标大小
            const QSize CART_ICON_SIZE(150, 150);
            // 物品列表图标大小
            const QSize INVENTORY_ICON_SIZE(64, 64);
            // 货架上物品图标大小
            const QSize SHELF_ITEM_ICON_SIZE(80, 80);
            // 脏布草拖拽图标大小
            const QSize DIRTY_LINEN_DRAG_SIZE(100, 100);

            // 场景元素坐标 (x, y, w, h)

            // 入口场景
            const QRect BTN_ENTRANCE_ENTER(350, 600, 200, 50); // 进入酒店/下班回家按钮

            // 员工通道场景
            const QRect LBL_HALLWAY_CLOCKED_OFF(100, 100, 200, 50); // "已打卡下班"标签
            const QRect BTN_HALLWAY_EXIT(350, 600, 200, 50); // "返回入口"按钮
            const QRect BTN_HALLWAY_CLOCK_IN(100, 100, 150, 50); // "打卡签到"按钮
            const QRect BTN_HALLWAY_CLOCK_OUT(100, 100, 150, 50); // "打卡下班"按钮
            const QRect BTN_HALLWAY_OFFICE(50, 300, 150, 50); // 去办公室
            const QRect BTN_HALLWAY_WAREHOUSE(250, 300, 150, 50); // 去仓库
            const QRect BTN_HALLWAY_ELEVATOR(450, 300, 150, 50); // 去电梯

            // 办公室场景
            const QRect BTN_OFFICE_REPORT(100, 100, 150, 50); // 汇报工作
            const QRect LBL_OFFICE_DONE(100, 100, 300, 50); // "工作已汇报"提示
            const QRect BTN_OFFICE_GET_TASK(100, 100, 150, 50); // 领取任务
            const QRect BTN_OFFICE_BACK(100, 600, 150, 50); // 返回通道

            // 仓库(入口)场景
            const QRect BTN_WAREHOUSE_TAKE(100, 300, 150, 60); // 拿取布草
            const QRect BTN_WAREHOUSE_BACK(50, 600, 150, 50); // 返回通道

            // 仓库(货架)场景
            const QRect BTN_SHELF_BACK(50, 600, 150, 50); // 返回入口(实际上是返回仓库入口视图)

            // 电梯厅场景
            const QRect BTN_ELEVATOR_ENTER(350, 300, 200, 100); // 进入电梯
            const QRect BTN_ELEVATOR_BACK(100, 600, 150, 50); // 返回

            // 电梯内场景
            const int ELEVATOR_BTN_X = 400; // 电梯按钮X坐标
            const int ELEVATOR_BTN_START_Y = 100; // 电梯按钮起始Y坐标
            const int ELEVATOR_BTN_SPACING = 100; // 按钮垂直间距
            const QSize ELEVATOR_BTN_SIZE(80, 80); // 按钮大小

            // 楼层走廊场景
            const QRect BTN_CORRIDOR_LINEN(200, 200, 200, 100); // 布草间
            const QRect BTN_CORRIDOR_ELEVATOR(500, 200, 200, 100); // 电梯厅

            // 布草间场景
            const QRect LBL_LINEN_DIRTY(700, 400, 100, 100); // 脏布草(拖拽源)
            const QRect BTN_LINEN_BACK(50, 600, 150, 50); // 返回走廊

            // 货架物品区域 (适用于仓库取货和布草间放货)
            // 格式: QRect(x, y, w, h)
            const QRect SHELF_AREA_SHEET(50, 150, 150, 150);  // 大床单
            const QRect SHELF_AREA_DUVET(250, 150, 150, 150); // 大被套
            const QRect SHELF_AREA_S_DUVET(450, 150, 150, 150); // 小被套
            const QRect SHELF_AREA_PILLOW(50, 350, 150, 150); // 枕巾
            const QRect SHELF_AREA_GN_TOWEL(250, 350, 150, 150); // 晚安巾
            const QRect SHELF_AREA_TOWEL(450, 350, 150, 150); // 毛巾

            // 物资申领表
            const QSize SHEET_DIALOG_SIZE(600, 800);

            // 物资申领表文字坐标 (相对申领表图片左上角的偏移)
            // 格式: QPoint(x, y)
            const QPoint SHEET_TEXT_FLOOR(200, 100); // 楼层号
            const QPoint SHEET_TEXT_SHEET(100, 200); // 大床单数量
            const QPoint SHEET_TEXT_DUVET(300, 200); // 大被套数量
            const QPoint SHEET_TEXT_S_DUVET(100, 300); // 小被套数量
            const QPoint SHEET_TEXT_PILLOW(300, 300); // 枕巾数量
            const QPoint SHEET_TEXT_GN_TOWEL(100, 400); // 晚安巾数量
            const QPoint SHEET_TEXT_TOWEL(300, 400); // 毛巾数量
        }
    }
}

#endif // CONFIG_H
