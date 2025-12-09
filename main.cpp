#include "mainwindow.h"
#include <QApplication>

// 开发者模式开关 (Developer Mode Switch)
// 将此变量设置为 true 即可默认开启开发者模式。
// 开启后，将跳过前置任务限制，并在 RPG 模块中显示坐标点击信息。
const bool DEV_MODE_DEFAULT = false;

int main(int argc, char *argv[])
{
    // Force OpenGL backend to avoid D3D11 debug layer requirement on some Windows systems
    // This fixes the "D3D11CreateDevice: Flags (0x2) ... D3D11 SDK Layers" error
    qputenv("QSG_RHI_BACKEND", "opengl");
    qputenv("QT_MEDIA_BACKEND", "ffmpeg"); // Ensure ffmpeg backend is used

    QApplication a(argc, argv);
    MainWindow w(nullptr, DEV_MODE_DEFAULT);
    w.show();
    return a.exec();
}
