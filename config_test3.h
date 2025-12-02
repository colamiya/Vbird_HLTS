#ifndef CONFIG_TEST3_H
#define CONFIG_TEST3_H

#include <QString>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QVector>
#include <QMap>
#include <QPolygon>
#include "config_global.h"

/**
 * @brief 测试3 (RPG模拟实训) 模块配置
 */
namespace Config
{
    namespace Test3
    {

        // --- 几何布局 (Geometry) ---
        namespace Geometry
        {
            // 侧边栏宽度
            const int SIDEBAR_LEFT_WIDTH = 150; // New smaller left sidebar
            const int SIDEBAR_RIGHT_WIDTH = 330; // Expanded right sidebar
            const int SIDEBAR_WIDTH = 240; // Deprecated, keeping for compatibility if needed, but we should switch to split ones.
            // Actually, we should check if we can remove it. Let's keep it for now but note to use the new ones.
            // Wait, existing code uses SIDEBAR_WIDTH. I will replace usage in code.

            // 中央游戏区域尺寸 (固定分辨率，方便背景图定位)
            // 1440 (Total) - 150 (Left) - 330 (Right) = 960 Width
            // Height = 900
            const QSize CENTER_PANEL_SIZE(960, 900);

            // 悬浮提示框几何 (相对于 CenterPanel)
            const QRect RECT_HOVER_HINT(50, 800, 860, 50);

            // 按钮与交互区域定义 (相对于 CenterPanel)
            // 所有坐标 (x, y) 均为中心点坐标!

            // [场景: 入口]
            const QRect RECT_BTN_ENTRANCE_HOME(110, 840, 100, 50); // 下班回家
            const int ANGLE_BTN_ENTRANCE_HOME = 180;              // 指向左
            static inline QPolygon POLY_ENTRANCE_ENTER()          // 进入酒店 (不规则区域)
            {
                QVector<QPoint> pts;
                // 示例多边形 (门的位置)
                pts << QPoint(300, 100) << QPoint(660, 100) << QPoint(660, 700) << QPoint(300, 700);
                return QPolygon(pts);
            }

            // [场景: 员工通道]
            const QRect RECT_BTN_HALLWAY_EXIT(110, 840, 100, 50);       // 返回入口
            const int ANGLE_BTN_HALLWAY_EXIT = 180;                     // 左
            static inline QPolygon POLY_HALLWAY_CLOCK()                 // 打卡机
            {
                QVector<QPoint> pts;
                pts << QPoint(750, 300) << QPoint(850, 300) << QPoint(850, 450) << QPoint(750, 450);
                return QPolygon(pts);
            }
            const QRect RECT_BTN_HALLWAY_OFFICE(200, 450, 100, 50);     // 去办公室 (左侧门)
            const int ANGLE_BTN_HALLWAY_OFFICE = 180;
            const QRect RECT_BTN_HALLWAY_WAREHOUSE(480, 450, 100, 50);  // 去仓库 (中间门)
            const int ANGLE_BTN_HALLWAY_WAREHOUSE = 90;                 // 向内/上
            const QRect RECT_BTN_HALLWAY_ELEVATOR(760, 450, 100, 50);   // 去电梯厅 (右侧通道)
            const int ANGLE_BTN_HALLWAY_ELEVATOR = 0;

            // [场景: 办公室]
            const QRect RECT_BTN_OFFICE_ACTION(480, 500, 160, 50);      // 领任务/汇报
            const QRect RECT_BTN_OFFICE_BACK(100, 840, 100, 50);        // 返回
            const int ANGLE_BTN_OFFICE_BACK = 180;
            const QRect RECT_LBL_OFFICE_MSG(480, 300, 300, 50);         // 汇报成功提示

            // [场景: 仓库入口]
            const QRect RECT_WAREHOUSE_BIN(200, 600, 120, 160);         // 脏布草回收桶 (旧矩形区域，保留防错)
            // 新的不规则回收区域 (集成在背景中)
            static inline QPolygon POLY_WAREHOUSE_BIN()
            {
                QVector<QPoint> pts;
                pts << QPoint(50, 500) << QPoint(300, 500) << QPoint(300, 850) << QPoint(50, 850);
                return QPolygon(pts);
            }
            const QRect RECT_BTN_WAREHOUSE_TAKE(700, 400, 150, 150);    // 拿布草 (进入货架)
            const QRect RECT_BTN_WAREHOUSE_BACK(100, 840, 100, 50);     // 返回
            const int ANGLE_BTN_WAREHOUSE_BACK = 180;

