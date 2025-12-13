#include "test3.h"
#include "tutorial_overlay.h"
#include <QBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QDialog>
#include <QMap>
#include <QFile>
#include <cmath>
#include <QtMath>
#include <QMessageBox>
#include <QHeaderView>
#include <QCheckBox>
#include <QRegularExpression>
#include "config.h"
#include "utils.h" // Include custom dialogs
#include "logger.h"

// --- 辅助控件：任务项 (Task Item Widget) ---
// 用于在任务列表中显示单个任务的详情（手风琴风格）
class TaskItemWidget : public QWidget {
    Q_OBJECT
public:
    TaskItemWidget(int taskIndex, Task *task, QWidget *parent = nullptr) : QWidget(parent), m_task(task), m_taskIndex(taskIndex) {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(5, 5, 5, 5);
        layout->setSpacing(5);

        // 标题栏按钮 (可点击展开/折叠)
        QString status = m_task->isMarkedComplete ? Config::Test3::Texts::STATUS_MARKED_COMPLETE : Config::Test3::Texts::STATUS_IN_PROGRESS;
        QString titleText = QString("任务%1: %2层 %3 %4")
                                .arg(taskIndex + 1)
                                .arg(m_task->targetFloor)
                                .arg(m_task->isEmergency ? "[紧急]" : "")
                                .arg(status);

        m_headerBtn = new QPushButton(titleText, this);
        // 增强可见性：添加背景色和深色字体
        // 使用 Config::Text 中的变量
        QString baseStyle = QString("text-align: left; font-weight: bold; font-size: %1px; border-radius: 4px; padding: 6px; min-height: 40px;")
            .arg(Config::Text::SIZE_TEST3_TASK_HEADER);

        if (m_task->isEmergency) {
            // 紧急任务：黄色背景，红色字体
            baseStyle += QString("background-color: yellow; color: %1;")
                .arg(Config::Text::COLOR_TEST3_TASK_HEADER_EMERGENCY);
        } else {
             // 普通任务：原有样式
            baseStyle += QString("background-color: #d6eaf8; color: %1;")
                .arg(Config::Text::COLOR_TEST3_TASK_HEADER_NORMAL);
        }

        m_headerBtn->setStyleSheet(baseStyle);
        m_headerBtn->setCursor(Qt::PointingHandCursor);
        connect(m_headerBtn, &QPushButton::clicked, this, &TaskItemWidget::headerClicked);
        layout->addWidget(m_headerBtn);

        // 表格
        int rows = 0;
        for(auto v : m_task->requiredItems) if(v > 0) rows++;

        m_table = new QTableWidget(rows, 3, this);
        // 更新表头：物品、需求量、标记
        m_table->setHorizontalHeaderLabels(QStringList() << "物品" << "需求量" << "标记");

        // 调整列宽比例 50% : 25% : 25% (近似值: 侧边栏宽330 -> 可用约300 -> 160:70:70)
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
        m_table->setColumnWidth(1, 70);
        m_table->setColumnWidth(2, 70);

        m_table->verticalHeader()->setVisible(false);
        m_table->setSelectionMode(QAbstractItemView::NoSelection);
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

        // 使用配置中的字体大小和颜色
        QString tableStyle = QString("background: rgba(255,255,255,0.8); border: 1px solid #bdc3c7; border-radius: 4px; font-size: %1px; color: %2;")
            .arg(Config::Text::SIZE_TEST3_TASK_TABLE)
            .arg(Config::Text::COLOR_TEST3_TASK_TABLE_TEXT);
        m_table->setStyleSheet(tableStyle);

        // 启用滚动
        m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        int row = 0;
        for(auto it = m_task->requiredItems.begin(); it != m_task->requiredItems.end(); ++it) {
            if(it.value() > 0) {
                QString name = it.key();

                QTableWidgetItem *nameItem = new QTableWidgetItem(name);
                nameItem->setTextAlignment(Qt::AlignCenter);
                m_table->setItem(row, 0, nameItem);

                QTableWidgetItem *qtyItem = new QTableWidgetItem(QString::number(it.value()));
                qtyItem->setTextAlignment(Qt::AlignCenter);
                m_table->setItem(row, 1, qtyItem);

                QTableWidgetItem *checkItem = new QTableWidgetItem();
                bool checked = m_task->itemCompletionStatus.value(name, false);
                updateCheckItem(checkItem, checked);
                checkItem->setTextAlignment(Qt::AlignCenter);
                checkItem->setData(Qt::UserRole, name); // Store name
                m_table->setItem(row, 2, checkItem);
                row++;
            }
        }

        connect(m_table, &QTableWidget::cellClicked, [this](int r, int c){
            if(c == 2) {
                QTableWidgetItem *item = m_table->item(r, c);
                QString name = item->data(Qt::UserRole).toString();
                bool current = m_task->itemCompletionStatus.value(name, false);
                m_task->itemCompletionStatus[name] = !current;

                // 更新 UI
                updateCheckItem(item, !current);
            }
        });

        layout->addWidget(m_table);

        // 任务卡片背景 (增加不透明度以提高对比度)
        setStyleSheet("TaskItemWidget { background-color: rgba(255,255,255,0.95); border: 1px solid #95a5a6; border-radius: 8px; }");

        // 默认状态为折叠
        setExpanded(false);
    }

    void setExpanded(bool expanded) {
        m_table->setVisible(expanded);
        if (expanded) {
            m_table->setFixedHeight(Config::Test3::Geometry::HEIGHT_TASK_TABLE_EXPANDED);
        } else {
             m_table->setFixedHeight(0);
        }
    }

    // 获取任务索引
    int getTaskIndex() const { return m_taskIndex; }

    // 公有成员
    int m_taskIndex;

signals:
    void headerClicked();

private:
    Task *m_task;
    QTableWidget *m_table;
    QPushButton *m_headerBtn;

    void updateCheckItem(QTableWidgetItem *item, bool checked) {
        // 创建圆圈打勾图标
        // Size: 24x24
        QPixmap pix(24, 24);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);

        // Draw Circle
        if (checked) {
            p.setBrush(Qt::transparent);
            p.setPen(QPen(Qt::green, 2));
            p.drawEllipse(2, 2, 20, 20);

            // Draw Tick
            QPainterPath path;
            path.moveTo(6, 12);
            path.lineTo(10, 16);
            path.lineTo(18, 8);
            p.drawPath(path);
        } else {
            p.setBrush(Qt::transparent);
            p.setPen(QPen(Qt::gray, 2));
            p.drawEllipse(2, 2, 20, 20);
        }

        item->setIcon(QIcon(pix));
        // Clear text to show icon only (or keep empty)
        item->setText("");
    }
};

