#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>

#include "sugar_oil_main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("吃糖吃油大冒险");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("游戏开发团队");
    app.setOrganizationDomain("example.com");
    
    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // 设置深色主题（可选）
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    // 注释掉深色主题，使用默认主题
    // app.setPalette(darkPalette);
    
    // 创建启动画面（可选）
    QPixmap splashPixmap(400, 300);
    splashPixmap.fill(QColor(144, 238, 144)); // 浅绿色背景
    
    QPainter painter(&splashPixmap);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(splashPixmap.rect(), Qt::AlignCenter, "吃糖吃油大冒险\n模式2\n\n加载中...");
    
    QSplashScreen splash(splashPixmap);
    splash.show();
    
    // 处理事件以显示启动画面
    app.processEvents();
    
    // 模拟加载时间
    QTimer::singleShot(2000, [&splash]() {
        splash.close();
    });
    
    try {
        // 创建主窗口
        SugarOilMainWindow mainWindow;
        
        // 延迟显示主窗口
        QTimer::singleShot(2000, [&mainWindow]() {
            mainWindow.show();
            mainWindow.raise();
            mainWindow.activateWindow();
        });
        
        qDebug() << "应用程序启动成功";
        
        return app.exec();
        
    } catch (const std::exception& e) {
        qCritical() << "应用程序启动失败:" << e.what();
        
        QMessageBox::critical(nullptr, "启动错误", 
                             QString("应用程序启动失败：\n%1").arg(e.what()));
        
        return -1;
    } catch (...) {
        qCritical() << "应用程序启动失败：未知错误";
        
        QMessageBox::critical(nullptr, "启动错误", 
                             "应用程序启动失败：未知错误");
        
        return -1;
    }
}