            // [场景: 仓库货架]
            const QRect RECT_BTN_SHELF_BACK(100, 840, 100, 50);
            const int ANGLE_BTN_SHELF_BACK = 180;
            // 货架格子区域 (中心点 + 尺寸)
            const QSize SIZE_SHELF_SLOT(140, 140);
            const QRect AREA_SHEET(250, 250, 140, 140);     // 大床单
            const QRect AREA_DUVET(480, 250, 140, 140);     // 大被套
            const QRect AREA_S_DUVET(710, 250, 140, 140);   // 小被套
            const QRect AREA_PILLOW(250, 550, 140, 140);    // 枕巾
            const QRect AREA_GN_TOWEL(480, 550, 140, 140);  // 晚安巾
            const QRect AREA_TOWEL(710, 550, 140, 140);     // 毛巾

            // [场景: 电梯厅]
            const QRect RECT_BTN_ELEVATOR_ENTER(480, 450, 120, 200);    // 进入电梯
            const QRect RECT_BTN_ELEVATOR_BACK(100, 840, 100, 50);
            const int ANGLE_BTN_ELEVATOR_BACK = 180;

            // [场景: 电梯内部]
            const QRect RECT_BTN_ELEVATOR_EXIT(480, 800, 150, 50);      // 出电梯按钮

            // [场景: 楼层走廊]
            const QRect RECT_BTN_CORRIDOR_LINEN(200, 450, 150, 150);    // 进布草间
            const int ANGLE_BTN_CORRIDOR_LINEN = 180;
            const QRect RECT_BTN_CORRIDOR_ELEVATOR(760, 450, 150, 150); // 去电梯厅
            const int ANGLE_BTN_CORRIDOR_ELEVATOR = 0;

            // [场景: 布草间]
            const QRect RECT_BTN_LINEN_BACK(100, 840, 100, 50);
            const int ANGLE_BTN_LINEN_BACK = 180;
            // 布草间货架布局 (与仓库一致)
            const QRect AREA_LINEN_SHEET(250, 250, 140, 140);
            const QRect AREA_LINEN_DUVET(480, 250, 140, 140);
            const QRect AREA_LINEN_S_DUVET(710, 250, 140, 140);
            const QRect AREA_LINEN_PILLOW(250, 550, 140, 140);
            const QRect AREA_LINEN_GN_TOWEL(480, 550, 140, 140);
            const QRect AREA_LINEN_TOWEL(710, 550, 140, 140);
            const QRect RECT_EVENT_DIRTY_LINEN(850, 750, 120, 120);     // 脏布草生成位置

            // [UI元素]
            const QSize RETURN_BTN_SIZE(100, 40);
            const QSize TUTORIAL_BTN_SIZE(100, 40);
            const QSize ICON_CART(120, 120);
            const QSize ICON_INVENTORY(64, 64);
            const QSize ICON_SHELF_ITEM(100, 100);
            const QSize ICON_DIRTY_DRAG(80, 80);

            // 电梯面板按钮 (右侧栏)
            const int GRID_SPACING_ELEVATOR = 10;
            const QSize SIZE_ELEVATOR_BTN_SIDEBAR(60, 40);

            // 申领表弹窗
            const QSize SHEET_DIALOG(600, 800);
            const QSize SHEET_TEXT_BOX(80, 30); // 填数字的格
            const QPoint TXT_FLOOR(135, 145);   // 楼层填空位置
            const QPoint TXT_SHEET(460, 240);
            const QPoint TXT_DUVET(460, 290);
            const QPoint TXT_S_DUVET(460, 340);
            const QPoint TXT_PILLOW(460, 390);
            const QPoint TXT_GN_TOWEL(460, 440);
            const QPoint TXT_TOWEL(460, 490);

            const QRect RECT_BTN_MARK_COMPLETE(450, 720, 100, 40);

            // 教程弹窗
            const QRect RECT_TUTORIAL_OVERLAY(0, 0, 800, 600); // 相对全屏居中
            const QRect RECT_TUTORIAL_IMAGE(50, 50, 700, 350);
            const QRect RECT_TUTORIAL_TEXT(50, 420, 700, 100);
            // const QRect RECT_BTN_TUTORIAL_CLOSE(350, 530, 100, 40); // Removed standard close button
        }