Test3::Test3(bool isDevMode, QWidget *parent) : QWidget(parent), isDeveloperMode(isDevMode)
{
    // 主布局 (网格布局)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setContentsMargins(0, 0, 0, 0);
    mainGrid->setSpacing(0);

    QWidget *rpgContainer = new QWidget();
    QHBoxLayout *rpgLayout = new QHBoxLayout(rpgContainer);
    rpgLayout->setContentsMargins(0, 0, 0, 0);
    rpgLayout->setSpacing(0);

    // --- 左侧面板 ---
    QWidget *leftPanel = new QWidget();
    leftPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_LEFT_WIDTH);
    leftPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_LEFT);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);

    // 返回主菜单按钮
    QPushButton *returnBtn = new QPushButton(Config::Test3::Texts::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test3::Geometry::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test3::Styles::GET_BTN_RETURN_MENU());
    returnBtn->setCursor(Qt::PointingHandCursor);
    connect(returnBtn, &QPushButton::clicked, [this]()
            {
        // Custom Confirm Dialog
        if (Utils::ShowCustomConfirmDialog(this, "确认退出", "确定要退出当前实训并返回主菜单吗？\n当前进度将不会保留。")) {
             reset();
             emit levelCancelled();
        } });

    // 新手教程按钮
    tutorialBtn = new QPushButton(Config::Test3::Texts::BTN_TUTORIAL);
    tutorialBtn->setFixedSize(Config::Test3::Geometry::TUTORIAL_BTN_SIZE);
    tutorialBtn->setStyleSheet(Config::Test3::Styles::GET_BTN_TUTORIAL());
    tutorialBtn->setCursor(Qt::PointingHandCursor);
    connect(tutorialBtn, &QPushButton::clicked, [this]()
            { showTutorial(); });

    locationLabel = new QLabel(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + "入口");
    locationLabel->setStyleSheet(Config::Test3::Styles::GET_LBL_TITLE());
    locationLabel->setWordWrap(true);

    // 推车状态图标
    cartStatusLabel = new QLabel();
    cartStatusLabel->setFixedSize(Config::Test3::Geometry::ICON_CART);
    cartStatusLabel->setScaledContents(true);

    // 布局调整
    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->addWidget(locationLabel);
    topLayout->addStretch();

    QVBoxLayout *midLayout = new QVBoxLayout();
    midLayout->addStretch();
    midLayout->addWidget(cartStatusLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

    QVBoxLayout *botLayout = new QVBoxLayout();
    botLayout->addWidget(returnBtn, 0, Qt::AlignCenter);

    leftLayout->addLayout(topLayout, 2);
    leftLayout->addWidget(tutorialBtn, 2, Qt::AlignCenter);
    leftLayout->addLayout(midLayout, 5);
    leftLayout->addLayout(botLayout, 1);

    rpgLayout->addWidget(leftPanel);

    // --- 中间面板 (游戏主画面) ---
    rpgCenterPanel = new QWidget();
    // 移除固定尺寸，改为自适应
    // rpgCenterPanel->setFixedSize(Config::Test3::Geometry::CENTER_PANEL_SIZE);
    rpgCenterPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rpgCenterPanel->setStyleSheet("background-color: #ecf0f1;");
    rpgCenterPanel->installEventFilter(this);
    rpgLayout->addWidget(rpgCenterPanel);

    // 悬浮提示标签 (全局单例)
    hoverHintLabel = new QLabel(rpgCenterPanel);
    hoverHintLabel->setAlignment(Qt::AlignCenter);
    hoverHintLabel->setStyleSheet(Config::Test3::Styles::GET_LBL_HOVER_HINT());
    hoverHintLabel->hide();
    // 转换为中心坐标以适配自适应系统
    QRect hintRect = Config::Test3::Geometry::RECT_HOVER_HINT;
    setGeometryCentered(hoverHintLabel, hintRect.center().x(), hintRect.center().y(), hintRect.width(), hintRect.height());
    // Store original font size for scaling
    hoverHintLabel->setProperty("originalFontSize", Config::Text::SIZE_TEST3_HOVER_HINT);

    // --- 右侧面板 ---
    QWidget *rightPanel = new QWidget();
    rightPanel->setFixedWidth(Config::Test3::Geometry::SIDEBAR_RIGHT_WIDTH);
    rightPanel->setStyleSheet(Config::Test3::Styles::SIDEBAR_RIGHT);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);

    // 1. 任务标题 (已移除)
    // QLabel *taskTitle = new QLabel(Config::Test3::Texts::LBL_TASK_TITLE);
    // taskTitle->setStyleSheet(Config::Test3::Styles::LBL_TITLE_RIGHT);
    // rightLayout->addWidget(taskTitle);

    // 2. 任务列表 (QListWidget with Custom Items) - 占 50%
    taskListWidget = new QListWidget();
    taskListWidget->setStyleSheet("background: transparent; border: none;");
    taskListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    taskListWidget->setSpacing(5);

    rightLayout->addWidget(taskListWidget, 5);

    // 原“查看申领表”按钮已移除

    // 3. 库存标题区域 (HBox for Title + Count)
    QWidget *inventoryHeader = new QWidget();
    QHBoxLayout *invHeaderLayout = new QHBoxLayout(inventoryHeader);
    invHeaderLayout->setContentsMargins(0, 0, 0, 0);

    inventoryTitleLabel = new QLabel(Config::Test3::Texts::LBL_INVENTORY_TITLE);
    inventoryTitleLabel->setStyleSheet(Config::Test3::Styles::GET_LBL_TITLE_RIGHT());

    cartCountLabel = new QLabel("0/40");
    cartCountLabel->setStyleSheet(Config::Test3::Styles::GET_LBL_TITLE_RIGHT());

    invHeaderLayout->addWidget(inventoryTitleLabel);
    invHeaderLayout->addWidget(cartCountLabel);
    invHeaderLayout->addStretch(); // Title on left

    rightLayout->addWidget(inventoryHeader);

    // 4. 库存列表 - 占 50%
    inventoryListWidget = new DraggableListWidget();
    inventoryListWidget->setIconSize(Config::Test3::Geometry::ICON_INVENTORY);
    inventoryListWidget->setStyleSheet("color: black; background: white;");
    inventoryListWidget->onItemDroppedIn = [this](QString itemName, const QMimeData *mimeData)
    {
        handleInventoryDrop(itemName, mimeData);
    };
    rightLayout->addWidget(inventoryListWidget, 5);

    // --- 电梯面板容器 (Sidebar) ---
    elevatorPanelContainer = new QWidget();
    QVBoxLayout *elePanelLayout = new QVBoxLayout(elevatorPanelContainer);
    elePanelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *eleTitle = new QLabel(Config::Test3::Texts::LBL_ELEVATOR_PANEL_TITLE);
    eleTitle->setStyleSheet(Config::Test3::Styles::GET_LBL_TITLE_RIGHT());
    elePanelLayout->addWidget(eleTitle);

    QGridLayout *btnGrid = new QGridLayout();
    btnGrid->setSpacing(Config::Test3::Geometry::GRID_SPACING_ELEVATOR);

    auto createEleBtn = [&](int floor)
    {
        QString txt = (floor == 0) ? "G层" : QString("%1楼").arg(floor);
        QPushButton *btn = new QPushButton(txt);
        btn->setFixedSize(Config::Test3::Geometry::SIZE_ELEVATOR_BTN_SIDEBAR);
        // 使用统一蓝色样式
        btn->setStyleSheet(Config::Test3::Styles::GET_STYLE_BTN_UNIFIED());
        // Override font size specifically for sidebar elevator buttons?
        // GET_STYLE_BTN_UNIFIED uses Config::Text::SIZE_TEST3_ACTION_BTN.
        // We might want SIZE_TEST3_ELEVATOR_SIDEBAR_BTN.
        // Let's replace the font-size if needed.
        QString base = Config::Test3::Styles::GET_STYLE_BTN_UNIFIED();
        QString custom = base + QString(" font-size: %1px; color: %2;").arg(Config::Text::SIZE_TEST3_ELEVATOR_SIDEBAR_BTN).arg(Config::Text::COLOR_TEST3_ELEVATOR_SIDEBAR_BTN);
        btn->setStyleSheet(custom);

        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, [this, floor]()
                { handleElevatorButton(floor); });
        return btn;
    };

    btnGrid->addWidget(createEleBtn(9), 0, 0);
    btnGrid->addWidget(createEleBtn(10), 0, 1);
    btnGrid->addWidget(createEleBtn(7), 1, 0);
    btnGrid->addWidget(createEleBtn(8), 1, 1);
    btnGrid->addWidget(createEleBtn(5), 2, 0);
    btnGrid->addWidget(createEleBtn(6), 2, 1);
    btnGrid->addWidget(createEleBtn(3), 3, 0);
    btnGrid->addWidget(createEleBtn(4), 3, 1);
    btnGrid->addWidget(createEleBtn(0), 4, 0);
    btnGrid->addWidget(createEleBtn(2), 4, 1);

    elePanelLayout->addLayout(btnGrid);
    elePanelLayout->addStretch();

    rightLayout->addWidget(elevatorPanelContainer);
    elevatorPanelContainer->hide();

    rpgLayout->addWidget(rightPanel);
    mainGrid->addWidget(rpgContainer, 0, 0);

    // 迟到计时器
    latenessTimer = new QTimer(this);
    latenessTimer->setSingleShot(true);
    latenessTimer->setInterval(Config::Test3::Logic::TIME_LATE_THRESHOLD_SEC * 1000);
    connect(latenessTimer, &QTimer::timeout, [this]()
            {
        isLate = true;
        emit logMessage("计时结束: 标记为迟到状态");
        if (gameState.currentScene == GameScene::StaffHallway) {
            renderStaffHallway();
        } });

    // 紧急事件计时器
    emergencyTimer = new QTimer(this);
    emergencyTimer->setSingleShot(true);
    connect(emergencyTimer, &QTimer::timeout, this, &Test3::checkEmergencyTask);

    heartbeatTimer = new QTimer(this);
    heartbeatTimer->setInterval(1000);
    connect(heartbeatTimer, &QTimer::timeout, [this]()
            {
        static int count = 0;
        emit logMessage(QString("Test3 Heartbeat: %1s (Main Thread Active)").arg(++count)); });

    reset();
}

void Test3::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_startTime = QDateTime::currentDateTime();
    emit logMessage("Test 3 Session Started at " + m_startTime.toString("HH:mm:ss"));
}

// 缓存获取辅助函数
QPixmap Test3::getPixmap(const QString &path)
{
    if (m_sceneCache.contains(path))
    {
        return m_sceneCache.value(path);
    }
    QPixmap pix(path);
    if (!pix.isNull())
    {
        m_sceneCache.insert(path, pix);
    }
    return pix;
}

