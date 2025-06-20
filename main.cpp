#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    a.setApplicationName("ちいかわ营养大冒险");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("ChiikawaGame");
    
    // 创建主窗口（会自动显示登录窗口）
    MainWindow w;
    w.show();
    
    return a.exec();
}