        // --- 文本内容 (Texts) ---
        namespace Texts
        {
            // 按钮文本
            constexpr const char *BTN_TEXT_BACK_TO_MENU = "返回主菜单";
            constexpr const char *BTN_TUTORIAL = "小技巧";
            constexpr const char *BTN_CLOCK_IN = "上班打卡";
            constexpr const char *BTN_CLOCK_OUT = "下班打卡";
            constexpr const char *BTN_GET_TASK = "领取任务";
            constexpr const char *BTN_REPORT_WORK = "汇报工作";
            constexpr const char *BTN_TAKE_LINEN = "拿取布草";
            constexpr const char *BTN_ENTER_ELEVATOR = "进入电梯";
            constexpr const char *BTN_EXIT_ELEVATOR = "出电梯";
            constexpr const char *BTN_VIEW_TASK_SHEET = "查看申领表";

            // 场景交互提示 (Tooltip)
            constexpr const char *BTN_ENTER_HOTEL = "进入酒店";
            constexpr const char *BTN_RETURN_ENTRANCE = "返回入口";
            constexpr const char *BTN_GO_OFFICE = "去办公室";
            constexpr const char *BTN_GO_WAREHOUSE = "去仓库";
            constexpr const char *BTN_GO_ELEVATOR = "去电梯厅";
            constexpr const char *BTN_RETURN_HALLWAY = "返回员工通道";
            constexpr const char *BTN_GO_LINEN_ROOM = "进入布草间";
            constexpr const char *BTN_GO_ELEVATOR_HALL = "去电梯厅";
            constexpr const char *BTN_RETURN_BACK = "返回";
            constexpr const char *BTN_RETURN_CORRIDOR = "返回走廊";
            constexpr const char *BTN_RETURN_WAREHOUSE_ENTRY = "返回仓库入口";
            constexpr const char *TEXT_BTN_ENTRANCE_HOME = "下班回家";

            // 标题文本
            constexpr const char *LBL_LOCATION_PREFIX = "当前位置: ";
            constexpr const char *LBL_CART_EMPTY = "空车";
            constexpr const char *LBL_CART_HAS_ITEMS = "装载中";
            constexpr const char *LBL_CART_DIRTY = "脏布草";
            constexpr const char *LBL_TASK_TITLE = "今日任务";
            constexpr const char *LBL_INVENTORY_TITLE = "物品推车";
            constexpr const char *LBL_ELEVATOR_PANEL_TITLE = "电梯面板";
            constexpr const char *LBL_WORK_REPORTED = "工作已汇报";
            constexpr const char *LBL_DIRTY_BIN_TIP = "脏布草回收处 (拖入)";

            // 状态文本
            constexpr const char *STATUS_IN_PROGRESS = "[进行中]";
            constexpr const char *STATUS_MARKED_COMPLETE = "[标记完成]";

            // 消息/弹窗文本
            constexpr const char *POPUP_EMERGENCY_MANAGER = "经理: %1楼客人急需布草，请立即优先处理！";
            constexpr const char *REPORT_BLOCK_MSG = "请先在任务列表或申领表中将所有任务标记为“完成”后再汇报。";
            constexpr const char *REPORT_SUCCESS = "工作汇报完毕。";
            constexpr const char *REPORT_ERR_MARKED_INCOMPLETE = "警告: 你标记了%1楼完成，但实际并未满足需求。";
            constexpr const char *REPORT_ERR_MISSING_TASK = "错误: 有任务未完成。";
            constexpr const char *REPORT_ERR_LATE = "考勤: 迟到";
            constexpr const char *REPORT_ERR_PRIORITY = "错误: 未优先处理紧急任务";
            constexpr const char *ERR_LOG_MIXED_LINEN = "错误: 脏净布草混装";

            // 教程文本 (Updated for Speech Bubbles)
            constexpr const char *TUTORIAL_GENERAL = "点击蓝色箭头可以切换场景，但还有其他可点击的区域等待你探索。";
            constexpr const char *TUTORIAL_SHELF = "1、拖动物品栏需要补充的布草 2、拖至对应的柜子中 3.在右上角上点击圆圈标记完成进度";
            // Note: The third point "click circle" refers to the task sidebar check action, not the shelf itself.
            constexpr const char *TUTORIAL_WAREHOUSE_ENTRY = "可将脏布草拖到脏布草槽进行回收。";
            // For Warehouse Shelf specific tip (Drag from shelf to cart)
            constexpr const char *TUTORIAL_WAREHOUSE_SHELF_ACTION = "可通过拖动，将货架上的物品拖到右侧的物品栏里。";
        }