void Test3::reset()
{
    emit logMessage("Test3::reset() called");
    gameState.currentScene = GameScene::Entrance;
    gameState.currentFloor = 0;
    gameState.hasClockedIn = false;
    gameState.hasEverClockedIn = false;
    gameState.hasReceivedTask = false;
    gameState.hasReported = false;
    gameState.tasks.clear();
    gameState.inventory.cleanItems.clear();
    gameState.inventory.dirtyItemsCount = 0;
    gameState.dirtyBagState.clear();
    gameState.floorInventory.clear();

    // 重置提示标志
    gameState.hasShownTipEntrance = false;
    gameState.hasShownTipWarehouse = false;
    gameState.hasShownTipShelf = false;
    gameState.hasShownTipLinenRoom = false;

    isLate = false;
    isTimerTriggered = false;
    isEmergencyActive = false;
    latenessTimer->stop();
    emergencyTimer->stop();
    errorLog = ErrorLog();

    taskListWidget->clear();
    inventoryListWidget->clear();

    // 增加轮次计数
    m_roundCount++;

    // 记录开始时间
    m_startTime = QDateTime::currentDateTime();

    emit logMessage(QString("--- 开始第 %1 轮实训 ---").arg(m_roundCount));
    emit logMessage("测试3: 重置状态");
    renderScene();
}

void Test3::installDevFilter(QWidget *widget)
{
    if (!isDeveloperMode)
        return;
    widget->installEventFilter(this);
    const QObjectList &children = widget->children();
    for (QObject *child : children)
    {
        if (child->isWidgetType())
        {
            installDevFilter(static_cast<QWidget *>(child));
        }
    }
}

bool Test3::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && isDeveloperMode)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint globalPos = static_cast<QWidget *>(watched)->mapToGlobal(mouseEvent->pos());
        QPoint localPos = rpgCenterPanel->mapFromGlobal(globalPos);

        QString coordText = QString("DevMode Click: (%1, %2)").arg(localPos.x()).arg(localPos.y());
        qDebug() << coordText;
        emit logMessage(coordText);
    }
    else if (event->type() == QEvent::Resize && watched == rpgCenterPanel)
    {
        updateGameLayout();
    }
    return QWidget::eventFilter(watched, event);
}

void Test3::updateGameLayout()
{
    if (!rpgCenterPanel) return;

    // 计算缩放比例
    QSize currentSize = rpgCenterPanel->size();
    QSize refSize = Config::Test3::Geometry::CENTER_PANEL_SIZE; // 1270x850

    // 使用非保持宽高比缩放 (Stretch to fill)
    float scaleX = (float)currentSize.width() / refSize.width();
    float scaleY = (float)currentSize.height() / refSize.height();

    QList<QObject*> children = rpgCenterPanel->children();
    for (QObject* child : children) {
        QWidget* w = qobject_cast<QWidget*>(child);
        if (!w) continue;

        // 1. 处理 SpeechBubble (特殊处理，覆盖全屏)
        if (qobject_cast<SpeechBubble*>(w)) {
            w->setGeometry(0, 0, currentSize.width(), currentSize.height());
            continue;
        }

        // 2. 处理通用 Widget (包括背景 Label, 按钮等)
        QVariant varRect = w->property("originalGeometry");
        if (varRect.isValid() && varRect.canConvert<QRect>()) {
            QRect origRect = varRect.toRect();

            // 重新计算几何
            // 原始坐标是 Center-Based (CenterX, CenterY, W, H)
            int cx = origRect.x() * scaleX;
            int cy = origRect.y() * scaleY;
            int cw = origRect.width() * scaleX;
            int ch = origRect.height() * scaleY;

            // setGeometry 需要 (Left, Top, W, H)
            w->setGeometry(cx - cw/2, cy - ch/2, cw, ch);

            // 字体缩放处理 (如果有 originalFontSize 属性)
            QVariant varFontSize = w->property("originalFontSize");
            if (varFontSize.isValid()) {
                int origSize = varFontSize.toInt();
                // 使用最小缩放比例，防止压扁时字体截断
                float minScale = qMin(scaleX, scaleY);
                int newSize = static_cast<int>(origSize * minScale);
                if (newSize < 8) newSize = 8; // 最小字体保护

                QString baseStyle = w->property("baseStyleSheet").toString();
                if (baseStyle.isEmpty()) {
                    baseStyle = w->styleSheet();
                    w->setProperty("baseStyleSheet", baseStyle);
                }

                // 使用正则替换 font-size，支持复杂样式表 (如 QPushButton { ... })
                QString newStyle = baseStyle;
                static QRegularExpression regex("font-size\\s*:\\s*\\d+px", QRegularExpression::CaseInsensitiveOption);
                QString replacement = QString("font-size: %1px").arg(newSize);

                if (newStyle.contains(regex)) {
                    newStyle.replace(regex, replacement);
                } else {
                    // 如果没有 font-size，则尝试插入或追加
                    newStyle += QString("; font-size: %1px;").arg(newSize);
                }

                w->setStyleSheet(newStyle);
            }
        }

        // 3. 处理 ClickableArea (Polygon)
        ClickableArea* ca = qobject_cast<ClickableArea*>(w);
        if (ca) {
            // 修复: 强制设置几何为全屏，以确保点击区域有效 (尤其是首次加载时)
            ca->setGeometry(0, 0, currentSize.width(), currentSize.height());
            ca->rescale(scaleX, scaleY);
        }
    }
}

// --- 逻辑处理 (Logic) ---

void Test3::goToScene(GameScene scene)
{
    // emit logMessage(QString("goToScene: %1").arg((int)scene)); // Filtered per request

    if (gameState.currentScene == GameScene::StaffHallway && scene != GameScene::StaffHallway)
    {
        if (!gameState.hasClockedIn && !isTimerTriggered && !isLate)
        {
            isTimerTriggered = true;
            latenessTimer->start();
            emit logMessage("计时开始: 10秒迟到倒计时");
        }
    }

    gameState.currentScene = scene;
    // emit logMessage("移动到场景: " + QString::number((int)scene)); // Filtered per request
    renderScene();
}

void Test3::updateRPGStatusLabels()
{
    QString locStr;
    switch (gameState.currentScene)
    {
    case GameScene::Entrance: locStr = "入口"; break;
    case GameScene::StaffHallway: locStr = "员工通道"; break;
    case GameScene::Office: locStr = "办公室"; break;
    case GameScene::Warehouse: locStr = "布草仓库(入口)"; break;
    case GameScene::WarehouseShelf: locStr = "布草仓库(货架)"; break;
    case GameScene::ElevatorHall: locStr = (gameState.currentFloor == 0) ? "电梯厅 (G)" : QString("%1楼 电梯厅").arg(gameState.currentFloor); break;
    case GameScene::ElevatorInside: locStr = "电梯内"; break;
    case GameScene::FloorCorridor: locStr = QString("%1楼 走廊").arg(gameState.currentFloor); break;
    case GameScene::LinenRoom: locStr = QString("%1楼 布草间").arg(gameState.currentFloor); break;
    }
    locationLabel->setText(QString(Config::Test3::Texts::LBL_LOCATION_PREFIX) + "\n" + locStr);

    QString statusImg;
    int cleanCount = 0;
    for (auto v : gameState.inventory.cleanItems)
        cleanCount += v;

    if (gameState.inventory.dirtyItemsCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_DIRTY;
    } else if (cleanCount > 0) {
        statusImg = Config::Test3::Images::UI_CART_CLEAN;
    } else {
        statusImg = Config::Test3::Images::UI_CART_EMPTY;
    }

    QPixmap pix = getPixmap(statusImg);
    if (pix.isNull()) {
        cartStatusLabel->setText(gameState.inventory.dirtyItemsCount > 0 ? Config::Test3::Texts::LBL_CART_DIRTY
                                                                         : (cleanCount > 0 ? Config::Test3::Texts::LBL_CART_HAS_ITEMS : Config::Test3::Texts::LBL_CART_EMPTY));
        cartStatusLabel->setStyleSheet("border: 1px solid white; color: white;");
    } else {
        cartStatusLabel->setPixmap(pix);
        cartStatusLabel->setText("");
        cartStatusLabel->setStyleSheet("border: none;");
    }
}

void Test3::refreshInventoryList()
{
    inventoryListWidget->clear();
    int totalCount = 0;
    for (auto it = gameState.inventory.cleanItems.begin(); it != gameState.inventory.cleanItems.end(); ++it)
    {
        if (it.value() > 0)
        {
            totalCount += it.value();
            QListWidgetItem *item = new QListWidgetItem();
            QString classifier = Config::Test3::Logic::CLASSIFIERS().value(it.key(), "个");
            item->setText(QString("%1%2 %3").arg(it.value()).arg(classifier).arg(it.key()));
            item->setData(Qt::UserRole, it.key());

            QString iconPath = Config::Test3::Images::ITEMS().value(it.key());
            if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
            inventoryListWidget->addItem(item);
        }
    }

    if (gameState.inventory.dirtyItemsCount > 0)
    {
        totalCount += gameState.inventory.dirtyItemsCount;
        QListWidgetItem *item = new QListWidgetItem();
        QString classifier = Config::Test3::Logic::CLASSIFIERS().value("脏布草", "件");
        item->setText(QString("%1%2 脏布草").arg(gameState.inventory.dirtyItemsCount).arg(classifier));
        QString iconPath = Config::Test3::Images::UI_DIRTY_LINEN;
        if (QFile::exists(iconPath)) item->setIcon(QIcon(iconPath));
        item->setData(Qt::UserRole, "DirtyLinen");
        inventoryListWidget->addItem(item);
    }

    // 更新数量标签
    if (cartCountLabel) {
        cartCountLabel->setText(QString("%1/%2").arg(totalCount).arg(Config::Test3::Logic::MAX_CART_ITEMS));
    }

    updateRPGStatusLabels();
}

