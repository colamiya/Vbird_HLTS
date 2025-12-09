#include "test1.h"
#include "config.h"
#include "utils.h" // Include utils for Custom Dialogs
#include <QMessageBox>
#include <QRegularExpression>

Test1::Test1(QWidget *parent) : QWidget(parent)
{
    // 主布局 (网格布局，方便右上角放置按钮)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setAlignment(Qt::AlignCenter);

    // 返回按钮 (右上角)
    returnBtn = new QPushButton(Config::Test1::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test1::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test1::GET_BTN_RETURN_STYLE());
    connect(returnBtn, &QPushButton::clicked, [this]()
            {
        // Replace standard QMessageBox with Custom Confirm Dialog
        if (Utils::ShowCustomConfirmDialog(this, "确认退出", "确定要退出当前测试并返回主菜单吗？\n当前进度将不会保存。")) {
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
    prevBtn = new QPushButton(Config::Test1::BTN_TEXT_PREV);
    nextBtn = new QPushButton(Config::Test1::BTN_TEXT_NEXT);

    QString navBtnStyle = Config::Test1::GET_BTN_NAV_STYLE();
    prevBtn->setStyleSheet(navBtnStyle);
    nextBtn->setStyleSheet(navBtnStyle);

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

        // 使用当前缩放比例计算显示尺寸
        QSize currentDisplaySize = Config::Test1::DISPLAY_SIZE * m_currentScale;

        if (!pixmap.isNull())
        {
            slideImageLabel->setPixmap(pixmap.scaled(currentDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
        else
        {
            slideImageLabel->setPixmap(generatePlaceholder(
                QString("幻灯片 %1 (缺失: %2)").arg(currentSlideIndex + 1).arg(imagePath),
                Config::Test1::COL_PLACEHOLDER_BG,
                currentDisplaySize));
        }
    }
    else
    {
        // 索引越界回退
        QSize currentDisplaySize = Config::Test1::DISPLAY_SIZE * m_currentScale;
        slideImageLabel->setPixmap(generatePlaceholder("无效的幻灯片索引", Qt::red, currentDisplaySize));
    }
}

void Test1::finishSlideshow()
{
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    // Store reference to finishBtn to update its size later
    finishBtn = new QPushButton(Config::Test1::BTN_TEXT_FINISH);

    // 创建网格布局显示缩略图
    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    summaryThumbnails.clear(); // Clear old references if any

    // Scale spacing for grid
    int spacing = 10 * m_currentScale;
    grid->setSpacing(spacing);

    for (int i = 0; i < totalSlides; ++i)
    {
        QLabel *thumb = new QLabel();

        QString imagePath = "";
        if (i < Config::Test1::SLIDE_IMAGES().size())
        {
            imagePath = Config::Test1::SLIDE_IMAGES()[i];
        }

        // Store path in property for resizing later
        thumb->setProperty("imagePath", imagePath);
        thumb->setProperty("imageIndex", i); // Backup

        QPixmap pix = getPixmap(imagePath); // 同样使用缓存
        QSize currentThumbSize = Config::Test1::THUMBNAIL_SIZE * m_currentScale;

        if (pix.isNull())
        {
            pix = generatePlaceholder(QString("图 %1").arg(i + 1), Qt::gray, currentThumbSize);
        }
        else
        {
            pix = pix.scaled(currentThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        thumb->setPixmap(pix);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setStyleSheet(Config::Test1::STYLE_THUMBNAIL_BORDER);

        summaryThumbnails.append(thumb); // Store reference
        grid->addWidget(thumb, i / 5, i % 5);
    }

    // 完成按钮
    // Apply style for finish button (similar to nav but maybe bigger?)
    // Using nav style for simplicity as requested, or default global
    finishBtn->setStyleSheet(Config::Test1::GET_BTN_NAV_STYLE());

    // Initial Size Update
    int btnH = Config::Test1::BTN_HEIGHT_BASE * m_currentScale;
    if (btnH < 40) btnH = 40;
    finishBtn->setMinimumHeight(btnH);

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

void Test1::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLayoutScale();
    // Force immediate update
    this->update();
}

void Test1::updateLayoutScale()
{
    // Calculate scale based on current size vs reference size
    float scaleX = (float)width() / REF_SIZE.width();
    float scaleY = (float)height() / REF_SIZE.height();
    m_currentScale = qMin(scaleX, scaleY); // Maintain aspect ratio for content elements generally

    // Scale Slide Image
    // Use currentSlideIndex to reload and scale the image
    // Note: updateSlide() uses Config::Test1::DISPLAY_SIZE.
    // We should scale that display size.
    QSize scaledDisplaySize = Config::Test1::DISPLAY_SIZE * m_currentScale;
    if (!slideImageLabel->pixmap().isNull()) {
        // Reload current image to prevent degradation from repeated scaling of cached pixmap
        if (currentSlideIndex >= 0 && currentSlideIndex < Config::Test1::SLIDE_IMAGES().size()) {
             QString path = Config::Test1::SLIDE_IMAGES()[currentSlideIndex];
             QPixmap p = getPixmap(path);
             if(!p.isNull()) {
                 slideImageLabel->setPixmap(p.scaled(scaledDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
             }
        }
    }

    // Scale Buttons (Prev/Next/Finish)
    // Use BTN_HEIGHT_BASE from Config
    int btnW = 100 * m_currentScale; // Approx initial width
    int btnH = Config::Test1::BTN_HEIGHT_BASE * m_currentScale;
    if (btnW < 60) btnW = 60;
    if (btnH < 30) btnH = 30; // Minimum safety

    int btnFontSize = static_cast<int>(Config::Text::SIZE_TEST1_NAV_BTN * m_currentScale);
    if (btnFontSize < 8) btnFontSize = 8;

    // Use regex to update font-size in existing stylesheet to preserve colors
    QString navBtnStyle = Config::Test1::GET_BTN_NAV_STYLE();
    navBtnStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(btnFontSize));

    if(prevBtn) {
        prevBtn->setStyleSheet(navBtnStyle);
        prevBtn->setMinimumHeight(btnH);
    }
    if(nextBtn) {
        nextBtn->setStyleSheet(navBtnStyle);
        nextBtn->setMinimumHeight(btnH);
    }
    if(finishBtn) {
        finishBtn->setStyleSheet(navBtnStyle);
        finishBtn->setMinimumHeight(btnH);
    }

    // Scale Return Button
    // We scale Config::Test1::RETURN_BTN_SIZE (which is now larger in config)
    QSize scaledRetBtnMin = Config::Test1::RETURN_BTN_SIZE * m_currentScale;

    // Ensure the height respects the new config logic (max of scaled size or min limit)
    int retBtnH = scaledRetBtnMin.height();
    if (retBtnH < Config::Test1::BTN_HEIGHT_BASE) retBtnH = Config::Test1::BTN_HEIGHT_BASE; // At least base
    scaledRetBtnMin.setHeight(retBtnH);

    if (scaledRetBtnMin.width() < 120) scaledRetBtnMin.setWidth(120);

    if(returnBtn) {
        returnBtn->setFixedSize(QSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX)); // Clear fixed size
        returnBtn->setMinimumSize(scaledRetBtnMin);
        returnBtn->setMaximumSize(scaledRetBtnMin.width() * 2, scaledRetBtnMin.height());

        int retBtnFontSize = static_cast<int>(Config::Text::SIZE_TEST1_RETURN_BTN * m_currentScale);
        if (retBtnFontSize < 10) retBtnFontSize = 10;

        QString retBtnStyle = Config::Test1::GET_BTN_RETURN_STYLE();
        retBtnStyle.replace(QRegularExpression("font-size: \\d+px"), QString("font-size: %1px").arg(retBtnFontSize));
        returnBtn->setStyleSheet(retBtnStyle);
    }

    // Scale Summary Grid
    if (slideshowSummaryWidget && slideshowSummaryWidget->isVisible()) {
        QGridLayout *grid = qobject_cast<QGridLayout*>(slideshowSummaryWidget->layout());
        if (grid) {
            grid->setSpacing(10 * m_currentScale);
        }

        QSize newThumbSize = Config::Test1::THUMBNAIL_SIZE * m_currentScale;

        for(QLabel *thumb : summaryThumbnails) {
            if(!thumb) continue;
            QString path = thumb->property("imagePath").toString();
            int idx = thumb->property("imageIndex").toInt();

            QPixmap pix = getPixmap(path);
            if(!pix.isNull()) {
                 thumb->setPixmap(pix.scaled(newThumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                 // Regenerate placeholder with new size
                 thumb->setPixmap(generatePlaceholder(QString("图 %1").arg(idx + 1), Qt::gray, newThumbSize));
            }
        }
    }
}
