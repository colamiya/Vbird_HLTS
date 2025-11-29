#include "test1.h"
#include "config.h"
#include <QMessageBox>

Test1::Test1(QWidget *parent) : QWidget(parent) {
    // Main Layout (Grid to allow top-right positioning)
    QGridLayout *mainGrid = new QGridLayout(this);
    mainGrid->setAlignment(Qt::AlignCenter);

    // Return Button (Top Right)
    QPushButton *returnBtn = new QPushButton(Config::Test1::BTN_TEXT_BACK_TO_MENU);
    returnBtn->setFixedSize(Config::Test1::RETURN_BTN_SIZE);
    returnBtn->setStyleSheet(Config::Test1::BTN_RETURN_STYLE);
    connect(returnBtn, &QPushButton::clicked, [this]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认退出", "确定要退出当前测试并返回主菜单吗？\n当前进度将不会保存。",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Logic to handle exit without saving score/progress?
            // The levelCompleted signal usually advances progress.
            // We need a way to just go back.
            // Since we don't have a specific signal for "cancel", we might need to add one or use levelCompleted but check progress logic in MainWindow?
            // User requirement: "can cancel and not retain results".
            // If I just emit levelCompleted, it might mark it as done.
            // I should emit a specific signal or let MainWindow handle cancellation.
            // For now, I will reuse levelCompleted BUT logic in MainWindow might advance level.
            // Wait, looking at MainWindow::onLevelCompleted: "if (progressState < level + 1) progressState = level + 1;"
            // This is BAD for cancellation.

            // I need to add a "levelCancelled" signal.
            emit levelCancelled();
        }
    });
    // Add to top right: row 0, col 1, align right
    mainGrid->addWidget(returnBtn, 0, 1, Qt::AlignRight | Qt::AlignTop);

    // Content Container
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // Add content to row 1, span 2 columns
    mainGrid->addWidget(contentWidget, 1, 0, 1, 2);


    slideshowContainer = new QWidget();
    QVBoxLayout *slideLayout = new QVBoxLayout(slideshowContainer);

    slideImageLabel = new QLabel();
    slideImageLabel->setAlignment(Qt::AlignCenter);
    slideLayout->addWidget(slideImageLabel);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton(Config::Test1::BTN_TEXT_PREV);
    QPushButton *nextBtn = new QPushButton(Config::Test1::BTN_TEXT_NEXT);
    navLayout->addWidget(prevBtn);
    navLayout->addWidget(nextBtn);
    slideLayout->addLayout(navLayout);

    connect(prevBtn, &QPushButton::clicked, [this]() {
        if (currentSlideIndex > 0) {
            currentSlideIndex--;
            updateSlide();
        }
    });
    connect(nextBtn, &QPushButton::clicked, [this]() {
        if (currentSlideIndex < totalSlides - 1) {
            currentSlideIndex++;
            updateSlide();
        } else {
            finishSlideshow();
        }
    });

    layout->addWidget(slideshowContainer);

    // Summary Widget (Initially Hidden)
    slideshowSummaryWidget = new QWidget();
    slideshowSummaryWidget->setVisible(false);
    layout->addWidget(slideshowSummaryWidget);

    updateSlide();
}

void Test1::updateSlide() {
    // Check if index is within bounds of config list
    if (currentSlideIndex >= 0 && currentSlideIndex < Config::Test1::SLIDE_IMAGES().size()) {
        QString imagePath = Config::Test1::SLIDE_IMAGES()[currentSlideIndex];
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            slideImageLabel->setPixmap(pixmap.scaled(Config::Test1::DISPLAY_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            slideImageLabel->setPixmap(generatePlaceholder(
                QString("幻灯片 %1 (缺失: %2)").arg(currentSlideIndex + 1).arg(imagePath),
                Config::Test1::COL_PLACEHOLDER_BG,
                Config::Test1::DISPLAY_SIZE
            ));
        }
    } else {
        // Fallback
        slideImageLabel->setPixmap(generatePlaceholder("无效的幻灯片索引", Qt::red, Config::Test1::DISPLAY_SIZE));
    }
}

void Test1::finishSlideshow() {
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    for (int i = 0; i < totalSlides; ++i) {
        QLabel *thumb = new QLabel();

        // Use config paths
        QString imagePath = "";
        if (i < Config::Test1::SLIDE_IMAGES().size()) {
            imagePath = Config::Test1::SLIDE_IMAGES()[i];
        }

        QPixmap pix(imagePath);
        if (pix.isNull()) {
             pix = generatePlaceholder(QString("图 %1").arg(i + 1), Qt::gray, Config::Test1::THUMBNAIL_SIZE);
        } else {
             pix = pix.scaled(Config::Test1::THUMBNAIL_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        thumb->setPixmap(pix);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setStyleSheet(Config::Test1::STYLE_THUMBNAIL_BORDER);
        grid->addWidget(thumb, i / 5, i % 5);
    }

    QPushButton *finishBtn = new QPushButton(Config::Test1::BTN_TEXT_FINISH);
    connect(finishBtn, &QPushButton::clicked, [this]() {
        emit levelCompleted();
    });
    grid->addWidget(finishBtn, 2, 0, 1, 5, Qt::AlignCenter);

    emit logMessage("Test 1 completed.");
}

QPixmap Test1::generatePlaceholder(QString text, QColor color, QSize size) {
    QPixmap pixmap(size);
    pixmap.fill(color);
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Microsoft YaHei", Config::Test1::FONT_SIZE_PLACEHOLDER, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