// 刷新任务列表 (Updated to use Custom Widget)
void Test3::refreshTaskList()
{
    taskListWidget->clear();

    // 排序任务：紧急任务置顶 (Sort logic: Emergency first)
    // 注意：我们不改变 gameState.tasks 的实际顺序（因为 index 用于其他逻辑），
    // 而是建立一个索引映射列表来渲染。
    // 但是这里为了简化 logic，我们简单地按顺序渲染，前提是 tasks 列表已经被正确排序或插入。
    // 为了满足“紧急任务置顶”且不破坏 index 逻辑 (TaskItemWidget 用 index 显示 "任务X")，
    // 我们最好是在插入任务时就保证顺序，或者在这里仅仅是视觉上的置顶。
    // 但 TaskItemWidget 构造函数用了 `i` 作为任务编号。
    // 如果我们改变了渲染顺序，`i` 就不再对应 `gameState.tasks` 的索引了。
    // 让我们先把紧急任务排在前面吧。

    QList<int> sortedIndices;
    // 1. Emergency
    for(int i=0; i<gameState.tasks.size(); ++i) {
        if(gameState.tasks[i].isEmergency) sortedIndices.append(i);
    }
    // 2. Normal
    for(int i=0; i<gameState.tasks.size(); ++i) {
        if(!gameState.tasks[i].isEmergency) sortedIndices.append(i);
    }

    for (int i : sortedIndices)
    {
        // 创建任务项 Widget
        TaskItemWidget *widget = new TaskItemWidget(i, &gameState.tasks[i]);

        // 设置初始状态
        bool isExpanded = (i == gameState.expandedTaskIndex);
        widget->setExpanded(isExpanded);

        QListWidgetItem *item = new QListWidgetItem(taskListWidget);
        item->setSizeHint(widget->sizeHint()); // 重要：设置 Item 大小以适应 Widget
        taskListWidget->addItem(item);
        taskListWidget->setItemWidget(item, widget);

        // 连接点击信号
        connect(widget, &TaskItemWidget::headerClicked, [this, i](){
            // 更新展开索引
            if (gameState.expandedTaskIndex == i) {
                gameState.expandedTaskIndex = -1; // Toggle off
            } else {
                gameState.expandedTaskIndex = i;
            }

            // 原地更新列表项状态，避免重建导致崩溃和滚动丢失
            for (int j = 0; j < taskListWidget->count(); ++j) {
                QListWidgetItem *it = taskListWidget->item(j);
                TaskItemWidget *w = qobject_cast<TaskItemWidget*>(taskListWidget->itemWidget(it));
                if (w) {
                    // 修复Bug: 使用任务自身的ID (w->getTaskIndex()) 而不是视觉索引 j 来判断是否展开
                    bool expand = (w->getTaskIndex() == gameState.expandedTaskIndex);
                    w->setExpanded(expand);
                    it->setSizeHint(w->sizeHint()); // 更新尺寸提示
                }
            }
        });
    }
}

// --- 场景渲染 (Scene Rendering) ---

void Test3::renderScene()
{
    // emit logMessage(QString("renderScene: %1").arg((int)gameState.currentScene)); // Filtered per request
    QList<QObject *> children = rpgCenterPanel->children();
    for (QObject *child : children)
    {
        if (child == hoverHintLabel)
            continue;
        if (child->isWidgetType())
            static_cast<QWidget *>(child)->hide();
        child->deleteLater();
    }

    QVBoxLayout *rightLayout = qobject_cast<QVBoxLayout *>(inventoryListWidget->parentWidget()->layout());
    if (gameState.currentScene == GameScene::ElevatorInside)
    {
        inventoryListWidget->hide();
        inventoryTitleLabel->parentWidget()->hide(); // Hide the container (Title + Count)
        elevatorPanelContainer->show();

        if (rightLayout)
        {
            rightLayout->setStretchFactor(taskListWidget, 5);
            rightLayout->setStretchFactor(elevatorPanelContainer, 5);
        }
    }
    else
    {
        elevatorPanelContainer->hide();
        inventoryTitleLabel->parentWidget()->show();
        inventoryListWidget->show();

        if (rightLayout)
        {
            rightLayout->setStretchFactor(taskListWidget, 5);
            rightLayout->setStretchFactor(inventoryListWidget, 5);
        }
    }

    // 渲染背景和物体
    switch (gameState.currentScene)
    {
    case GameScene::Entrance: renderEntrance(); tryShowTip(GameScene::Entrance); break;
    case GameScene::StaffHallway: renderStaffHallway(); break;
    case GameScene::Office: renderOffice(); break;
    case GameScene::Warehouse: renderWarehouse(); tryShowTip(GameScene::Warehouse); break;
    case GameScene::WarehouseShelf: renderWarehouseShelf(); tryShowTip(GameScene::WarehouseShelf); break;
    case GameScene::ElevatorHall: renderElevatorHall(); break;
    case GameScene::ElevatorInside: renderElevatorInside(); break;
    case GameScene::FloorCorridor: renderFloorCorridor(); break;
    case GameScene::LinenRoom: renderLinenRoom(); tryShowTip(GameScene::LinenRoom); break;
    }
    updateRPGStatusLabels();
    installDevFilter(rpgCenterPanel);
    if (hoverHintLabel)
        hoverHintLabel->raise();

    // Force layout update to apply scaling immediately
    updateGameLayout();
}

