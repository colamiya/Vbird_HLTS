#ifndef TEST1_H
#define TEST1_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPixmap>
#include <QPainter>
#include <QDebug>

class Test1 : public QWidget {
    Q_OBJECT
public:
    explicit Test1(QWidget *parent = nullptr);

signals:
    void levelCompleted();
    void logMessage(QString msg);

private:
    QWidget *slideshowContainer;
    QWidget *slideshowSummaryWidget;
    QLabel *slideImageLabel;

    int currentSlideIndex = 0;
    const int totalSlides = 10;

    void updateSlide();
    void finishSlideshow();
    QPixmap generatePlaceholder(QString text, QColor color, QSize size);
};

#endif // TEST1_H
