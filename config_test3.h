#ifndef CONFIG_TEST3_H
#define CONFIG_TEST3_H

#include <QString>
#include <QMap>
#include <QSize>
#include <QRect>
#include <QPoint>

/**
 * @brief 测试3 (RPG 实训) 配置
 * 包含所有场景图片路径、按钮坐标、尺寸、文本、颜色等详细配置。
 * 作用范围: Test3 模块
 *
 * 注意: 所有坐标 (x, y) 均代表控件的**中心点**位置，而非左上角。
 * Note: All coordinates (x, y) represent the **center** point of the widget, not the top-left.
 */
namespace Config {
    namespace Test3 {

        // --- 资源路径 (Images) ---
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

            // UI 图标
            const QString UI_CART_DIRTY = ":/source/Test3/推车-脏布草.png";
            const QString UI_CART_CLEAN = ":/source/Test3/推车-布草.png";
            const QString UI_CART_EMPTY = ":/source/Test3/推车-空.png";
            const QString UI_DIRTY_LINEN = ":/source/Test3/脏布草.jpg";
            const QString UI_TASK_SHEET = ":/source/Test3/申领表.png";

            // 物品对应的图标路径映射 (Key: 物品名, Value: 路径)
            const QMap<QString, QString> ITEMS = {
                {"大床单", ":/source/Test3/大床单.png"},
                {"大被套", ":/source/Test3/大被套.png"},
                {"小被套", ":/source/Test3/小被套.png"},
                {"枕巾", ":/source/Test3/枕巾.png"},
                {"晚安巾", ":/source/Test3/晚安巾.png"},
                {"毛巾", ":/source/Test3/毛巾.png"},
                {"脏布草", ":/source/Test3/脏布草.jpg"}
            };
        }

        // --- 界面文本 (Texts) ---
        namespace Texts {
            // 侧边栏文本
            const QString LBL_LOCATION_PREFIX = "当前位置:\n";
            const QString LBL_TASK_TITLE = "当前任务:";
            const QString BTN_VIEW_TASK_SHEET = "查看申领表";
            const QString LBL_INVENTORY_TITLE = "推车存货 (拖拽使用):";
            const QString LBL_CART_DIRTY = "脏布草";
            const QString LBL_CART_HAS_ITEMS = "有布草";
            const QString LBL_CART_EMPTY = "空车";

            // 返回主菜单按钮
            const QString BTN_TEXT_BACK_TO_MENU = "返回主界面";

            // 场景交互按钮文本
            const QString BTN_ENTER_HOTEL = "进入酒店";
            const QString BTN_GO_HOME = "下班回家";
            const QString LBL_CLOCKED_OFF = "已打卡下班";
            const QString BTN_RETURN_ENTRANCE = "返回入口";
            const QString BTN_CLOCK_IN = "打卡签到";
            const QString BTN_CLOCK_OUT = "打卡下班";
            const QString BTN_GO_OFFICE = "去办公室";
            const QString BTN_GO_WAREHOUSE = "去仓库";
            const QString BTN_GO_ELEVATOR = "去电梯";
            const QString BTN_REPORT_WORK = "汇报工作";
            const QString LBL_WORK_REPORTED = "工作已汇报，请去走廊下班。";
            const QString BTN_GET_TASK = "领取任务";
            const QString BTN_TASK_IN_PROGRESS = "任务进行中...";
            const QString BTN_RETURN_HALLWAY = "返回通道";
            const QString BTN_TAKE_LINEN = "拿取布草";
            const QString BTN_RETURN_WAREHOUSE_ENTRY = "返回入口"; // 货架视图返回
            const QString BTN_ENTER_ELEVATOR = "进入电梯";
            const QString BTN_RETURN_BACK = "返回";
            const QString BTN_GO_LINEN_ROOM = "布草间";
            const QString BTN_GO_ELEVATOR_HALL = "电梯厅";
            const QString BTN_RETURN_CORRIDOR = "返回走廊";
            const QString LBL_DIRTY_LINEN_DRAG = "脏布草(拖拽)";
        }