void Test3::tryShowTip(GameScene scene)
{
    // Video Tutorial Check
    if (scene == GameScene::Warehouse && !gameState.hasShownTipWarehouse) {
        VideoTutorialDialog *dlg = new VideoTutorialDialog(":/source/Test3/脏布草.mp4", Config::Test3::Texts::TUTORIAL_WAREHOUSE_ENTRY, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
        gameState.hasShownTipWarehouse = true;
        return;
    }

    if (scene == GameScene::WarehouseShelf && !gameState.hasShownTipShelf) {
        VideoTutorialDialog *dlg = new VideoTutorialDialog(":/source/Test3/拿布草.mp4", Config::Test3::Texts::TUTORIAL_WAREHOUSE_SHELF_ACTION, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
        gameState.hasShownTipShelf = true;
        return;
    }

    if (scene == GameScene::LinenRoom && !gameState.hasShownTipLinenRoom) {
        VideoTutorialDialog *dlg = new VideoTutorialDialog(":/source/Test3/放布草.mp4", Config::Test3::Texts::TUTORIAL_SHELF, this);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->exec();
        gameState.hasShownTipLinenRoom = true;
        return;
    }

    // Default Fallback (SpeechBubble) for Entrance
    QString tipText;
    bool *flag = nullptr;

    switch (scene) {
        case GameScene::Entrance:
            if (!gameState.hasShownTipEntrance) {
                tipText = Config::Test3::Texts::TUTORIAL_GENERAL;
                flag = &gameState.hasShownTipEntrance;
            }
            break;
        default: break;
    }

    if (flag && *flag == false) {
        // 显示气泡
        SpeechBubble *bubble = new SpeechBubble(tipText, rpgCenterPanel);
        bubble->show();
        *flag = true;
    }
}

// Helper for ArrowButton creation to inject configs
ArrowButton* Test3::createArrow(QWidget* parent, const QRect &rect, int angle, const QString &text, int fontSize) {
    ArrowButton *btn = new ArrowButton(parent);
    setGeometryCentered(btn, rect);
    btn->setProperty("originalFontSize", fontSize); // Used for scaling
    btn->setAngle(angle);
    btn->setArrowText(""); // Hide default center text
    btn->setArrowTextSize(fontSize);
    btn->setColor(QColor(Config::Text::COLOR_TEST3_ARROW_TEXT));
    return btn;
}

void Test3::renderEntrance()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ENTRANCE);
    if (pix.isNull())
        pix = generatePlaceholder("酒店入口", Qt::darkGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true); // 允许背景随窗口缩放
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    ArrowButton *btnHome = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_ENTRANCE_HOME, Config::Test3::Geometry::ANGLE_BTN_ENTRANCE_HOME, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(btnHome, &QPushButton::clicked, this, &Test3::handleGoHome);
    connect(btnHome, &ArrowButton::hovered, [this](bool status, QString text)
            {
        QString tip = Config::Test3::Texts::TEXT_BTN_ENTRANCE_HOME;
        if (status) { hoverHintLabel->setText(tip); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnHome->show();

    ClickableArea *btnEnter = new ClickableArea(rpgCenterPanel);
    btnEnter->setPolygon(Config::Test3::Geometry::POLY_ENTRANCE_ENTER());
    btnEnter->setToolTip(Config::Test3::Texts::BTN_ENTER_HOTEL);
    connect(btnEnter, &ClickableArea::clicked, [this]()
            { goToScene(GameScene::StaffHallway); });
    connect(btnEnter, &ClickableArea::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnEnter->setGeometry(rpgCenterPanel->rect());
    btnEnter->show();
}

void Test3::renderStaffHallway()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QString bgPath = Config::Test3::Images::SCENE_HALLWAY_NORMAL;
    bool anyTaskDone = false;
    for (const auto &t : gameState.tasks) if (t.isCompleted) anyTaskDone = true;
    if (anyTaskDone) bgPath = Config::Test3::Images::SCENE_HALLWAY_CLOCKED_OUT;
    else if (isLate) bgPath = Config::Test3::Images::SCENE_HALLWAY_LATE;

    QPixmap pix = getPixmap(bgPath);
    if (pix.isNull()) pix = generatePlaceholder("员工通道", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    ArrowButton *exitBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_HALLWAY_EXIT, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_EXIT, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(exitBtn, &QPushButton::clicked, [this]() { goToScene(GameScene::Entrance); });
    connect(exitBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_ENTRANCE); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    exitBtn->show();

    ClickableArea *btnClock = new ClickableArea(rpgCenterPanel);
    btnClock->setPolygon(Config::Test3::Geometry::POLY_HALLWAY_CLOCK());
    QString clockText = gameState.hasClockedIn ? Config::Test3::Texts::BTN_CLOCK_OUT : Config::Test3::Texts::BTN_CLOCK_IN;
    btnClock->setToolTip(clockText);
    connect(btnClock, &ClickableArea::clicked, [this]()
            {
        if (gameState.hasClockedIn) handleClockOut();
        else handleClockIn(); });
    connect(btnClock, &ClickableArea::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    btnClock->setGeometry(rpgCenterPanel->rect());
    btnClock->show();

    auto createArrowHelper = [&](const QRect &rect, int angle, QString text, auto func)
    {
        ArrowButton *btn = createArrow(rpgCenterPanel, rect, angle, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
        connect(btn, &QPushButton::clicked, func);
        connect(btn, &ArrowButton::hovered, [this, text](bool status, QString t)
                {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide(); });
        btn->show();
    };

    createArrowHelper(Config::Test3::Geometry::RECT_BTN_HALLWAY_OFFICE, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_OFFICE, Config::Test3::Texts::BTN_GO_OFFICE, [this](){ goToScene(GameScene::Office); });
    createArrowHelper(Config::Test3::Geometry::RECT_BTN_HALLWAY_WAREHOUSE, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_WAREHOUSE, Config::Test3::Texts::BTN_GO_WAREHOUSE, [this](){ goToScene(GameScene::Warehouse); });
    createArrowHelper(Config::Test3::Geometry::RECT_BTN_HALLWAY_ELEVATOR, Config::Test3::Geometry::ANGLE_BTN_HALLWAY_ELEVATOR, Config::Test3::Texts::BTN_GO_ELEVATOR, [this](){ goToScene(GameScene::ElevatorHall); });
}

void Test3::renderWarehouse()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_WAREHOUSE_ENTRY);
    if (pix.isNull()) pix = generatePlaceholder("仓库 (入口)", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 脏布草回收区域 (Polygon)
    ClickableArea *binArea = new ClickableArea(rpgCenterPanel);
    binArea->setPolygon(Config::Test3::Geometry::POLY_WAREHOUSE_BIN());
    binArea->setToolTip(Config::Test3::Texts::LBL_DIRTY_BIN_TIP);
    binArea->setGeometry(rpgCenterPanel->rect());

    binArea->onDropCallback = [this](QString item) {
        if (item == "DirtyLinen" || item == "脏布草") {
            gameState.inventory.dirtyItemsCount = 0;
            emit logMessage("脏布草已回收");
            refreshInventoryList();
            Utils::ShowCustomMessageBox(this, "提示", "脏布草已回收。");
        } else {
            // 回收干净布草
            int count = gameState.inventory.cleanItems[item];
            if (count > 0) {
                 gameState.inventory.cleanItems[item] = 0;
                 emit logMessage("回收(丢弃): " + item);
                 refreshInventoryList();
                 Utils::ShowCustomMessageBox(this, "提示", QString("已将 %1 放入回收处理。").arg(item));
            }
        }
    };
    connect(binArea, &ClickableArea::hovered, [this](bool status, QString text) {
        if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide();
    });
    binArea->show();

    QPushButton *takeBtn = new QPushButton(Config::Test3::Texts::BTN_TAKE_LINEN, rpgCenterPanel);
    setGeometryCentered(takeBtn, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_TAKE);
    takeBtn->setProperty("originalFontSize", Config::Text::SIZE_TEST3_ACTION_BTN);
    // 使用统一蓝色样式 (原圆环样式移除，用户要求统一)
    takeBtn->setStyleSheet(Config::Test3::Styles::GET_STYLE_BTN_UNIFIED());
    takeBtn->setCursor(Qt::PointingHandCursor);
    connect(takeBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::WarehouseShelf); });
    takeBtn->show();

    ArrowButton *backBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_WAREHOUSE_BACK, Config::Test3::Geometry::ANGLE_BTN_WAREHOUSE_BACK, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderWarehouseShelf()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_WAREHOUSE_SHELF);
    if (pix.isNull()) pix = generatePlaceholder("货架", Qt::darkYellow, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    auto createShelfArea = [&](const QString &name, const QRect &rect, const QPoint &lblPos)
    {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true);
        area->setSourceType("WarehouseShelf");
        area->setToolTip(name + " (可拿取)");
        connect(area, &ShelfArea::hovered, [this](bool status, QString text)
                {
            if (status) { hoverHintLabel->setText(text); hoverHintLabel->show(); hoverHintLabel->raise(); }
            else hoverHintLabel->hide(); });

        QString iconPath = Config::Test3::Images::ITEMS().value(name);
        if (QFile::exists(iconPath))
        {
            QPixmap pix = getPixmap(iconPath);
            if (!pix.isNull())
            {
                area->setPixmap(pix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                area->setAlignment(Qt::AlignCenter);
            }
        }

        // 标签显示
        QLabel *lbl = new QLabel(name, rpgCenterPanel);
        int fontSize = Config::Text::SIZE_TEST3_SHELF_LABEL;
        QString lblStyle = QString("color: %1; font-weight: bold; background-color: rgba(0,0,0,0.5); padding: 2px; border-radius: 4px; font-size: %2px;")
                .arg(Config::Text::COLOR_TEST3_SHELF_LABEL)
                .arg(fontSize);

        lbl->setProperty("originalFontSize", fontSize);
        lbl->setStyleSheet(lblStyle);
        lbl->adjustSize();

        // 设置原始几何 (使用中心点和初始宽高)
        // 注意：lblPos 是中心点，lbl->width/height 是 adjustSize 后的
        // 我们需要记录中心点和初始尺寸，以便缩放
        // lbl->move 逻辑是: left = cx - w/2
        // setGeometryCentered 需要 rect(cx, cy, w, h)
        // 我们用当前大小作为初始大小
        setGeometryCentered(lbl, lblPos.x(), lblPos.y(), lbl->width(), lbl->height());
        lbl->show();

        area->onDropCallback = [this, name](QString item)
        {
            if (item != name)
            {
                // Warning/Error stays as standard critical/warning often, but for consistency can change
                // Standard critical is usually fine, but user wanted "similar popups".
                // I will use CustomMessageBox with warning flag.
                Utils::ShowCustomMessageBox(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name), true);
                return;
            }
            handleSceneDrop(item, true);
        };
        area->show();
    };

    createShelfArea("大床单", Config::Test3::Geometry::AREA_SHEET, Config::Test3::Geometry::POS_LBL_SHEET);
    createShelfArea("大被套", Config::Test3::Geometry::AREA_DUVET, Config::Test3::Geometry::POS_LBL_DUVET);
    createShelfArea("小被套", Config::Test3::Geometry::AREA_S_DUVET, Config::Test3::Geometry::POS_LBL_S_DUVET);
    createShelfArea("枕巾", Config::Test3::Geometry::AREA_PILLOW, Config::Test3::Geometry::POS_LBL_PILLOW);
    createShelfArea("晚安巾", Config::Test3::Geometry::AREA_GN_TOWEL, Config::Test3::Geometry::POS_LBL_GN_TOWEL);
    createShelfArea("毛巾", Config::Test3::Geometry::AREA_TOWEL, Config::Test3::Geometry::POS_LBL_TOWEL);

    ArrowButton *backBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_SHELF_BACK, Config::Test3::Geometry::ANGLE_BTN_SHELF_BACK, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::Warehouse); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_WAREHOUSE_ENTRY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::handleInventoryDrop(QString itemName, const QMimeData *mimeData)
{
    // 推车限制检查
    int totalItems = 0;
    for (auto c : gameState.inventory.cleanItems) totalItems += c;
    totalItems += gameState.inventory.dirtyItemsCount;

    if (itemName == "脏布草" || itemName == "DirtyLinen")
    {
        if (totalItems + 1 > Config::Test3::Logic::MAX_CART_ITEMS) {
            Utils::ShowCustomMessageBox(this, "推车已满", "推车已满，无法再装入物品！(上限40件)", true);
            return;
        }

        // 混装检查: 如果车里有干净布草，则记录错误
        int cleanCount = 0;
        for (auto c : gameState.inventory.cleanItems) cleanCount += c;
        if (cleanCount > 0) errorLog.mixedLinen = true;

        // 处理来源：从地板 (LinenRoomDirty) 还是其他
        // 验证拖拽来源 (Source) 必须是 "LinenRoomDirty" 才能减少 floorInventory
        // 防止从推车自身拖拽导致无限刷物品
        bool fromFloor = false;
        if (mimeData && mimeData->hasFormat("application/x-source")) {
            QString source = QString::fromUtf8(mimeData->data("application/x-source"));
            if (source == "LinenRoomDirty") {
                fromFloor = true;
            }
        }

        // 如果是从布草间生成的脏布草
        if (fromFloor && gameState.currentScene == GameScene::LinenRoom &&
            gameState.floorInventory[gameState.currentFloor].value("DirtyLinen", 0) > 0) {
             gameState.floorInventory[gameState.currentFloor]["DirtyLinen"]--;
             gameState.inventory.dirtyItemsCount++;
             emit logMessage("回收脏布草");
        }
        else if (!fromFloor) {
             // ... Logic preserved ...
        }

        renderScene();
    }
    else
    {
        if (totalItems + 1 > Config::Test3::Logic::MAX_CART_ITEMS) {
            Utils::ShowCustomMessageBox(this, "推车已满", "推车已满，无法再装入物品！(上限40件)", true);
            return;
        }

        // 混装检查: 脏布草在车上时装干净布草
        if (gameState.inventory.dirtyItemsCount > 0) errorLog.mixedLinen = true;

        if (gameState.currentScene == GameScene::LinenRoom)
        {
            if (gameState.floorInventory[gameState.currentFloor].value(itemName, 0) > 0)
            {
                gameState.floorInventory[gameState.currentFloor][itemName]--;
                emit logMessage("从布草间货架取回: " + itemName);
            }
        }

        gameState.inventory.cleanItems[itemName]++;
        emit logMessage("装车: " + itemName);

        if (gameState.currentScene == GameScene::LinenRoom) renderScene();
    }
    refreshInventoryList();
}

void Test3::handleReportWork()
{
    QStringList feedback;
    bool perfect = true;

    // 1. Check Emergency Priority First
    for (const auto &t : gameState.tasks) {
        if (t.isEmergency) {
            bool isMet = true;
            for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
                if (gameState.floorInventory[t.targetFloor].value(it.key(), 0) < it.value()) {
                    isMet = false; break;
                }
            }
            if (!isMet) {
                feedback << "【严重】紧急任务未完成！经理非常生气。";
                perfect = false;
                errorLog.missedEmergencyPriority = true;
            }
        }
    }

    // 2. Check All Tasks
    QStringList incompleteDetails;
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        const Task &t = gameState.tasks[i];
        bool isMet = true;
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
             if (gameState.floorInventory[t.targetFloor].value(it.key(), 0) < it.value()) {
                 isMet = false; break;
             }
        }

        if (!isMet) {
            perfect = false;
            QString taskName = t.isEmergency ? "紧急任务" : QString("任务%1").arg(i+1);
            if (t.isMarkedComplete) {
                incompleteDetails << QString("%1(%2楼) 虚假标记完成").arg(taskName).arg(t.targetFloor);
            } else {
                incompleteDetails << QString("%1(%2楼) 未完成").arg(taskName).arg(t.targetFloor);
            }
        }
    }

    if (!incompleteDetails.isEmpty()) {
        feedback << "任务完成情况不佳：";
        feedback.append(incompleteDetails);
    } else if (perfect) {
        feedback << "所有任务均已达标。";
    }

    // 3. Other Errors
    if (errorLog.lateClockIn) {
        feedback << "考勤问题：上班迟到。";
        perfect = false;
    }
    if (errorLog.mixedLinen) {
        feedback << "操作失误：曾出现脏净布草混装。";
        perfect = false;
    }

    // Final Message Construction
    QString title = "汇报结果 (经理反馈)";
    QString finalMsg;

    if (perfect) {
        finalMsg = "经理：\n\n“干得漂亮！所有任务都完美完成了，流程也很规范。继续保持！”";
    } else {
        finalMsg = "经理：\n\n“你今天的工作有一些问题：\n";
        for (const QString &s : feedback) {
            finalMsg += "- " + s + "\n";
        }
        finalMsg += "\n下次注意改进。”";
    }

    Utils::ShowCustomMessageBox(this, title, finalMsg);
    gameState.hasReported = true;
    emit logMessage("已汇报工作: " + finalMsg.replace("\n", " "));
    renderScene();
}

