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
#include <QMap>

class Test1 : public QWidget
{
    Q_OBJECT
public:
    explicit Test1(QWidget *parent = nullptr);

signals:
    void levelCompleted();        // 关卡完成信号
    void levelCancelled();        // 关卡取消信号
    void logMessage(QString msg); // 日志信号

private:
    QWidget *slideshowContainer;     // 幻灯片容器
    QWidget *slideshowSummaryWidget; // 总结页面容器
    QLabel *slideImageLabel;         // 显示幻灯片图片的标签

    int currentSlideIndex = 0;  // 当前幻灯片索引
    const int totalSlides = 10; // 总幻灯片数

    QMap<QString, QPixmap> m_pixmapCache; // 图片缓存，避免重复加载

    void updateSlide();     // 更新当前幻灯片显示
    void finishSlideshow(); // 完成幻灯片播放，显示总结

    // 生成占位图 (当图片缺失时使用)
    QPixmap generatePlaceholder(QString text, QColor color, QSize size);

    // 获取图片 (优先从缓存获取)
    QPixmap getPixmap(const QString &path);
};

#endif // TEST1_H