        // --- 样式与颜色 (Styles) ---
        namespace Styles {
            // 侧边栏样式 - Mac Dark Mode inspired sidebar
            const QString SIDEBAR_LEFT = "background-color: #f2f2f7; color: #333333; border-right: 1px solid #d1d1d6;";
            const QString SIDEBAR_RIGHT = "background-color: #f2f2f7; color: #333333; border-left: 1px solid #d1d1d6;";
            const QString LBL_TITLE = "font-weight: bold; color: #333333; margin-top: 10px; font-size: 16px;";

            // 任务列表样式
            const QString LIST_WIDGET = "color: #333333; background: white; font-size: 13px; border: 1px solid #d1d1d6; border-radius: 6px;";

            // 按钮特殊颜色 - Updated for Flat Design
            const QString BTN_ORANGE = "background-color: #FF9500; color: white;"; // 查看申领表
            const QString BTN_GREEN = "background-color: #34C759; color: white; font-size: 16px; font-weight: 600;"; // 下班回家
            const QString BTN_RED = "background-color: #FF3B30; color: white; font-size: 16px; font-weight: bold;"; // 打卡下班
            const QString BTN_YELLOW = "background-color: #FFCC00; color: black; font-weight: 600;"; // 汇报工作
            const QString BTN_BLUE = "font-size: 16px; background-color: #007AFF; color: white; font-weight: 600;"; // 拿取布草
            const QString BTN_RETURN_MENU = "background-color: #FF3B30; color: white; font-weight: bold; border-radius: 6px;"; // 返回主菜单 (Deep Red)

            // 场景切换按钮通用样式 - Transparent/Overlay buttons can remain standard or be styled
            const QString BTN_SCENE_DEFAULT = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            // 货架区域样式 (半透明灰色背景)
            const QString SHELF_AREA = "background-color: rgba(255, 255, 255, 0.6); border: 2px dashed #8e8e93; border-radius: 8px;";

            // 提示标签颜色
            const QString LBL_SUCCESS_GREEN = "font-size: 18px; color: #34C759; font-weight: bold;";
        }

        // --- 字体配置 (Fonts) ---
        namespace Fonts {
            const int SIZE_LINEN_COUNT = 32;
            const QString COL_LINEN_COUNT = "#FF3B30"; // Red color for visibility
        }

        // --- 坐标与尺寸 (Geometry) ---
        // 注意: 所有 (x, y) 坐标均为控件中心点
        namespace Geometry {
            // 全局尺寸
            const QSize CENTER_PANEL_SIZE(896, 720);
            const int SIDEBAR_WIDTH = 220; // Slightly wider for Mac look
            const int TASK_LIST_HEIGHT = 150; // Taller
            const QSize RETURN_BTN_SIZE(140, 40);

            // 图标尺寸
            const QSize ICON_CART(140, 140);
            const QSize ICON_INVENTORY(64, 64);
            const QSize ICON_SHELF_ITEM(90, 90);
            const QSize ICON_DIRTY_DRAG(100, 100);

            // --- 场景元素布局 (x_center, y_center, w, h) ---

            // 入口场景
            const QRect BTN_ENTRANCE_ACTION(678, 460, 200, 50);

            // 员工通道
            const QRect LBL_HALLWAY_STATUS(100, 100, 120, 40);
            const QRect BTN_HALLWAY_EXIT(350, 600, 120, 40);
            const QRect BTN_HALLWAY_CLOCK_ACTION(176, 430, 120, 40); // 签到/下班 共用位置
            const QRect BTN_HALLWAY_GO_OFFICE(270, 409, 120, 40);
            const QRect BTN_HALLWAY_GO_WAREHOUSE(655, 354, 120, 40);
            const QRect BTN_HALLWAY_GO_ELEVATOR(515, 372, 120, 40);

