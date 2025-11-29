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
            constexpr const char* SCENE_ENTRANCE = ":/source/Test3/入口.jpg";
            constexpr const char* SCENE_HALLWAY = ":/source/Test3/员工通道走廊.jpg";
            constexpr const char* SCENE_OFFICE = ":/source/Test3/办公室.png";
            constexpr const char* SCENE_WAREHOUSE_ENTRY = ":/source/Test3/仓库1.jpg";
            constexpr const char* SCENE_WAREHOUSE_SHELF = ":/source/Test3/取布草的货架.jpg";
            constexpr const char* SCENE_ELEVATOR_HALL = ":/source/Test3/电梯厅.jpg";
            constexpr const char* SCENE_ELEVATOR_INSIDE = ":/source/Test3/电梯内.jpg";
            constexpr const char* SCENE_FLOOR_CORRIDOR = ":/source/Test3/楼层走廊-前.png";
            constexpr const char* SCENE_LINEN_ROOM_EMPTY = ":/source/Test3/布草间-空.jpg";

            // UI 图标
            constexpr const char* UI_CART_DIRTY = ":/source/Test3/推车-脏布草.png";
            constexpr const char* UI_CART_CLEAN = ":/source/Test3/推车-布草.png";
            constexpr const char* UI_CART_EMPTY = ":/source/Test3/推车-空.png";
            constexpr const char* UI_DIRTY_LINEN = ":/source/Test3/脏布草.jpg";
            constexpr const char* UI_TASK_SHEET = ":/source/Test3/申领表.png";

            // 物品对应的图标路径映射 (Key: 物品名, Value: 路径)
            // Use static function to avoid global static construction
            static const QMap<QString, QString>& ITEMS() {
                static const QMap<QString, QString> map = {
                    {"大床单", ":/source/Test3/大床单.png"},
                    {"大被套", ":/source/Test3/大被套.png"},
                    {"小被套", ":/source/Test3/小被套.png"},
                    {"枕巾", ":/source/Test3/枕巾.png"},
                    {"晚安巾", ":/source/Test3/晚安巾.png"},
                    {"毛巾", ":/source/Test3/毛巾.png"},
                    {"脏布草", ":/source/Test3/脏布草.jpg"}
                };
                return map;
            }
        }

        // --- 界面文本 (Texts) ---
        namespace Texts {
            // 侧边栏文本
            constexpr const char* LBL_LOCATION_PREFIX = "当前位置:\n";
            constexpr const char* LBL_TASK_TITLE = "当前任务:";
            constexpr const char* BTN_VIEW_TASK_SHEET = "查看申领表";
            constexpr const char* LBL_INVENTORY_TITLE = "推车存货 (拖拽使用):";
            constexpr const char* LBL_ELEVATOR_PANEL_TITLE = "电梯楼层选择:"; // New title for elevator panel
            constexpr const char* LBL_CART_DIRTY = "脏布草";
            constexpr const char* LBL_CART_HAS_ITEMS = "有布草";
            constexpr const char* LBL_CART_EMPTY = "空车";

            // 返回主菜单按钮
            constexpr const char* BTN_TEXT_BACK_TO_MENU = "返回主界面";

            // 场景交互按钮文本
            constexpr const char* BTN_ENTER_HOTEL = "进入酒店";
            constexpr const char* BTN_GO_HOME = "下班回家";
            constexpr const char* LBL_CLOCKED_OFF = "已打卡下班";
            constexpr const char* BTN_RETURN_ENTRANCE = "返回入口";
            constexpr const char* BTN_CLOCK_IN = "打卡签到";
            constexpr const char* BTN_CLOCK_OUT = "打卡下班";
            constexpr const char* BTN_GO_OFFICE = "去办公室";
            constexpr const char* BTN_GO_WAREHOUSE = "去仓库";
            constexpr const char* BTN_GO_ELEVATOR = "去电梯";
            constexpr const char* BTN_REPORT_WORK = "汇报工作";
            constexpr const char* LBL_WORK_REPORTED = "工作已汇报，请去走廊打卡下班。";
            constexpr const char* BTN_GET_TASK = "领取任务";
            constexpr const char* BTN_TASK_IN_PROGRESS = "任务进行中...";
            constexpr const char* BTN_RETURN_HALLWAY = "返回通道";
            constexpr const char* BTN_TAKE_LINEN = "拿取布草";
            constexpr const char* BTN_RETURN_WAREHOUSE_ENTRY = "返回入口"; // 货架视图返回
            constexpr const char* BTN_ENTER_ELEVATOR = "乘坐电梯";
            constexpr const char* BTN_RETURN_BACK = "进入走廊";
            constexpr const char* BTN_EXIT_ELEVATOR = "出电梯"; // New button text
            constexpr const char* BTN_GO_LINEN_ROOM = "布草间";
            constexpr const char* BTN_GO_ELEVATOR_HALL = "电梯厅";
            constexpr const char* BTN_RETURN_CORRIDOR = "返回走廊";
            constexpr const char* LBL_DIRTY_LINEN_DRAG = "脏布草";
        }

        // --- 样式与颜色 (Styles) ---
        namespace Styles {
            // 侧边栏样式
            constexpr const char* SIDEBAR_LEFT = "background-color: #f2f2f7; color: #333333; border-right: 1px solid #d1d1d6;";
            constexpr const char* SIDEBAR_RIGHT = "background-color: #f2f2f7; color: #333333; border-left: 1px solid #d1d1d6;";
            constexpr const char* LBL_TITLE = "font-weight: bold; color: #333333; margin-top: 10px; font-size: 16px;";

            // 任务列表样式
            constexpr const char* LIST_WIDGET = "color: #333333; background: white; font-size: 13px; border: 1px solid #d1d1d6; border-radius: 6px;";

            // 按钮特殊颜色
            constexpr const char* BTN_ORANGE = "background-color: #FF9500; color: white;"; // 查看申领表
            //constexpr const char* BTN_GREEN = "background-color: #34C759; color: white; font-size: 16px; font-weight: 600;"; // 下班回家
            //constexpr const char* BTN_RED = "background-color: #FF3B30; color: white; font-size: 16px; font-weight: bold;"; // 打卡下班
            //constexpr const char* BTN_YELLOW = "background-color: #FFCC00; color: black; font-weight: 600;"; // 汇报工作
            //constexpr const char* BTN_BLUE = "font-size: 16px; background-color: #007AFF; color: white; font-weight: 600;"; // 拿取布草
            constexpr const char* BTN_RETURN_MENU = "background-color: #FF3B30; color: white; font-weight: bold; border-radius: 6px;"; // 返回主菜单

            // 货架区域样式 (半透明灰色背景)
            constexpr const char* SHELF_AREA = "background-color: rgba(255, 255, 255, 0.6); border: 2px dashed #8e8e93; border-radius: 8px;";

            // 提示标签颜色
            constexpr const char* LBL_SUCCESS_GREEN = "font-size: 18px; color: #34C759; font-weight: bold;";

            // --- 独立按钮样式配置 (Specific Button Styles) ---
            // 格式: 背景色, 前景色, 边框, 圆角, 字体粗细
            constexpr const char* STYLE_BTN_ENTRANCE_ENTER = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_ENTRANCE_HOME = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_HALLWAY_EXIT = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_HALLWAY_CLOCK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;"; // 签到
            constexpr const char* STYLE_BTN_HALLWAY_CLOCK_OFF = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;"; // 下班
            constexpr const char* STYLE_BTN_HALLWAY_OFFICE = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_HALLWAY_WAREHOUSE = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_HALLWAY_ELEVATOR = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_OFFICE_ACTION = "background-color: #FFCC00; color: black; border-radius: 8px; font-weight: bold;"; // 汇报/领任务
            constexpr const char* STYLE_BTN_OFFICE_BACK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_WAREHOUSE_TAKE = "background-color: #007AFF; color: white; border-radius: 8px; font-weight: bold; font-size: 16px;";
            constexpr const char* STYLE_BTN_WAREHOUSE_BACK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_SHELF_BACK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_ELEVATOR_ENTER = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_ELEVATOR_BACK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_ELEVATOR_EXIT = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            // 电梯内部楼层按钮样式 (Sidebar) - 金属质感高对比度 (Dark Grey background, White Text)
            constexpr const char* STYLE_BTN_ELEVATOR_FLOOR = "background-color: #444444; color: #FFFFFF; border: 2px solid #999999; border-radius: 4px; font-weight: bold; font-size: 18px;";

            constexpr const char* STYLE_BTN_CORRIDOR_LINEN = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_CORRIDOR_ELEVATOR = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_LINEN_BACK = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
        }

        // --- 字体配置 (Fonts) ---
        namespace Fonts {
            const int SIZE_LINEN_COUNT = 18; // Smaller font size
            constexpr const char* COL_LINEN_COUNT = "#FF3B30"; // Red color for visibility
        }

        // --- 坐标与尺寸 (Geometry) ---
        // 注意: 所有 (x, y) 坐标均为控件中心点，而非左上角。
        // 包括按钮位置、文本绘制位置等。
        // Note: All (x, y) coordinates represent the CENTER point of the widget/text.
        namespace Geometry {
            // 全局尺寸
            const QSize CENTER_PANEL_SIZE(896, 720);
            const int SIDEBAR_WIDTH = 220;
            const int TASK_LIST_HEIGHT = 150;
            const QSize RETURN_BTN_SIZE(140, 40);

            // 图标尺寸
            const QSize ICON_CART(140, 140);
            const QSize ICON_INVENTORY(64, 64);
            const QSize ICON_SHELF_ITEM(90, 90);
            const QSize ICON_DIRTY_DRAG(100, 100);

            // --- 场景元素布局 (x_center, y_center, w, h) ---

            // 1. 入口场景 (Entrance)
            const QRect RECT_BTN_ENTRANCE_ENTER(683, 473, 150, 40); // 进入酒店
            const QRect RECT_BTN_ENTRANCE_HOME(129, 632, 150, 40);  // 下班回家 (位置相同)

            // 2. 员工通道 (Staff Hallway)
            const QRect RECT_LBL_HALLWAY_STATUS(425, 256, 200, 50);   // 状态标签
            const QRect RECT_BTN_HALLWAY_EXIT(350, 600, 150, 40);     // 返回入口
            const QRect RECT_BTN_HALLWAY_CLOCK(171, 442, 150, 40);    // 签到/下班
            const QRect RECT_BTN_HALLWAY_OFFICE(271, 307, 150, 40);   // 去办公室
            const QRect RECT_BTN_HALLWAY_WAREHOUSE(659, 348, 150, 40);// 去仓库
            const QRect RECT_BTN_HALLWAY_ELEVATOR(516, 389, 150, 40); // 去电梯

            // 3. 办公室 (Office)
            const QRect RECT_BTN_OFFICE_ACTION(490, 352, 150, 40);    // 汇报/领任务
            const QRect RECT_LBL_OFFICE_MSG(425, 256, 300, 50);       // 消息提示
            const QRect RECT_BTN_OFFICE_BACK(214, 654, 150, 40);      // 返回通道

            // 4. 仓库入口 (Warehouse Entry)
            const QRect RECT_BTN_WAREHOUSE_TAKE(541, 357, 150, 40);   // 拿取布草
            const QRect RECT_BTN_WAREHOUSE_BACK(259, 659, 150, 40);   // 返回通道

            // 5. 仓库货架 (Warehouse Shelf)
            const QRect RECT_BTN_SHELF_BACK(100, 600, 150, 40);       // 返回仓库入口
            // 货架物品区域 (中心坐标)
            const QRect AREA_SHEET(348, 561, 100, 100);      //大床单
            const QRect AREA_DUVET(754, 136, 100, 100);      //大被套
            const QRect AREA_S_DUVET(262, 141, 100, 100);    //小被套
            const QRect AREA_PILLOW(216, 428, 100, 100);     //枕巾
            const QRect AREA_GN_TOWEL(517, 578, 100, 100);   //晚安巾
            const QRect AREA_TOWEL(757, 572, 100, 100);      //毛巾

            // 6. 电梯厅 (Elevator Hall)
            const QRect RECT_BTN_ELEVATOR_ENTER(656, 368, 150, 40);  // 进入电梯
            const QRect RECT_BTN_ELEVATOR_BACK(186, 655, 150, 40);    // 返回

            // 7. 电梯内 (Elevator Inside)
            const QRect RECT_BTN_ELEVATOR_EXIT(448, 650, 150, 40); // 出电梯按钮

            // 侧边栏按钮布局
            // 侧边栏宽度约 220，内部按钮网格布局
            const QSize SIZE_ELEVATOR_BTN_SIDEBAR(50, 50); // 侧边栏按钮大小
            const int GRID_SPACING_ELEVATOR = 10;
            // 注意: 这些按钮将添加到 Right Panel 的 Layout 中，而非绝对定位

            // 8. 楼层走廊 (Floor Corridor)
            const QRect RECT_BTN_CORRIDOR_LINEN(463, 339, 150, 40);    // 去布草间
            const QRect RECT_BTN_CORRIDOR_ELEVATOR(439, 647, 150, 40); // 去电梯厅

            // 9. 布草间 (Linen Room)
            //const QRect RECT_LBL_DIRTY_SOURCE(700, 400, 150, 150);      // 脏布草堆位置
            const QRect RECT_BTN_LINEN_BACK(240, 667, 150, 40);         // 返回走廊

            // 紧急任务 脏布草 (Event B) 位置配置
            const QRect RECT_EVENT_DIRTY_LINEN(621, 628, 100, 100); // 突发事件B 脏布草位置

            // 货架物品区域 - 楼层布草间专用
            const QRect AREA_LINEN_SHEET(604, 350, 100, 100);    //大床单
            const QRect AREA_LINEN_DUVET(346, 530, 100, 100);    //大被套
            const QRect AREA_LINEN_S_DUVET(128, 179, 100, 100);  //小被套
            const QRect AREA_LINEN_PILLOW(351, 200, 100, 100);   //枕巾
            const QRect AREA_LINEN_GN_TOWEL(337, 343, 100, 100); //晚安巾
            const QRect AREA_LINEN_TOWEL(608, 180, 100, 100);    //毛巾

            // 申领表弹窗
            const QSize SHEET_DIALOG(600, 560);
            const QPoint TXT_FLOOR(190, 50);

            const QPoint TXT_SHEET(295, 185);
            const QPoint TXT_DUVET(295, 250);
            const QPoint TXT_S_DUVET(295, 315);
            const QPoint TXT_PILLOW(295, 380);
            const QPoint TXT_GN_TOWEL(295, 445);
            const QPoint TXT_TOWEL(295, 510);
        }
    }
}

#endif // CONFIG_TEST3_H
