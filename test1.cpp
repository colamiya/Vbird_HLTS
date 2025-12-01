#include "test1.h"
#include "config.h"
#include <QMessageBox>

Test1::Test1(QWidget *parent) : QWidget(parent)
{
    // 主布局 (网格布局，方便右上角放置按钮)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setAlignment(Qt::AlignCenter);

    // 返回按钮 (右上角)
    QPushButton *returnBtn = new QPushButton(Config::Test1::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test1::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test1::BTN_RETURN_STYLE);
    connect(returnBtn, &QPushButton::clicked, [this]()
            {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认退出", "确定要退出当前测试并返回主菜单吗？\n当前进度将不会保存。",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // 发出取消信号，由 MainWindow 处理页面切换
            emit levelCancelled();
        } });
    // 添加到右上角: 第0行, 第1列, 右对齐 | 顶对齐
    mainGrid->addWidget(returnBtn, 0, 1, Qt::AlignRight | Qt::AlignTop);

    // 内容容器
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 添加内容到第1行, 跨2列
    mainGrid->addWidget(contentWidget, 1, 0, 1, 2);

    // --- 幻灯片区域 ---
    slideshowContainer = new QWidget();
    QVBoxLayout *slideLayout = new QVBoxLayout(slideshowContainer);

    slideImageLabel = new QLabel();
    slideImageLabel->setAlignment(Qt::AlignCenter);
    slideLayout->addWidget(slideImageLabel);

    // 导航按钮
    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton(Config::Test1::BTN_TEXT_PREV);
    QPushButton *nextBtn = new QPushButton(Config::Test1::BTN_TEXT_NEXT);
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(nextBtn);
    slideLayout->addLayout(navLayout);

    connect(prevBtn, &QPushButton::clicked, [this]()
            {
        if (currentSlideIndex > 0) {
            currentSlideIndex--;
            updateSlide();
        } });
    connect(nextBtn, &QPushButton::clicked, [this]()
            {
        if (currentSlideIndex < totalSlides - 1) {
            currentSlideIndex++;
            updateSlide();
        } else {
            finishSlideshow();
        } });

    layout->addWidget(slideshowContainer);

    // --- 总结区域 (初始隐藏) ---
    slideshowSummaryWidget = new QWidget();
    slideshowSummaryWidget->setVisible(false);
    layout->addWidget(slideshowSummaryWidget);

    updateSlide();
}

QPixmap Test1::getPixmap(const QString &path)
{
    // 检查缓存
    if (m_pixmapCache.contains(path))
    {
        return m_pixmapCache.value(path);
    }

    // 加载新图片
    QPixmap pix(path);
    if (!pix.isNull())
    {
        m_pixmapCache.insert(path, pix); // 存入缓存
    }
    return pix;
}

void Test1::updateSlide()
{
    // 检查索引是否在配置列表范围内
    if (currentSlideIndex >= 0 && currentSlideIndex < Config::Test1::SLIDE_IMAGES().size())
    {
        QString imagePath = Config::Test1::SLIDE_IMAGES()[currentSlideIndex];

        // 使用缓存机制获取图片
        QPixmap pixmap = getPixmap(imagePath);

        if (!pixmap.isNull())
        {
            slideImageLabel->setPixmap(pixmap.scaled(Config::Test1::DISPLAY_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            slideImageLabel->setPixmap(generatePlaceholder(
                QString("幻灯片 %1 (缺失: %2)").arg(currentSlideIndex + 1).arg(imagePath),
                Config::Test1::COL_PLACEHOLDER_BG,
                Config::Test1::DISPLAY_SIZE));
        }
    }
    else
    {
        // 索引越界回退
        slideImageLabel->setPixmap(generatePlaceholder("无效的幻灯片索引", Qt::red, Config::Test1::DISPLAY_SIZE));
    }
}

void Test1::finishSlideshow()
{
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    // 创建网格布局显示缩略图
    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    for (int i = 0; i < totalSlides; ++i)
    {
        QLabel *thumb = new QLabel();

        QString imagePath = "";
        if (i < Config::Test1::SLIDE_IMAGES().size())
        {
            imagePath = Config::Test1::SLIDE_IMAGES()[i];
        }

        QPixmap pix = getPixmap(imagePath); // 同样使用缓存
        if (pix.isNull())
        {
            pix = generatePlaceholder(QString("图 %1").arg(i + 1), Qt::gray, Config::Test1::THUMBNAIL_SIZE);
        }
        else
        {
            pix = pix.scaled(Config::Test1::THUMBNAIL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        thumb->setPixmap(pix);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setStyleSheet(Config::Test1::STYLE_THUMBNAIL_BORDER);
        grid->addWidget(thumb, i / 5, i % 5);
    }

    // 完成按钮
    QPushButton *finishBtn = new QPushButton(Config::Test1::BTN_TEXT_FINISH);
    connect(finishBtn, &QPushButton::clicked, [this]()
            { emit levelCompleted(); });
    grid->addWidget(finishBtn, 2, 0, 1, 5, Qt::AlignCenter);

    emit logMessage("Test 1 completed.");
}

QPixmap Test1::generatePlaceholder(QString text, QColor color, QSize size)
{
    // 动态生成的占位图通常不缓存，因为包含特定文本
    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", Config::Test1::FONT_SIZE_PLACEHOLDER, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
