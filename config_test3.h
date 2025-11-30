#ifndef CONFIG_TEST3_H
#define CONFIG_TEST3_H

#include <QString>
#include <QMap>
#include <QSize>
#include <QRect>
#include <QPoint>
#include <QPolygon>
#include <QVector>

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
            // 走廊状态图
            constexpr const char* SCENE_HALLWAY_NORMAL = ":/source/Test3/走廊-状态2（正常打卡）.jpg";
            constexpr const char* SCENE_HALLWAY_LATE = ":/source/Test3/走廊-状态2（迟到打卡）.jpg";
            constexpr const char* SCENE_HALLWAY_CLOCKED_OUT = ":/source/Test3/走廊-状态3（下班打卡）.jpg";

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
            constexpr const char* UI_DIRTY_LINEN = ":/source/Test3/脏布草.png";
            constexpr const char* UI_TASK_SHEET = ":/source/Test3/申领表.png";

            // 物品对应的图标路径映射 (Key: 物品名, Value: 路径)
            static const QMap<QString, QString>& ITEMS() {
                static const QMap<QString, QString> map = {
                    {"大床单", ":/source/Test3/大床单.png"},
                    {"大被套", ":/source/Test3/大被套.png"},
                    {"小被套", ":/source/Test3/小被套.png"},
                    {"枕巾", ":/source/Test3/枕巾.png"},
                    {"晚安巾", ":/source/Test3/晚安巾.png"},
                    {"毛巾", ":/source/Test3/毛巾.png"},
                    {"脏布草", ":/source/Test3/脏布草.png"}
                };
                return map;
            }
        }

        // --- 逻辑配置 (Logic) ---
        namespace Logic {
            const int TIME_LATE_THRESHOLD_SEC = 10; // 迟到判定时间 (秒)
            const int TASK_FIXED_FLOOR_1 = 6;       // 基础任务楼层 1
            const int TASK_FIXED_FLOOR_2 = 7;       // 基础任务楼层 2
            const int MAX_TASK_ITEM_TYPES = 6;      // 任务物品种类最大数量
            const int MAX_TASK_ITEM_COUNT = 5;      // 单个物品最大数量
        }

        // --- 界面文本 (Texts) ---
        namespace Texts {
            // 侧边栏文本
            constexpr const char* LBL_LOCATION_PREFIX = "当前位置:\n";
            constexpr const char* LBL_TASK_TITLE = "当前任务:";
            constexpr const char* BTN_VIEW_TASK_SHEET = "查看申领表";
            constexpr const char* LBL_INVENTORY_TITLE = "推车存货 (拖拽使用):";
            constexpr const char* LBL_ELEVATOR_PANEL_TITLE = "电梯楼层选择:";
            constexpr const char* LBL_CART_DIRTY = "脏布草";
            constexpr const char* LBL_CART_HAS_ITEMS = "有布草";
            constexpr const char* LBL_CART_EMPTY = "空车";

            // 返回主菜单按钮
            constexpr const char* BTN_TEXT_BACK_TO_MENU = "返回主界面";

            // 场景交互文本
            // 注意：部分按钮现在使用箭头样式，文本将显示在箭头尾部
            constexpr const char* BTN_ENTER_HOTEL = "进入酒店"; // 不规则区域，无文本显示(或作为Tooltip)
            constexpr const char* BTN_GO_HOME = "下班回家";   // 不规则区域

            constexpr const char* BTN_RETURN_ENTRANCE = "返回入口";
            constexpr const char* BTN_CLOCK_IN = "打卡签到";
            constexpr const char* BTN_CLOCK_OUT = "打卡下班";
            constexpr const char* BTN_GO_OFFICE = "办公室";
            constexpr const char* BTN_GO_WAREHOUSE = "仓库";
            constexpr const char* BTN_GO_ELEVATOR = "电梯厅";
            constexpr const char* BTN_REPORT_WORK = "汇报工作";
            constexpr const char* BTN_GET_TASK = "领取任务";
            constexpr const char* BTN_RETURN_HALLWAY = "回通道";
            constexpr const char* BTN_TAKE_LINEN = "拿取布草";
            constexpr const char* BTN_RETURN_WAREHOUSE_ENTRY = "返回";
            constexpr const char* BTN_ENTER_ELEVATOR = "乘坐电梯";
            constexpr const char* BTN_RETURN_BACK = "回走廊";
            constexpr const char* BTN_EXIT_ELEVATOR = "出电梯";
            constexpr const char* BTN_GO_LINEN_ROOM = "布草间";
            constexpr const char* BTN_GO_ELEVATOR_HALL = "电梯厅";
            constexpr const char* BTN_RETURN_CORRIDOR = "回走廊";

            // 状态/提示文本
            constexpr const char* LBL_CLOCKED_OFF = "已打卡下班";
            constexpr const char* LBL_WORK_REPORTED = "工作已汇报。";
            constexpr const char* BTN_TASK_IN_PROGRESS = "任务进行中...";
            constexpr const char* LBL_DIRTY_LINEN_DRAG = "脏布草";

            // 紧急任务弹窗
            constexpr const char* POPUP_EMERGENCY_MANAGER = "经理：%1楼现在需要马上补充一些布草，你先去%1楼送一下。";

            // 汇报工作与错误反馈
            constexpr const char* REPORT_SUCCESS = "经理：工作做得很好，流程很规范。";
            constexpr const char* REPORT_ERR_LATE = "经理：你今天迟到了。";
            constexpr const char* REPORT_ERR_PRIORITY = "经理：你怎么不先完成紧急任务？";
            constexpr const char* REPORT_ERR_MISSING_TASK = "经理：还有任务没做完。";
            // 即使未汇报或未打卡，这些通常是下班后的总结，但如果点击汇报时有其他问题，可以组合显示
            constexpr const char* ERR_LOG_LATE_CLOCK = "上班迟到";
            constexpr const char* ERR_LOG_MISSED_PRIORITY = "未优先完成紧急任务";
            constexpr const char* ERR_LOG_NO_REPORT = "下班前未汇报工作";
            constexpr const char* ERR_LOG_NO_CLOCK_OUT = "下班前未打卡";
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
            constexpr const char* BTN_ORANGE = "background-color: #FF9500; color: white;";
            constexpr const char* BTN_RETURN_MENU = "background-color: #FF3B30; color: white; font-weight: bold; border-radius: 6px;";

            // 货架区域样式
            constexpr const char* SHELF_AREA = "background-color: #fff; border: 2px dashed #8e8e93; border-radius: 8px;";

            // 提示标签颜色
            constexpr const char* LBL_SUCCESS_GREEN = "font-size: 18px; color: #34C759; font-weight: bold;";

            // 箭头按钮样式配置
            // 颜色：箭头颜色
            constexpr const char* ARROW_COLOR = "#007AFF";
            constexpr const char* ARROW_TEXT_COLOR = "#333333";
            const int ARROW_FONT_SIZE = 14;

            // --- 独立按钮样式配置 ---
            // 格式: 背景色, 前景色, 边框, 圆角, 字体粗细

            // 不规则按钮不需要样式表，因为是透明的

            // 员工通道
            constexpr const char* STYLE_BTN_HALLWAY_EXIT = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            constexpr const char* STYLE_BTN_OFFICE_ACTION = "background-color: #FFCC00; color: black; border-radius: 8px; font-weight: bold;"; // 汇报/领任务

            constexpr const char* STYLE_BTN_WAREHOUSE_TAKE = "background-color: #007AFF; color: white; border-radius: 8px; font-weight: bold; font-size: 16px;";

            constexpr const char* STYLE_BTN_ELEVATOR_ENTER = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";
            constexpr const char* STYLE_BTN_ELEVATOR_EXIT = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 8px; padding: 10px; font-weight: 600;";

            // 电梯内部楼层按钮样式 (Sidebar)
            constexpr const char* STYLE_BTN_ELEVATOR_FLOOR = "background-color: rgba(255, 255, 255, 0.9); color: #007AFF; border: 1px solid #007AFF; border-radius: 4px; padding: 10px; font-weight: 600;";
        }

        // --- 字体配置 (Fonts) ---
        namespace Fonts {
            const int SIZE_LINEN_COUNT = 18; // Smaller font size
            constexpr const char* COL_LINEN_COUNT = "#FF3B30"; // Red color for visibility
        }

        // --- 坐标与尺寸 (Geometry) ---
        // 注意: 所有 (x, y) 坐标均为控件中心点，而非左上角。
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

            // --- 1. 入口场景 (Entrance) - 不规则区域 ---
            // 定义4个点的坐标 (相对于 896x720 区域)
            // Enter Hotel Area
            const QPoint PT_ENTRANCE_ENTER_1(625, 307);
            const QPoint PT_ENTRANCE_ENTER_2(723, 285);
            const QPoint PT_ENTRANCE_ENTER_3(725, 658);
            const QPoint PT_ENTRANCE_ENTER_4(626, 631);

            // Go Home Button (Arrow)
            const QRect RECT_BTN_ENTRANCE_HOME(261, 655, 100, 50); // Default position
            const int ANGLE_BTN_ENTRANCE_HOME = 110;
            constexpr const char* TEXT_BTN_ENTRANCE_HOME = "下班回家";

            // Helper to get Polygons
            static const QPolygon POLY_ENTRANCE_ENTER() {
                QVector<QPoint> v;
                v << PT_ENTRANCE_ENTER_1 << PT_ENTRANCE_ENTER_2 << PT_ENTRANCE_ENTER_3 << PT_ENTRANCE_ENTER_4;
                return QPolygon(v);
            }

            // --- 2. 员工通道 (Staff Hallway) ---
            const QRect RECT_LBL_HALLWAY_STATUS(425, 256, 200, 50);   // 状态标签

            // 箭头按钮配置: 中心点(Rect), 箭头角度(0=右, 90=下, 180=左, 270=上), 箭头长度
            // 返回入口
            const QRect RECT_BTN_HALLWAY_EXIT(512, 686, 150, 50);
            const int ANGLE_BTN_HALLWAY_EXIT = 110;

            // 签到/下班 (也是不规则区域)
            const QPoint PT_HALLWAY_CLOCK_1(157, 280);
            const QPoint PT_HALLWAY_CLOCK_2(202, 286);
            const QPoint PT_HALLWAY_CLOCK_3(201, 392);
            const QPoint PT_HALLWAY_CLOCK_4(155, 392);
             static const QPolygon POLY_HALLWAY_CLOCK() {
                QVector<QPoint> v;
                v << PT_HALLWAY_CLOCK_1 << PT_HALLWAY_CLOCK_2 << PT_HALLWAY_CLOCK_3 << PT_HALLWAY_CLOCK_4;
                return QPolygon(v);
            }

            // 场景切换箭头
            const QRect RECT_BTN_HALLWAY_OFFICE(271, 307, 50, 50);   // 去办公室
            const int ANGLE_BTN_HALLWAY_OFFICE = 270; // 向上偏左? or just Up

            const QRect RECT_BTN_HALLWAY_WAREHOUSE(663, 474, 50, 50);// 去仓库
            const int ANGLE_BTN_HALLWAY_WAREHOUSE = 270; // 右

            const QRect RECT_BTN_HALLWAY_ELEVATOR(537, 495, 50, 50); // 去电梯
            const int ANGLE_BTN_HALLWAY_ELEVATOR = 225;

            // --- 3. 办公室 (Office) ---
            const QRect RECT_BTN_OFFICE_ACTION(490, 352, 150, 40);    // 汇报/领任务 (普通按钮)
            const QRect RECT_LBL_OFFICE_MSG(425, 256, 300, 50);       // 消息提示
            const QRect RECT_BTN_OFFICE_BACK(297, 665, 50, 50);      // 返回通道 (箭头)
            const int ANGLE_BTN_OFFICE_BACK = 110;

            // --- 4. 仓库入口 (Warehouse Entry) ---
            const QRect RECT_BTN_WAREHOUSE_TAKE(541, 357, 150, 40);   // 拿取布草
            const QRect RECT_BTN_WAREHOUSE_BACK(419, 674, 50, 50);   // 返回通道 (箭头)
            const int ANGLE_BTN_WAREHOUSE_BACK = 110;

            // --- 5. 仓库货架 (Warehouse Shelf) ---
            const QRect RECT_BTN_SHELF_BACK(201, 688, 50, 50);       // 返回仓库入口 (箭头)
            const int ANGLE_BTN_SHELF_BACK = 110;

            // 货架物品区域 (中心坐标) - 保持不变
            const QRect AREA_SHEET(348, 568, 120, 120);      //大床单
            const QRect AREA_DUVET(754, 130, 120, 120);       //大被套
            const QRect AREA_S_DUVET(262, 130, 120, 120);     //小被套
            const QRect AREA_PILLOW(216, 410, 120, 120);      //枕巾
            const QRect AREA_GN_TOWEL(517, 568, 120, 120);    //晚安巾
            const QRect AREA_TOWEL(757, 568, 120, 120);       //毛巾

            // --- 6. 电梯厅 (Elevator Hall) ---
            const QRect RECT_BTN_ELEVATOR_ENTER(656, 368, 150, 40);  // 进入电梯
            const QRect RECT_BTN_ELEVATOR_BACK(186, 655, 50, 50);    // 返回 (箭头)
            const int ANGLE_BTN_ELEVATOR_BACK = 120;

            // --- 7. 电梯内 (Elevator Inside) ---
            const QRect RECT_BTN_ELEVATOR_EXIT(447, 340, 150, 40); // 出电梯按钮

            // 侧边栏按钮布局
            const QSize SIZE_ELEVATOR_BTN_SIDEBAR(50, 50); // 侧边栏按钮大小
            const int GRID_SPACING_ELEVATOR = 10;

            // --- 8. 楼层走廊 (Floor Corridor) ---
            const QRect RECT_BTN_CORRIDOR_LINEN(457, 488, 50, 50);    // 去布草间 (箭头)
            const int ANGLE_BTN_CORRIDOR_LINEN = 280;
            const QRect RECT_BTN_CORRIDOR_ELEVATOR(439, 647, 50, 50); // 去电梯厅 (箭头)
            const int ANGLE_BTN_CORRIDOR_ELEVATOR = 100;

            // --- 9. 布草间 (Linen Room) ---
            const QRect RECT_BTN_LINEN_BACK(308, 682, 50, 50);         // 返回走廊 (箭头)
            const int ANGLE_BTN_LINEN_BACK = 110;

            // 紧急任务 脏布草 (Event B) 位置配置
            const QRect RECT_EVENT_DIRTY_LINEN(621, 628, 120, 120); // 突发事件B 脏布草位置

            // 货架物品区域 - 楼层布草间专用
            const QRect AREA_LINEN_SHEET(600, 365, 120, 120);    //大床单
            const QRect AREA_LINEN_DUVET(350, 540, 120, 120);    //大被套
            const QRect AREA_LINEN_S_DUVET(600, 550, 120, 120);  //小被套
            const QRect AREA_LINEN_PILLOW(350, 185, 120, 120);   //枕巾
            const QRect AREA_LINEN_GN_TOWEL(350, 360, 120, 120); //晚安巾
            const QRect AREA_LINEN_TOWEL(600, 185, 120, 120);    //毛巾

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
