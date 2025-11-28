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
 */
namespace Config {
    namespace Test3 {

        // --- 资源路径 (Images) ---
        namespace Images {
            // 场景背景
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

            // 物品对应的图标路径映射
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
            // 侧边栏
            const QString LBL_LOCATION_PREFIX = "当前位置:\n";
            const QString LBL_TASK_TITLE = "当前任务:";
            const QString BTN_VIEW_TASK_SHEET = "查看申领表";
            const QString LBL_INVENTORY_TITLE = "推车存货 (拖拽使用):";
            const QString LBL_CART_DIRTY = "脏布草";
            const QString LBL_CART_HAS_ITEMS = "有布草";
            const QString LBL_CART_EMPTY = "空车";

            // 场景按钮文本
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
            // 侧边栏样式
            const QString SIDEBAR_LEFT = "background-color: #2c3e50; color: white; border-right: 1px solid #1a252f;";
            const QString SIDEBAR_RIGHT = "background-color: #34495e; color: white; border-left: 1px solid #1a252f;";
            const QString LBL_TITLE = "font-weight: bold; color: #ecf0f1; margin-top: 10px;";

            // 任务列表样式
            const QString LIST_WIDGET = "color: black; background: white; font-size: 12px;";

            // 按钮特殊颜色
            const QString BTN_ORANGE = "background-color: #e67e22;"; // 查看申领表
            const QString BTN_GREEN = "background-color: #27ae60; color: white; font-size: 18px;"; // 下班回家
            const QString BTN_RED = "background-color: #e74c3c; color: white; font-size: 18px; font-weight: bold;"; // 打卡下班
            const QString BTN_YELLOW = "background-color: #f1c40f; color: black;"; // 汇报工作
            const QString BTN_BLUE = "font-size: 18px; background-color: #3498db; color: white;"; // 拿取布草

            // 货架区域样式 (半透明灰色背景)
            const QString SHELF_AREA = "background-color: rgba(200, 200, 200, 0.5); border: 2px solid #bdc3c7; border-radius: 4px;";

            // 提示标签颜色
            const QString LBL_SUCCESS_GREEN = "font-size: 18px; color: green; font-weight: bold;";
        }

        // --- 坐标与尺寸 (Geometry) ---
        namespace Geometry {
            // 全局尺寸
            const QSize CENTER_PANEL_SIZE(896, 720);
            const int SIDEBAR_WIDTH = 192;
            const int TASK_LIST_HEIGHT = 100;

            // 图标尺寸
            const QSize ICON_CART(150, 150);
            const QSize ICON_INVENTORY(64, 64);
            const QSize ICON_SHELF_ITEM(80, 80);
            const QSize ICON_DIRTY_DRAG(100, 100);

            // --- 场景元素布局 (x, y, w, h) ---

            // 入口
            const QRect BTN_ENTRANCE_ACTION(350, 600, 200, 50);

            // 员工通道
            const QRect LBL_HALLWAY_STATUS(100, 100, 200, 50);
            const QRect BTN_HALLWAY_EXIT(350, 600, 200, 50);
            const QRect BTN_HALLWAY_CLOCK_ACTION(100, 100, 150, 50); // 签到/下班 共用位置
            const QRect BTN_HALLWAY_GO_OFFICE(50, 300, 150, 50);
            const QRect BTN_HALLWAY_GO_WAREHOUSE(250, 300, 150, 50);
            const QRect BTN_HALLWAY_GO_ELEVATOR(450, 300, 150, 50);

            // 办公室
            const QRect BTN_OFFICE_MAIN_ACTION(100, 100, 150, 50); // 汇报/领任务
            const QRect LBL_OFFICE_MSG(100, 100, 300, 50);
            const QRect BTN_OFFICE_BACK(100, 600, 150, 50);

            // 仓库(入口)
            const QRect BTN_WAREHOUSE_TAKE(100, 300, 150, 60);
            const QRect BTN_WAREHOUSE_BACK(50, 600, 150, 50);

            // 仓库(货架)
            const QRect BTN_SHELF_BACK(50, 600, 150, 50);
            // 货架物品区域 (x, y, w, h)
            const QRect AREA_SHEET(50, 150, 150, 150);//大床单
            const QRect AREA_DUVET(250, 150, 150, 150);//大被套
            const QRect AREA_S_DUVET(450, 150, 150, 150);//小被套
            const QRect AREA_PILLOW(50, 350, 150, 150);//枕巾
            const QRect AREA_GN_TOWEL(250, 350, 150, 150);//晚安巾
            const QRect AREA_TOWEL(450, 350, 150, 150);//毛巾

            // 电梯厅
            const QRect BTN_ELEVATOR_ENTER(350, 300, 200, 100);
            const QRect BTN_ELEVATOR_BACK(100, 600, 150, 50);

            // 电梯内
            const int ELEVATOR_BTN_X = 400;
            const int ELEVATOR_BTN_START_Y = 100;
            const int ELEVATOR_BTN_SPACING = 100;
            const QSize ELEVATOR_BTN_SIZE(80, 80);

            // 走廊
            const QRect BTN_CORRIDOR_LINEN(200, 200, 200, 100);
            const QRect BTN_CORRIDOR_ELEVATOR(500, 200, 200, 100);

            // 布草间
            const QRect LBL_DIRTY_SOURCE(700, 400, 100, 100);
            const QRect BTN_LINEN_BACK(50, 600, 150, 50);

            // 申领表弹窗
            const QSize SHEET_DIALOG(600, 580);
            // 申领表文字坐标 (x, y)
            const QPoint TXT_FLOOR(180, 65); //楼层
            const QPoint TXT_SHEET(288, 305); //大床单
            const QPoint TXT_DUVET(288, 375); //大被套
            const QPoint TXT_S_DUVET(288, 435); //小被套
            const QPoint TXT_PILLOW(288, 495); //枕巾
            const QPoint TXT_GN_TOWEL(288, 555); //晚安巾
            const QPoint TXT_TOWEL(288, 625); //毛巾
        }
    }
}

#endif // CONFIG_TEST3_H