        // --- 样式 (Styles) ---
        namespace Styles
        {
            // 侧边栏样式
            constexpr const char *SIDEBAR_LEFT = "background-color: rgba(52, 73, 94, 0.9); border-right: 1px solid #7f8c8d;";
            constexpr const char *SIDEBAR_RIGHT = "background-color: rgba(236, 240, 241, 0.9); border-left: 1px solid #bdc3c7;";

            // 标签样式
            constexpr const char *LBL_TITLE = "color: white; font-weight: bold; font-size: 16px; margin: 10px;";
            // Note: Right sidebar uses dark text now because bg is light
            constexpr const char *LBL_TITLE_RIGHT = "color: #2c3e50; font-weight: bold; font-size: 16px; margin: 10px;";

            constexpr const char *LBL_HOVER_HINT = "background-color: rgba(0,0,0,180); color: white; border-radius: 5px; padding: 5px; font-size: 14px;";
            constexpr const char *LBL_SUCCESS_GREEN = "color: #27ae60; font-weight: bold; font-size: 18px; border: 2px solid #27ae60; padding: 10px; background: rgba(255,255,255,0.8);";

            // 按钮样式
            constexpr const char *BTN_RETURN_MENU = "QPushButton { background-color: #e74c3c; color: white; border-radius: 5px; } QPushButton:hover { background-color: #c0392b; }";
            constexpr const char *BTN_TUTORIAL = "QPushButton { background-color: #f1c40f; color: #2c3e50; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #f39c12; }";
            constexpr const char *BTN_VIEW_TASK_SHEET = "padding: 8px; margin: 5px;";
            constexpr const char *BTN_ORANGE = "background-color: #e67e22; color: white; border-radius: 4px;";

            constexpr const char *STYLE_BTN_OFFICE_ACTION = "QPushButton { background-color: #3498db; color: white; font-size: 16px; border-radius: 8px; } QPushButton:hover { background-color: #2980b9; }";
            constexpr const char *STYLE_BTN_WAREHOUSE_TAKE = "QPushButton { background-color: rgba(46, 204, 113, 0.8); color: white; font-size: 18px; border-radius: 75px; border: 2px solid white; } QPushButton:hover { background-color: rgba(39, 174, 96, 0.9); }";

            constexpr const char *STYLE_BTN_ELEVATOR_ENTER = "QPushButton { background-color: rgba(0,0,0,0.3); border: 2px dashed rgba(255,255,255,0.7); color: white; font-weight: bold; } QPushButton:hover { background-color: rgba(0,0,0,0.5); }";
            constexpr const char *STYLE_BTN_ELEVATOR_EXIT = "QPushButton { background-color: #e74c3c; color: white; border-radius: 10px; font-size: 16px; }";
            constexpr const char *STYLE_BTN_ELEVATOR_FLOOR = "QPushButton { background-color: #444444; color: white; border: 1px solid #666; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #666; } QPushButton:pressed { background-color: #222; }";

            constexpr const char *STYLE_BTN_MARK_COMPLETE = "QPushButton { background-color: #27ae60; color: white; border-radius: 5px; font-size: 14px; } QPushButton:hover { background-color: #2ecc71; }";
            constexpr const char *STYLE_LBL_CHECKMARK = "color: green; font-size: 120px; font-weight: bold; background: transparent;";

            // 列表样式
            constexpr const char *LIST_WIDGET = "QTreeWidget { background: transparent; border: none; font-size: 14px; } QTreeWidget::item { padding: 5px; }";

            // 货架区域样式
            constexpr const char *SHELF_AREA = "background-color: rgba(255, 255, 255, 0.2); border: 1px dashed rgba(0,0,0,0.3); border-radius: 10px;";

            // 箭头样式 (Custom Widget uses these colors)
            constexpr const char *ARROW_TEXT_COLOR = "#0000FF"; // Blue Solid (Replaces previous logic)
            const int ARROW_TEXT_SIZE = 14;

            // 教程
            constexpr const char *TUTORIAL_TEXT_STYLE = "color: white; font-size: 18px; font-weight: bold;";
        }