            // 办公室
            const QRect BTN_OFFICE_MAIN_ACTION(475, 346, 160, 50); // 汇报/领任务
            const QRect LBL_OFFICE_MSG(296, 116, 300, 50);
            const QRect BTN_OFFICE_BACK(161, 656, 160, 50);

            // 仓库(入口)
            const QRect BTN_WAREHOUSE_TAKE(454, 372, 160, 50);
            const QRect BTN_WAREHOUSE_BACK(213, 658, 160, 50);

            // 仓库(货架)
            const QRect BTN_SHELF_BACK(149, 651, 160, 50);

            // 货架物品区域 - 仓库专用 (中心坐标)
            const QRect AREA_SHEET(242, 270, 100, 100);//大床单
            const QRect AREA_DUVET(511, 270, 100, 100);//大被套
            const QRect AREA_S_DUVET(703, 270, 100, 100);//小被套
            const QRect AREA_PILLOW(157, 433, 100, 100);//枕巾
            const QRect AREA_GN_TOWEL(516, 580, 100, 100);//晚安巾
            const QRect AREA_TOWEL(759, 573, 100, 100);//毛巾

            // 电梯厅
            const QRect BTN_ELEVATOR_ENTER(655, 377, 160, 50);
            const QRect BTN_ELEVATOR_BACK(125, 651, 160, 50);

            // 电梯内 (按钮面板)
            const QSize ELEVATOR_BTN_SIZE(40, 40);
            // 单独设置每个电梯按钮的位置 (中心点)
            // G(0), 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
            const QPoint BTN_FLOOR_G(600, 500); // 底层
            const QPoint BTN_FLOOR_1(600, 420);
            const QPoint BTN_FLOOR_2(670, 420);
            const QPoint BTN_FLOOR_3(600, 360);
            const QPoint BTN_FLOOR_4(670, 360);
            const QPoint BTN_FLOOR_5(600, 280);
            const QPoint BTN_FLOOR_6(670, 280);
            const QPoint BTN_FLOOR_7(600, 200);
            const QPoint BTN_FLOOR_8(670, 200);
            const QPoint BTN_FLOOR_9(600, 120);
            const QPoint BTN_FLOOR_10(670, 120);

            // 走廊
            const QRect BTN_CORRIDOR_LINEN(459, 343, 100, 50);
            const QRect BTN_CORRIDOR_ELEVATOR(436, 644, 100, 50);

            // 布草间
            const QRect LBL_DIRTY_SOURCE(700, 400, 100, 100);
            const QRect BTN_LINEN_BACK(100, 600, 160, 50);

            // 货架物品区域 - 楼层布草间专用 (中心坐标，位置与仓库不同)
            const QRect AREA_LINEN_SHEET(612, 355, 100, 100);//大床单
            const QRect AREA_LINEN_DUVET(353, 532, 100, 100);//大被套
            const QRect AREA_LINEN_S_DUVET(139, 175, 100, 100);//小被套
            const QRect AREA_LINEN_PILLOW(350, 200, 100, 100);//枕巾
            const QRect AREA_LINEN_GN_TOWEL(347, 354, 100, 100);//晚安巾
            const QRect AREA_LINEN_TOWEL(626, 186, 100, 100);//毛巾

            // 申领表弹窗
            const QSize SHEET_DIALOG(600, 560);
            // 申领表文字坐标 (x, y)
            const QPoint TXT_FLOOR(187, 72); //楼层
            const QPoint TXT_SHEET(293, 206); //大床单
            const QPoint TXT_DUVET(293, 265); //大被套
            const QPoint TXT_S_DUVET(293, 327); //小被套
            const QPoint TXT_PILLOW(293, 495); //枕巾
            const QPoint TXT_GN_TOWEL(293, 555); //晚安巾
            const QPoint TXT_TOWEL(293, 625); //毛巾
        }
    }
}

#endif // CONFIG_TEST3_H