void Test3::handleGetTask()
{
    if (gameState.hasReceivedTask) {
        Utils::ShowCustomMessageBox(this, "提示", "您已经领取过任务了，请查看右侧任务列表。");
        return;
    }

    gameState.hasReceivedTask = true;

    // 分配基础任务 (6楼 和 7楼) - 随机生成
    auto generateRandomReqs = [this]() -> QMap<QString, int> {
        QMap<QString, int> reqs;
        QStringList allItems = Config::Test3::Images::ITEMS().keys();
        int typeCount = getNormalRandom(4, 6);
        while (reqs.size() < typeCount && !allItems.isEmpty()) {
            int idx = QRandomGenerator::global()->bounded(allItems.size());
            QString item = allItems.takeAt(idx);
            int count = getNormalRandom(1, 6);
            reqs.insert(item, count);
        }
        return reqs;
    };

    Task t1;
    t1.targetFloor = Config::Test3::Logic::TASK_FIXED_FLOOR_1; // 6
    t1.requiredItems = generateRandomReqs();
    t1.isEmergency = false;
    gameState.tasks.append(t1);

    Task t2;
    t2.targetFloor = Config::Test3::Logic::TASK_FIXED_FLOOR_2; // 7
    t2.requiredItems = generateRandomReqs();
    t2.isEmergency = false;
    gameState.tasks.append(t2);

    refreshTaskList();
    emit logMessage("任务已下发: 6楼, 7楼");
    Utils::ShowCustomMessageBox(this, "提示", "任务已下发，请查看右侧上方任务列表。\n 点击任务可展开具体需求");
    renderScene();

    // 只有在勾选了紧急事件开关时才触发
    if (isEmergencyEnabled)
    {
        bool isEventA = (QRandomGenerator::global()->bounded(2) == 0);

        if (isEventA)
        {
            // 事件A: 延时触发
            int delay = getNormalRandom(20000, 40000);
            emergencyTimer->start(delay);
            emit logMessage(QString("计划触发事件A: 紧急任务将于 %1 秒后触发").arg(delay / 1000));
        }
        else
        {
            // 事件B: 立即触发 (脏布草)
            QVector<int> baseFloors;
            baseFloors << Config::Test3::Logic::TASK_FIXED_FLOOR_1 << Config::Test3::Logic::TASK_FIXED_FLOOR_2;
            int targetFloor = baseFloors[QRandomGenerator::global()->bounded(baseFloors.size())];

            int dirtyCount = getNormalRandom(1, 5);
            gameState.floorInventory[targetFloor]["DirtyLinen"] += dirtyCount;

            emit logMessage(QString("触发事件B (立即): %1楼 生成 %2 件脏布草 (无弹窗)").arg(targetFloor).arg(dirtyCount));
        }
    }
}

void Test3::checkEmergencyTask()
{
    // 事件A: 紧急补货任务 (由定时器触发)
    int targetFloor = getNormalRandom(2, 10);
    // 避免选中1楼(跳过)或0楼
    if (targetFloor == 1) targetFloor = 2;

    Task t;
    t.targetFloor = targetFloor;
    t.isEmergency = true;
    t.requiredItems = {{"大床单", 1}, {"毛巾", 2}}; // 简单紧急需求
    gameState.tasks.append(t);

    isEmergencyActive = true;
    refreshTaskList();

    // 弹出经理提示 (增加换行)
    QString msg = QString(Config::Test3::Texts::POPUP_EMERGENCY_MANAGER).arg(targetFloor);
    Utils::ShowCustomMessageBox(this, "突发事件 (经理)", msg, true); // true = warning style
    emit logMessage("触发事件A: 紧急任务 " + QString::number(targetFloor) + "楼");
}

