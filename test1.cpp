#include "test1.h"

Test1::Test1(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    slideshowContainer = new QWidget();
    QVBoxLayout *slideLayout = new QVBoxLayout(slideshowContainer);

    slideImageLabel = new QLabel();
    slideImageLabel->setAlignment(Qt::AlignCenter);
    slideLayout->addWidget(slideImageLabel);

    QHBoxLayout *navLayout = new QHBoxLayout();
    QPushButton *prevBtn = new QPushButton("上一页");
    QPushButton *nextBtn = new QPushButton("下一页");
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
    QString imagePath = QString("source/Test1/fig%1.png").arg(currentSlideIndex + 1);
    QPixmap pixmap(imagePath);

    if (!pixmap.isNull()) {
        slideImageLabel->setPixmap(pixmap.scaled(800, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        slideImageLabel->setPixmap(generatePlaceholder(
            QString("幻灯片 %1 (缺失: %2)").arg(currentSlideIndex + 1).arg(imagePath),
            Qt::blue,
            QSize(800, 450)
        ));
    }
}

void Test1::finishSlideshow() {
    slideshowContainer->setVisible(false);
    slideshowSummaryWidget->setVisible(true);

    QGridLayout *grid = new QGridLayout(slideshowSummaryWidget);
    for (int i = 0; i < totalSlides; ++i) {
        QLabel *thumb = new QLabel();

        QString imagePath = QString("source/Test1/fig%1.png").arg(i + 1);
        QPixmap pix(imagePath);
        if (pix.isNull()) {
             pix = generatePlaceholder(QString("图 %1").arg(i + 1), Qt::gray, QSize(200, 150));
        } else {
             pix = pix.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        thumb->setPixmap(pix);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setStyleSheet("border: 1px solid #ccc;");
        grid->addWidget(thumb, i / 5, i % 5);
    }

    QPushButton *finishBtn = new QPushButton("完成学习 (返回主菜单)");
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
    painter.setFont(QFont("Microsoft YaHei", 20, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return pixmap;
}
