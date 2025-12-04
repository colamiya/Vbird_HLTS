#include "mainwindow.h"
#include <QApplication>

// 开发者模式开关 (Developer Mode Switch)
// 将此变量设置为 true 即可默认开启开发者模式。
// 开启后，将跳过前置任务限制，并在 RPG 模块中显示坐标点击信息。
const bool DEV_MODE_DEFAULT = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(nullptr, DEV_MODE_DEFAULT);
    w.show();
    return a.exec();
}