int Test3::getNormalRandom(int min, int max)
{
    return QRandomGenerator::global()->bounded(min, max + 1);
}

// --- 辅助函数 ---

QPixmap Test3::generatePlaceholder(QString text, QColor color, QSize size)
{
    QPixmap pix(size);
    pix.fill(color);
    QPainter p(&pix);
    p.setPen(Qt::white);
    p.drawText(pix.rect(), Qt::AlignCenter, text);
    return pix;
}

void Test3::showTutorial()
{
    gameState.hasShownTipEntrance = false;
    tryShowTip(GameScene::Entrance);
}

void Test3::handleClockIn()
{
    if (gameState.hasClockedIn) return;
    gameState.hasClockedIn = true;
    gameState.hasEverClockedIn = true;
    emit logMessage("上班打卡成功");
    Utils::ShowCustomMessageBox(this, "提示", "上班打卡成功！请前往办公室领取任务。");
    renderScene();
}

void Test3::handleClockOut()
{
    gameState.hasClockedIn = false;
    emit logMessage("下班打卡");
    Utils::ShowCustomMessageBox(this, "提示", "已打卡下班。");
    renderScene();
}

void Test3::handleGoHome()
{
    QStringList report;
    if (!gameState.hasReported) {
        errorLog.noReportBeforeHome = true;
        report << "错误: 下班前未汇报工作";
    }

    if (gameState.hasClockedIn) {
        errorLog.noClockOutBeforeHome = true;
        report << "错误: 下班前未打卡";
    }

    // 记录结果日志而非弹窗
    if (errorLog.hasErrors()) {
         QString msg = "本次实训存在以下问题: " + report.join("; ");
         emit logMessage("实训结束 (有瑕疵): " + msg);
    } else {
         emit logMessage("实训结束: 恭喜！流程规范，完美下班。");
    }

    // --- 更新日志数据 ---
    Logger::Test3BriefData &data = Logger::instance().test3Data;

    // 考勤状态判断
    data.isLate = errorLog.lateClockIn;

    // 上班打卡状态
    if (!gameState.hasEverClockedIn) {
        data.clockInStatus = "忘记上班打卡";
    } else if (errorLog.lateClockIn) {
        data.clockInStatus = "迟到";
    } else {
        data.clockInStatus = "正常";
    }

    // 下班打卡状态
    if (errorLog.noClockOutBeforeHome) {
        data.clockOutStatus = "忘记下班打卡";
    } else {
        data.clockOutStatus = "正常";
    }

    // Emergency & Mixed Linen
    data.emergencyPriorityMet = !errorLog.missedEmergencyPriority;
    data.mixedLinen = errorLog.mixedLinen;

    // Time Used
    qint64 secs = m_startTime.secsTo(QDateTime::currentDateTime());
    int mins = secs / 60;
    int s = secs % 60;
    data.timeUsed = QString("%1分%2秒").arg(mins).arg(s);

    // Task List (Detailed Table Logic)
    data.detailedTasks.clear();
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        const Task &t = gameState.tasks[i];
        Logger::Test3BriefData::TaskLog taskLog;
        taskLog.floor = t.targetFloor;
        taskLog.isEmergency = t.isEmergency;

        for(auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
            if(it.value() > 0) {
                QString name = it.key();
                int required = it.value();
                int actual = gameState.floorInventory[t.targetFloor].value(name, 0);
                bool marked = t.itemCompletionStatus.value(name, false);

                Logger::Test3BriefData::TaskItemLog itemLog;
                itemLog.name = name;
                itemLog.required = required;
                itemLog.isMarked = marked;

                // Determine Result Status
                if (actual >= required) {
                    if (marked) {
                        itemLog.resultStatus = "完成";
                    } else {
                        itemLog.resultStatus = "完成未标记";
                    }
                } else {
                    itemLog.resultStatus = "未完成";
                }

                taskLog.items.append(itemLog);
            }
        }
        data.detailedTasks.append(taskLog);
    }

    // Also format simple string just in case (optional, but requested detailed table so string might be unused)
    data.taskList = formatTaskList();

    // Floor Statuses (Task Completion)
    data.floorStatuses.clear();
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        const Task &t = gameState.tasks[i];
        Logger::Test3BriefData::FloorStatus fs;
        fs.floor = t.targetFloor;

        // Check completion logic again locally or trust some flag?
        // Let's re-verify completion against inventory
        bool isMet = true;
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
             if (gameState.floorInventory[t.targetFloor].value(it.key(), 0) < it.value()) {
                 isMet = false; break;
             }
        }

        fs.isCorrect = isMet;
        if (t.isEmergency) fs.details = "紧急任务";
        else fs.details = "普通任务";

        data.floorStatuses.append(fs);
    }

    // 重置状态以备下一次开始
    reset();

    // 退出到主菜单 (使用 levelCancelled 返回菜单，不触发 MainWindow 的成功弹窗)
    emit levelCancelled();
}

QString Test3::formatTaskList() const
{
    QStringList result;
    for (int i = 0; i < gameState.tasks.size(); ++i) {
        const Task &t = gameState.tasks[i];
        QString header = QString("[%1] %2层: ").arg(t.isEmergency ? "紧急" : "任务").arg(t.targetFloor);
        QStringList items;
        for (auto it = t.requiredItems.begin(); it != t.requiredItems.end(); ++it) {
            if (it.value() > 0) {
                items << QString("%1x%2").arg(it.key()).arg(it.value());
            }
        }
        result << (header + items.join(", "));
    }
    return result.join("; ");
}

void Test3::handleElevatorButton(int floor)
{
    if (floor == 1) return; // 1楼不停
    gameState.currentFloor = floor;
    emit logMessage(QString("电梯到达: %1层").arg(floor));
    goToScene(GameScene::ElevatorHall);
}

void Test3::handleSceneDrop(QString itemName, bool isWarehouse)
{
    int count = gameState.inventory.cleanItems.value(itemName);
    if (count <= 0) return;

    // Requirement: "Everything dragged in" (Move all items of this type)
    // 从推车移除全部
    gameState.inventory.cleanItems[itemName] = 0;

    if (isWarehouse) {
        emit logMessage(QString("归还物品到仓库: %1 (数量: %2)").arg(itemName).arg(count));
    } else {
        // 布草间
        gameState.floorInventory[gameState.currentFloor][itemName] += count;
        emit logMessage(QString("放置物品到 %1楼 布草间: %2 (数量: %3)").arg(gameState.currentFloor).arg(itemName).arg(count));
    }

    refreshInventoryList();
    renderScene();
}

void Test3::showTaskSheet(int taskIndex)
{
    // 此函数保留，但按钮入口已移除，防止逻辑报错。
    // 如果需要弹窗查看，可以在TaskList双击或其他方式触发。
    // 目前根据需求已移除申领表按钮。
}

void Test3::renderOffice()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_OFFICE);
    if (pix.isNull()) pix = generatePlaceholder("办公室", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 动作按钮 (领任务 / 汇报)
    QPushButton *actionBtn = new QPushButton(gameState.hasReported ? Config::Test3::Texts::LBL_WORK_REPORTED :
                                            (gameState.hasReceivedTask ? Config::Test3::Texts::BTN_REPORT_WORK : Config::Test3::Texts::BTN_GET_TASK), rpgCenterPanel);
    setGeometryCentered(actionBtn, Config::Test3::Geometry::RECT_BTN_OFFICE_ACTION);
    actionBtn->setProperty("originalFontSize", Config::Text::SIZE_TEST3_ACTION_BTN);
    actionBtn->setStyleSheet(Config::Test3::Styles::GET_STYLE_BTN_UNIFIED());
    actionBtn->setCursor(Qt::PointingHandCursor);
    if (gameState.hasReported) actionBtn->setEnabled(false);

    connect(actionBtn, &QPushButton::clicked, [this]() {
        if (!gameState.hasReceivedTask) {
            handleGetTask();
        } else {
            handleReportWork();
        }
    });
    actionBtn->show();

    // Back Button
    ArrowButton *backBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_OFFICE_BACK, Config::Test3::Geometry::ANGLE_BTN_OFFICE_BACK, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text)
            {
        if (status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); }
        else hoverHintLabel->hide(); });
    backBtn->show();
}

void Test3::renderElevatorHall()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_HALL);
    if (pix.isNull()) pix = generatePlaceholder("电梯厅", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 进入电梯按钮
    QPushButton *enterBtn = new QPushButton(Config::Test3::Texts::BTN_ENTER_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(enterBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_ENTER);
    enterBtn->setProperty("originalFontSize", Config::Text::SIZE_TEST3_ACTION_BTN);
    enterBtn->setStyleSheet(Config::Test3::Styles::GET_STYLE_BTN_UNIFIED());
    enterBtn->setCursor(Qt::PointingHandCursor);
    connect(enterBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::ElevatorInside); });
    enterBtn->show();

    ArrowButton *backBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_ELEVATOR_BACK, Config::Test3::Geometry::ANGLE_BTN_ELEVATOR_BACK, "", Config::Text::SIZE_TEST3_ARROW_TEXT);

    if (gameState.currentFloor == 0) {
        connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::StaffHallway); });
        connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_HALLWAY); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
        });
    } else {
        connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::FloorCorridor); });
        connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_CORRIDOR); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
        });
    }
    backBtn->show();
}

