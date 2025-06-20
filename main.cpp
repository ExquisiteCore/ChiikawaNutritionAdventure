#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    a.setApplicationName("ちいかわ营养大冒险");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("ChiikawaGame");
    
    MainWindow w;
    // 主窗口的显示由登录窗口控制，这里不需要直接显示
    
    return a.exec();
}
