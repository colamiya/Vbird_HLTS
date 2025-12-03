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
            const int SIDEBAR_LEFT_WIDTH = 150;
            const int SIDEBAR_RIGHT_WIDTH = 330;

            // 中央游戏区域尺寸
            const QSize CENTER_PANEL_SIZE(1270, 850);

            // 悬浮提示框几何 (相对于 CenterPanel)
            const QRect RECT_HOVER_HINT(66, 30, 1137, 50);

            // [场景: 入口]
            const QRect RECT_BTN_ENTRANCE_HOME(145, 790, 100, 50); // 下班回家
            const int ANGLE_BTN_ENTRANCE_HOME = 180;              // 指向左
            static inline QPolygon POLY_ENTRANCE_ENTER()          // 进入酒店 (不规则区域)
            {
                QVector<QPoint> pts;
                pts << QPoint(884, 365) << QPoint(1026, 337) << QPoint(1029, 779) << QPoint(885, 743);
                return QPolygon(pts);
            }

            // [场景: 员工通道]
            const QRect RECT_BTN_HALLWAY_EXIT(870, 795, 100, 50);       // 返回入口
            const int ANGLE_BTN_HALLWAY_EXIT = 105;                     // 左
            static inline QPolygon POLY_HALLWAY_CLOCK()                 // 打卡机
            {
                QVector<QPoint> pts;
                pts << QPoint(226, 331) << QPoint(290, 338) << QPoint(288, 462) << QPoint(222, 462);
                return QPolygon(pts);
            }
            const QRect RECT_BTN_HALLWAY_OFFICE(437, 796, 100, 50);     // 去办公室 (左侧门)
            const int ANGLE_BTN_HALLWAY_OFFICE = 225;
            const QRect RECT_BTN_HALLWAY_WAREHOUSE(924, 584, 100, 50);  // 去仓库 (中间门)
            const int ANGLE_BTN_HALLWAY_WAREHOUSE = 285;                 // 向内/上
            const QRect RECT_BTN_HALLWAY_ELEVATOR(754, 583, 100, 50);   // 去电梯厅 (右侧通道)
            const int ANGLE_BTN_HALLWAY_ELEVATOR = 225;

            // [场景: 办公室]
            const QRect RECT_BTN_OFFICE_ACTION(734, 409, 160, 50);      // 领任务/汇报
            const QRect RECT_BTN_OFFICE_BACK(132, 790, 100, 50);        // 返回
            const int ANGLE_BTN_OFFICE_BACK = 180;
            const QRect RECT_LBL_OFFICE_MSG(633, 282, 300, 50);         // 汇报成功提示

            // [场景: 仓库入口]
            static inline QPolygon POLY_WAREHOUSE_BIN()
            {
                QVector<QPoint> pts;
                pts << QPoint(627, 322) << QPoint(883, 305) << QPoint(882, 737) << QPoint(626, 683);
                return QPolygon(pts);
            }
            const QRect RECT_BTN_WAREHOUSE_TAKE(258, 420, 160, 50);    // 拿布草 (进入货架)
            const QRect RECT_BTN_WAREHOUSE_BACK(582, 788, 100, 50);     // 返回
            const int ANGLE_BTN_WAREHOUSE_BACK = 90;

            // [场景: 仓库货架]
            const QRect RECT_BTN_SHELF_BACK(132, 790, 100, 50);
            const int ANGLE_BTN_SHELF_BACK = 180;
            // 货架格子区域 (中心点 + 尺寸)
            const QSize SIZE_SHELF_SLOT(140, 140);
            const QRect AREA_SHEET(481, 315, 140, 140);     // 大床单
            const QRect AREA_DUVET(696, 315, 140, 140);     // 大被套
            const QRect AREA_S_DUVET(1077, 315, 140, 140);   // 小被套
            const QRect AREA_PILLOW(481, 676, 140, 140);    // 枕巾
            const QRect AREA_GN_TOWEL(696, 676, 140, 140);  // 晚安巾
            const QRect AREA_TOWEL(1077, 676, 140, 140);     // 毛巾

            // [场景: 电梯厅]
            const QRect RECT_BTN_ELEVATOR_ENTER(932, 420, 160, 50);    // 进入电梯
            const QRect RECT_BTN_ELEVATOR_BACK(132, 790, 100, 50);
            const int ANGLE_BTN_ELEVATOR_BACK = 180;

            // [场景: 电梯内部]
            const QRect RECT_BTN_ELEVATOR_EXIT(633, 752, 160, 50);      // 出电梯按钮

            // [场景: 楼层走廊]
            const QRect RECT_BTN_CORRIDOR_LINEN(662, 581, 100, 50);    // 进布草间
            const int ANGLE_BTN_CORRIDOR_LINEN = 260;
            const QRect RECT_BTN_CORRIDOR_ELEVATOR(697, 792, 100, 50); // 去电梯厅
            const int ANGLE_BTN_CORRIDOR_ELEVATOR = 80;

            // [场景: 布草间]
            const QRect RECT_BTN_LINEN_BACK(624, 799, 100, 50);
            const int ANGLE_BTN_LINEN_BACK = 110;
            // 布草间货架布局
            const QRect AREA_LINEN_SHEET(855, 430, 140, 140); // 大床单
            const QRect AREA_LINEN_DUVET(489, 650, 140, 140);  // 大被套
            const QRect AREA_LINEN_S_DUVET(855, 650, 140, 140); // 小被套
            const QRect AREA_LINEN_PILLOW(489, 217, 140, 140); // 枕巾
            const QRect AREA_LINEN_GN_TOWEL(489, 430, 140, 140); // 晚安巾
            const QRect AREA_LINEN_TOWEL(855, 217, 140, 140);  // 毛巾
            const QRect RECT_EVENT_DIRTY_LINEN(260, 717, 120, 120);     // 脏布草生成位置

            // [UI元素]
            const QSize RETURN_BTN_SIZE(100, 40);
            const QSize TUTORIAL_BTN_SIZE(100, 40);
            const QSize ICON_CART(120, 120);
            const QSize ICON_INVENTORY(64, 64);
            const QSize ICON_SHELF_ITEM(100, 100);
            const QSize ICON_DIRTY_DRAG(80, 80);

            // 电梯面板按钮 (右侧栏)
            const int GRID_SPACING_ELEVATOR = 10;
            const QSize SIZE_ELEVATOR_BTN_SIDEBAR(80, 60);

            // 申领表弹窗
            //const QSize SHEET_DIALOG(600, 800);
            //const QSize SHEET_TEXT_BOX(80, 30); // 填数字的格
            //const QPoint TXT_FLOOR(135, 145);   // 楼层填空位置
            //const QPoint TXT_SHEET(460, 240);
            //const QPoint TXT_DUVET(460, 290);
            //const QPoint TXT_S_DUVET(460, 340);
            //const QPoint TXT_PILLOW(460, 390);
            //const QPoint TXT_GN_TOWEL(460, 440);
            //const QPoint TXT_TOWEL(460, 490);

            const QRect RECT_BTN_MARK_COMPLETE(450, 720, 100, 40);

            // 任务列表表格展开高度
            const int HEIGHT_TASK_TABLE_EXPANDED = 250;

            // 教程弹窗
            const QRect RECT_TUTORIAL_OVERLAY(0, 0, 800, 600); // 相对全屏居中
            const QRect RECT_TUTORIAL_IMAGE(50, 50, 700, 350);
            const QRect RECT_TUTORIAL_TEXT(50, 420, 700, 100);

            // 货架标签坐标 (默认: 区域中心 X, 区域中心 Y + 30)
            const QPoint POS_LBL_SHEET(481, 345);
            const QPoint POS_LBL_DUVET(696, 345);
            const QPoint POS_LBL_S_DUVET(1077, 345);
            const QPoint POS_LBL_PILLOW(481, 706);
            const QPoint POS_LBL_GN_TOWEL(696, 706);
            const QPoint POS_LBL_TOWEL(1077, 706);
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
            constexpr const char *TUTORIAL_GENERAL = "点击蓝色箭头可以切换场景，还有其他可点击的隐藏交互区域吼";
            constexpr const char *TUTORIAL_SHELF = "1、拖动物品栏需要补充的布草 2、拖至对应的柜子中 3.在右上角上点击圆圈标记完成进度";
            constexpr const char *TUTORIAL_WAREHOUSE_ENTRY = "可将脏布草拖到脏布草槽进行回收。";
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
            constexpr const char *LBL_TITLE_RIGHT = "color: #2c3e50; font-weight: bold; font-size: 16px; margin: 10px;";

            constexpr const char *LBL_HOVER_HINT = "background-color: rgba(0,0,0,180); color: white; border-radius: 5px; padding: 5px; font-size: 14px;";
            constexpr const char *LBL_SUCCESS_GREEN = "color: #27ae60; font-weight: bold; font-size: 18px; border: 2px solid #27ae60; padding: 10px; background: rgba(255,255,255,0.8);";

            // 按钮样式
            constexpr const char *BTN_RETURN_MENU = "QPushButton { background-color: #e74c3c; color: white; border-radius: 5px; } QPushButton:hover { background-color: #c0392b; }";
            constexpr const char *BTN_TUTORIAL = "QPushButton { background-color: #f1c40f; color: #2c3e50; border-radius: 5px; font-weight: bold; } QPushButton:hover { background-color: #f39c12; }";
            constexpr const char *BTN_VIEW_TASK_SHEET = "padding: 8px; margin: 5px;";
            constexpr const char *BTN_ORANGE = "background-color: #e67e22; color: white; border-radius: 4px;";

            // 统一玻璃风格按钮 (Unified Glass Style)
            // 蓝色边框，白色半透明背景，蓝色字体
            constexpr const char *STYLE_BTN_UNIFIED = "QPushButton { border: 2px solid #3498db; background-color: rgba(255, 255, 255, 0.8); color: #3498db; border-radius: 10px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: rgba(255, 255, 255, 0.95); } QPushButton:disabled { color: gray; border-color: gray; }";

            // 列表样式
            constexpr const char *LIST_WIDGET = "QTreeWidget { background: transparent; border: none; font-size: 14px; } QTreeWidget::item { padding: 5px; }";

            // 货架区域样式 (白色底，灰色边框)
            constexpr const char *SHELF_AREA = "background-color: white; border: 1px solid #7f8c8d; border-radius: 4px;";

            // 箭头样式
            constexpr const char *ARROW_TEXT_COLOR = "#0000FF"; // 蓝色
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
            const int MAX_TASK_ITEM_TYPES = 6;
            // 随机任务单品数量上限
            const int MAX_TASK_ITEM_COUNT = 6;
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
            constexpr const char *SCENE_HALLWAY_NORMAL = ":/source/Test3/走廊-状态2（正常打卡）.jpg";
            constexpr const char *SCENE_HALLWAY_LATE = ":/source/Test3/走廊-状态2（迟到打卡）.jpg";
            constexpr const char *SCENE_HALLWAY_CLOCKED_OUT = ":/source/Test3/走廊-状态3（下班打卡）.jpg";
            constexpr const char *SCENE_OFFICE = ":/source/Test3/办公室.png";
            // 更新仓库入口背景图
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
            constexpr const char *UI_DIRTY_BIN = ":/source/Test3/脏布草回收.png";
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
            //constexpr const char *IMAGE_TUTORIAL_GENERAL = ":/source/Test3/入口.jpg";
            //constexpr const char *IMAGE_TUTORIAL_SHELF = ":/source/Test3/取布草的货架.jpg";
            //constexpr const char *IMAGE_TUTORIAL_WAREHOUSE = ":/source/Test3/仓库1.jpg";
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