void Test3::renderElevatorInside()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_ELEVATOR_INSIDE);
    if (pix.isNull()) pix = generatePlaceholder("电梯内部", Qt::gray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 出电梯按钮
    QPushButton *exitBtn = new QPushButton(Config::Test3::Texts::BTN_EXIT_ELEVATOR, rpgCenterPanel);
    setGeometryCentered(exitBtn, Config::Test3::Geometry::RECT_BTN_ELEVATOR_EXIT);
    exitBtn->setProperty("originalFontSize", Config::Text::SIZE_TEST3_ACTION_BTN);
    exitBtn->setStyleSheet(Config::Test3::Styles::GET_STYLE_BTN_UNIFIED());
    exitBtn->setCursor(Qt::PointingHandCursor);
    connect(exitBtn, &QPushButton::clicked, [this](){
        goToScene(GameScene::ElevatorHall);
    });
    exitBtn->show();
}

void Test3::renderFloorCorridor()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_FLOOR_CORRIDOR);
    if (pix.isNull()) pix = generatePlaceholder("楼层走廊", Qt::lightGray, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    // 去布草间
    ArrowButton *linenBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_CORRIDOR_LINEN, Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_LINEN, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(linenBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::LinenRoom); });
    connect(linenBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_LINEN_ROOM); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    linenBtn->show();

    // 去电梯厅
    ArrowButton *eleBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_CORRIDOR_ELEVATOR, Config::Test3::Geometry::ANGLE_BTN_CORRIDOR_ELEVATOR, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(eleBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::ElevatorHall); });
    connect(eleBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_GO_ELEVATOR_HALL); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    eleBtn->show();
}

void Test3::renderLinenRoom()
{
    QLabel *bg = new QLabel(rpgCenterPanel);
    QPixmap pix = getPixmap(Config::Test3::Images::SCENE_LINEN_ROOM_EMPTY);
    if (pix.isNull()) pix = generatePlaceholder("布草间", Qt::white, rpgCenterPanel->size());
    bg->setPixmap(pix.scaled(rpgCenterPanel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bg->setScaledContents(true);
    setGeometryCentered(bg, Config::Test3::Geometry::CENTER_PANEL_SIZE.width() / 2, Config::Test3::Geometry::CENTER_PANEL_SIZE.height() / 2,
                        Config::Test3::Geometry::CENTER_PANEL_SIZE.width(), Config::Test3::Geometry::CENTER_PANEL_SIZE.height());
    bg->show();

    auto createLinenShelf = [&](const QString &name, const QRect &rect) {
        ShelfArea *area = new ShelfArea(name, rpgCenterPanel);
        setGeometryCentered(area, rect);
        area->setStyleSheet(Config::Test3::Styles::SHELF_AREA);
        area->setDraggable(true);
        area->setSourceType("LinenRoomShelf");

        int count = gameState.floorInventory[gameState.currentFloor].value(name, 0);

        if (count > 0) {
            QString iconPath = Config::Test3::Images::ITEMS().value(name);
            if (QFile::exists(iconPath)) {
                QPixmap p = getPixmap(iconPath);
                area->setPixmap(p.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                area->setAlignment(Qt::AlignCenter);
            }

            // 数量标签 (Count Label)
            QLabel *cntLbl = new QLabel(QString("(X%1)").arg(count), rpgCenterPanel);

            int fontSize = Config::Text::SIZE_TEST3_COUNT_LABEL;
            QString style = QString("color: %1; font-size: %2px; font-weight: bold; background-color: rgba(0,0,0,0.6); padding: 2px; border-radius: 4px;")
                    .arg(Config::Text::COLOR_TEST3_COUNT_LABEL)
                    .arg(fontSize);

            cntLbl->setStyleSheet(style);
            cntLbl->adjustSize();

            int realLeft = rect.x() - rect.width() / 2;
            int realTop = rect.y() - rect.height() / 2;
            int targetX = realLeft + 5 + cntLbl->width() / 2;
            int targetY = realTop + 5 + cntLbl->height() / 2;

            setGeometryCentered(cntLbl, targetX, targetY, cntLbl->width(), cntLbl->height());
            cntLbl->setProperty("originalFontSize", fontSize);
            cntLbl->show();
        } else {
            area->setDraggable(false);
        }

        area->onDropCallback = [this, name](QString item) {
             if (item != name) {
                // Critical error
                Utils::ShowCustomMessageBox(this, "错误", QString("存放失败：不能将 %1 放入 %2 的位置！").arg(item, name), true);
                return;
            }
            handleSceneDrop(item, false); // false = LinenRoom
        };

        connect(area, &ShelfArea::hovered, [this, name, count](bool status, QString text) {
             if(status) {
                 QString t = QString("%1 (当前: %2)").arg(name).arg(count);
                 hoverHintLabel->setText(t); hoverHintLabel->show(); hoverHintLabel->raise();
             } else hoverHintLabel->hide();
        });
        area->show();
    };

    createLinenShelf("大床单", Config::Test3::Geometry::AREA_LINEN_SHEET);
    createLinenShelf("大被套", Config::Test3::Geometry::AREA_LINEN_DUVET);
    createLinenShelf("小被套", Config::Test3::Geometry::AREA_LINEN_S_DUVET);
    createLinenShelf("枕巾", Config::Test3::Geometry::AREA_LINEN_PILLOW);
    createLinenShelf("晚安巾", Config::Test3::Geometry::AREA_LINEN_GN_TOWEL);
    createLinenShelf("毛巾", Config::Test3::Geometry::AREA_LINEN_TOWEL);

    // 检查并渲染脏布草 (Event B)
    int dirtyCount = gameState.floorInventory[gameState.currentFloor].value("DirtyLinen", 0);
    if (dirtyCount > 0) {
        ShelfArea *dirtyArea = new ShelfArea("脏布草", rpgCenterPanel);
        setGeometryCentered(dirtyArea, Config::Test3::Geometry::RECT_EVENT_DIRTY_LINEN);
        dirtyArea->setDraggable(true);
        dirtyArea->setSourceType("LinenRoomDirty");

        QPixmap dirtyPix = getPixmap(Config::Test3::Images::UI_DIRTY_LINEN);
        if (!dirtyPix.isNull()) {
             dirtyArea->setPixmap(dirtyPix.scaled(Config::Test3::Geometry::ICON_SHELF_ITEM, Qt::KeepAspectRatio, Qt::SmoothTransformation));
             dirtyArea->setAlignment(Qt::AlignCenter);
        } else {
             dirtyArea->setText("脏布草");
        }

        // Display Count
        if (dirtyCount > 1) {
            QLabel *cntLbl = new QLabel(QString::number(dirtyCount), rpgCenterPanel);

            int fontSize = Config::Text::SIZE_TEST3_COUNT_LABEL;
            QString style = QString("color: %1; font-size: %2px; font-weight: bold;")
                    .arg(Config::Text::COLOR_TEST3_COUNT_LABEL_DIRTY)
                    .arg(fontSize);

            cntLbl->setStyleSheet(style);
            cntLbl->adjustSize();

            QRect dRect = Config::Test3::Geometry::RECT_EVENT_DIRTY_LINEN;
            int realRight = dRect.x() + dRect.width() / 2;
            int realBottom = dRect.y() + dRect.height() / 2;
            int targetX = realRight - cntLbl->width() / 2 - 5;
            int targetY = realBottom - cntLbl->height() / 2 - 5;

            setGeometryCentered(cntLbl, targetX, targetY, cntLbl->width(), cntLbl->height());
            cntLbl->setProperty("originalFontSize", fontSize);
            cntLbl->show();
        }

        dirtyArea->show();
    }

    ArrowButton *backBtn = createArrow(rpgCenterPanel, Config::Test3::Geometry::RECT_BTN_LINEN_BACK, Config::Test3::Geometry::ANGLE_BTN_LINEN_BACK, "", Config::Text::SIZE_TEST3_ARROW_TEXT);
    connect(backBtn, &QPushButton::clicked, [this](){ goToScene(GameScene::FloorCorridor); });
    connect(backBtn, &ArrowButton::hovered, [this](bool status, QString text) {
             if(status) { hoverHintLabel->setText(Config::Test3::Texts::BTN_RETURN_CORRIDOR); hoverHintLabel->show(); hoverHintLabel->raise(); } else hoverHintLabel->hide();
    });
    backBtn->show();
}

#include "test3.moc"
