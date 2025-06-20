#include <QApplication>
#include "mainwindow.h"
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    a.setApplicationName("ちいかわ营养大冒险");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("ChiikawaGame");
    
    // 创建并显示登录窗口
    LoginWindow loginWindow;
    loginWindow.show();
    
    return a.exec();
}