        // --- 逻辑参数 (Logic) ---
        namespace Logic
        {
            // 迟到阈值 (秒)
            const int TIME_LATE_THRESHOLD_SEC = 10;
            // 固定任务楼层
            const int TASK_FIXED_FLOOR_1 = 6;
            const int TASK_FIXED_FLOOR_2 = 7;
            // 随机任务物品种类上限
            const int MAX_TASK_ITEM_TYPES = 3;
            // 随机任务单品数量上限
            const int MAX_TASK_ITEM_COUNT = 3;
            // 推车最大容量
            const int MAX_CART_ITEMS = 40;

            // 量词映射
            static inline QMap<QString, QString> CLASSIFIERS()
            {
                QMap<QString, QString> m;
                m["大床单"] = "条";
                m["大被套"] = "条";
                m["小被套"] = "条";
                m["枕巾"] = "条";
                m["晚安巾"] = "条";
                m["毛巾"] = "条";
                m["脏布草"] = "件";
                return m;
            }
        }

        // --- 图片路径 (Files/Images) ---
        namespace Images
        {
            // 场景
            constexpr const char *SCENE_ENTRANCE = ":/source/Test3/入口.jpg";
            constexpr const char *SCENE_HALLWAY_NORMAL = ":/source/Test3/楼层走廊-前.png";
            constexpr const char *SCENE_HALLWAY_LATE = ":/source/Test3/走廊-状态2（迟到打卡）.jpg";
            constexpr const char *SCENE_HALLWAY_CLOCKED_OUT = ":/source/Test3/走廊-状态3（下班打卡）.jpg";
            constexpr const char *SCENE_OFFICE = ":/source/Test3/办公室.png";
            // Updated Warehouse Image
            constexpr const char *SCENE_WAREHOUSE_ENTRY = ":/source/Test3/仓库1(集成脏布草回收桶).png";
            constexpr const char *SCENE_WAREHOUSE_SHELF = ":/source/Test3/取布草的货架.jpg";
            constexpr const char *SCENE_ELEVATOR_HALL = ":/source/Test3/电梯厅.jpg";
            constexpr const char *SCENE_ELEVATOR_INSIDE = ":/source/Test3/电梯内.jpg";
            constexpr const char *SCENE_FLOOR_CORRIDOR = ":/source/Test3/楼层走廊-前.png";
            constexpr const char *SCENE_LINEN_ROOM_EMPTY = ":/source/Test3/布草间-空.jpg";

            // UI
            constexpr const char *UI_CART_EMPTY = ":/source/Test3/推车-空.png";
            constexpr const char *UI_CART_CLEAN = ":/source/Test3/推车-布草.png";
            constexpr const char *UI_CART_DIRTY = ":/source/Test3/推车-脏布草.png";
            constexpr const char *UI_DIRTY_BIN = ":/source/Test3/脏布草回收.png"; // Kept for Drag Image if needed, or removed from scene
            constexpr const char *UI_DIRTY_LINEN = ":/source/Test3/脏布草.png";
            constexpr const char *UI_TASK_SHEET = ":/source/Test3/申领表.png";

            // 物品
            static inline QMap<QString, QString> ITEMS()
            {
                QMap<QString, QString> m;
                m["大床单"] = ":/source/Test3/大床单.png";
                m["大被套"] = ":/source/Test3/大被套.png";
                m["小被套"] = ":/source/Test3/小被套.png";
                m["枕巾"] = ":/source/Test3/枕巾.png";
                m["晚安巾"] = ":/source/Test3/晚安巾.png";
                m["毛巾"] = ":/source/Test3/毛巾.png";
                return m;
            }

            // 教程图
            constexpr const char *IMAGE_TUTORIAL_GENERAL = ":/source/Test3/入口.jpg";
            constexpr const char *IMAGE_TUTORIAL_SHELF = ":/source/Test3/取布草的货架.jpg";
            constexpr const char *IMAGE_TUTORIAL_WAREHOUSE = ":/source/Test3/仓库1.jpg";
        }

        // --- 字体配置 (Fonts) ---
        namespace Fonts
        {
            constexpr const char *SHEET_FONT_FAMILY = "Microsoft YaHei";
            const int SHEET_FONT_SIZE = 12;
            const int SHEET_FONT_WEIGHT = 75; // Bold

            constexpr const char *COL_LINEN_COUNT = "red";
            const int SIZE_LINEN_COUNT = 18;
        }

    }
}

#endif // CONFIG_TEST3_H
