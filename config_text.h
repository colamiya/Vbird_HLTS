#ifndef CONFIG_TEXT_H
#define CONFIG_TEXT_H

#include <QString>

/**
 * @brief 文本与颜色配置 (Text Configuration)
 * 包含应用程序中所有文本的字体大小和颜色设置。
 * 所有变量均支持自定义修改。
 */
namespace Config
{
    namespace Text
    {
        // ==========================================
        // 1. 全局设置 (Global Settings)
        // ==========================================

        // 默认字体名称
        constexpr const char *FONT_FAMILY = "Segoe UI, Microsoft YaHei";

        // 全局默认字体大小
        const int SIZE_DEFAULT = 14;
        // 全局默认文字颜色 (深灰)
        constexpr const char *COLOR_DEFAULT = "#333333";

        // 禁用状态文字颜色
        constexpr const char *COLOR_DISABLED = "#8e8e93";

        // ==========================================
        // 2. 首页 (Start Page)
        // ==========================================

        // 标题 "学生基础信息登记"
        const int SIZE_START_TITLE = 24;
        constexpr const char *COLOR_START_TITLE = "#333333";

        // 表单标签 (如 "姓名:", "年龄:")
        const int SIZE_START_LABEL = 14;
        constexpr const char *COLOR_START_LABEL = "#333333";

        // 输入框文字
        const int SIZE_START_INPUT = 14;
        constexpr const char *COLOR_START_INPUT = "#333333";

        // "开始培训" 按钮
        const int SIZE_START_BTN = 18;
        constexpr const char *COLOR_START_BTN_TEXT = "#FFFFFF"; // 白色

        // ==========================================
        // 3. 主菜单 (Main Menu)
        // ==========================================

        // 主标题 "请选择测试项"
        const int SIZE_MENU_TITLE = 32;
        constexpr const char *COLOR_MENU_TITLE = "#333333";

        // 菜单按钮 (测试1, 测试2, 测试3)
        const int SIZE_MENU_BTN = 18;
        constexpr const char *COLOR_MENU_BTN_TEXT = "#FFFFFF";

        // 结束训练按钮
        const int SIZE_MENU_EXIT_BTN = 18;
        constexpr const char *COLOR_MENU_EXIT_BTN_TEXT = "#FFFFFF";

        // ==========================================
        // 4. 测试1: 幻灯片 (Test 1)
        // ==========================================

        // 导航按钮 (上一页, 下一页)
        const int SIZE_TEST1_NAV_BTN = 14;
        constexpr const char *COLOR_TEST1_NAV_BTN_TEXT = "#FFFFFF";

        // 返回按钮
        const int SIZE_TEST1_RETURN_BTN = 14;
        constexpr const char *COLOR_TEST1_RETURN_BTN_TEXT = "#FFFFFF";

        // ==========================================
        // 5. 测试2: 知识测验 (Test 2)
        // ==========================================

        // 题目文字
        const int SIZE_TEST2_QUESTION = 24;
        constexpr const char *COLOR_TEST2_QUESTION = "#333333";

        // 选项文字
        const int SIZE_TEST2_OPTION = 16;
        constexpr const char *COLOR_TEST2_OPTION = "#333333";
        constexpr const char *COLOR_TEST2_OPTION_SELECTED = "#FFFFFF"; // 选中时的文字颜色

        // 提交/下一题按钮
        const int SIZE_TEST2_ACTION_BTN = 16;
        constexpr const char *COLOR_TEST2_ACTION_BTN_TEXT = "#FFFFFF";

        // 结果弹窗标题
        const int SIZE_TEST2_RESULT_TITLE = 24;
        constexpr const char *COLOR_TEST2_RESULT_TITLE = "#333333";

        // ==========================================
        // 6. 测试3: RPG实训 (Test 3)
        // ==========================================

        // --- 侧边栏 (Sidebar) ---
        // 位置标题 (如 "当前位置: 入口")
        const int SIZE_TEST3_SIDEBAR_TITLE = 16;
        constexpr const char *COLOR_TEST3_SIDEBAR_TITLE = "#FFFFFF"; // 左侧深色背景，白字
        constexpr const char *COLOR_TEST3_SIDEBAR_TITLE_RIGHT = "#2c3e50"; // 右侧浅色背景，深字

        // 返回主菜单按钮
        const int SIZE_TEST3_RETURN_BTN = 14;
        constexpr const char *COLOR_TEST3_RETURN_BTN_TEXT = "#FFFFFF";

        // 教程按钮
        const int SIZE_TEST3_TUTORIAL_BTN = 14;
        constexpr const char *COLOR_TEST3_TUTORIAL_BTN_TEXT = "#FFFFFF";

        // 库存/推车 标题
        const int SIZE_TEST3_INVENTORY_TITLE = 16;
        constexpr const char *COLOR_TEST3_INVENTORY_TITLE = "#2c3e50";

        // 电梯面板按钮 (侧边栏)
        const int SIZE_TEST3_ELEVATOR_SIDEBAR_BTN = 16;
        constexpr const char *COLOR_TEST3_ELEVATOR_SIDEBAR_BTN = "#3498db"; // 蓝色文字

        // --- 任务列表 (Task List) ---
        // 任务标题 (如 "任务1: 6层 [进行中]")
        const int SIZE_TEST3_TASK_HEADER = 14;
        constexpr const char *COLOR_TEST3_TASK_HEADER_NORMAL = "#000000";
        constexpr const char *COLOR_TEST3_TASK_HEADER_EMERGENCY = "#FF0000"; // 红色

        // 任务详情表格文字
        const int SIZE_TEST3_TASK_TABLE = 14;
        constexpr const char *COLOR_TEST3_TASK_TABLE_TEXT = "#333333";

        // --- 场景交互 (Scene Interaction) ---
        // 场景内箭头按钮文字 (如 "办公室", "入口")
        const int SIZE_TEST3_ARROW_TEXT = 16;
        constexpr const char *COLOR_TEST3_ARROW_TEXT = "#0000FF"; // 蓝色

        // 场景动作按钮 (如 "领取任务", "进入电梯")
        const int SIZE_TEST3_ACTION_BTN = 16;
        constexpr const char *COLOR_TEST3_ACTION_BTN_TEXT = "#3498db";

        // 货架物品标签 (如 "大床单")
        const int SIZE_TEST3_SHELF_LABEL = 16;
        constexpr const char *COLOR_TEST3_SHELF_LABEL = "#FFFFFF";

        // 物品数量标签 (如 "X5")
        const int SIZE_TEST3_COUNT_LABEL = 18;
        constexpr const char *COLOR_TEST3_COUNT_LABEL = "#FFFFFF";
        constexpr const char *COLOR_TEST3_COUNT_LABEL_DIRTY = "#FF0000"; // 脏布草数量颜色

        // 悬浮提示框 (Hover Hint)
        const int SIZE_TEST3_HOVER_HINT = 14;
        constexpr const char *COLOR_TEST3_HOVER_HINT = "#FFFFFF";

        // --- 弹窗与对话 (Dialogs) ---
        // 经理反馈/教程气泡文字
        const int SIZE_TEST3_DIALOG_TEXT = 16;
        constexpr const char *COLOR_TEST3_DIALOG_TEXT = "#333333";

        // 教程气泡标题/重要文字
        const int SIZE_TEST3_TUTORIAL_TEXT = 16;
        constexpr const char *COLOR_TEST3_TUTORIAL_TEXT = "#333333";

    }
}

#endif // CONFIG_TEXT_H
