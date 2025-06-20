#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , loginWindow(nullptr)
    , gameWidget(nullptr)
{
    ui->setupUi(this);
    
    // 设置窗口属性
    setWindowTitle("ちいかわ营养大冒险");
    setFixedSize(1000, 700);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 创建并显示登录窗口
    loginWindow = new LoginWindow(this);
    connect(loginWindow, &LoginWindow::loginSuccessful, this, &MainWindow::onLoginSuccessful);
    
    // 隐藏主窗口，先显示登录窗口
    this->hide();
    loginWindow->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (loginWindow) {
        delete loginWindow;
    }
}

void MainWindow::onLoginSuccessful()
{
    // 登录成功后显示主窗口
    setupGameUI();
    applyGameStyles();
    this->show();
    
    if (loginWindow) {
        loginWindow->hide();
    }
}

void MainWindow::setupGameUI()
{
    // 创建游戏主界面
    gameWidget = new QWidget();
    setCentralWidget(gameWidget);
    
    gameLayout = new QVBoxLayout(gameWidget);
    gameLayout->setSpacing(30);
    gameLayout->setContentsMargins(50, 50, 50, 50);
    
    // 欢迎标题
    welcomeLabel = new QLabel("🎮 ちいかわ营养大冒险 🎮", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setObjectName("welcomeLabel");
    
    // 按钮布局
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 创建四个主要按钮
    gameIntroButton = new QPushButton("🎯 游戏简介", this);
    gameIntroButton->setObjectName("gameButton");
    
    levelsButton = new QPushButton("🎮 关卡选择", this);
    levelsButton->setObjectName("gameButton");
    
    settingsButton = new QPushButton("⚙️ 游戏设置", this);
    settingsButton->setObjectName("gameButton");
    
    logoutButton = new QPushButton("🚪 退出登录", this);
    logoutButton->setObjectName("logoutButton");
    
    // 添加按钮到布局
    buttonLayout->addWidget(gameIntroButton);
    buttonLayout->addWidget(levelsButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(logoutButton);
    
    // 添加到主布局
    gameLayout->addStretch();
    gameLayout->addWidget(welcomeLabel);
    gameLayout->addStretch();
    gameLayout->addLayout(buttonLayout);
    gameLayout->addStretch();
    
    // 连接信号槽
    connect(gameIntroButton, &QPushButton::clicked, this, &MainWindow::onGameIntroClicked);
    connect(levelsButton, &QPushButton::clicked, this, &MainWindow::onLevelsClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::onGameIntroClicked()
{
    QString introText = 
        "🎮 游戏简介 🎮\n\n"
        "• 吉伊、小八和乌萨奇三小只前来冒险！\n"
        "• 在游戏中，你将扮演乌萨奇，与奇美拉们战斗。\n"
        "• 使用 WASD 键控制移动，鼠标左键进行攻击。\n\n"
        "🎯 游戏模式：\n"
        "模式一（碳水化合物之战）：\n"
        "使用\"膳食纤维剑\"技能击败\"伪蔬菜\"BOSS\n\n"
        "模式二（糖油混合物歼灭战）：\n"
        "存活300秒，击败高糖油敌人（如奶茶、炸鸡）";
    
    QMessageBox::information(this, "游戏简介", introText);
}

void MainWindow::onLevelsClicked()
{
    QMessageBox::information(this, "关卡选择", 
                           "🎮 关卡选择 🎮\n\n"
                           "请选择游戏模式：\n\n"
                           "🥬 模式一：碳水化合物之战\n"
                           "🍗 模式二：糖油混合物歼灭战\n\n"
                           "（具体关卡功能将在后续开发中实现）");
}

void MainWindow::onSettingsClicked()
{
    QMessageBox::information(this, "游戏设置", 
                           "⚙️ 游戏设置 ⚙️\n\n"
                           "设置选项：\n\n"
                           "🖱️ 鼠标灵敏度调节\n"
                           "🔊 声音大小调节\n"
                           "🎵 音效开关\n\n"
                           "（具体设置功能将在后续开发中实现）");
}

void MainWindow::onLogoutClicked()
{
    int ret = QMessageBox::question(this, "退出登录", 
                                  "确定要退出登录吗？",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        this->hide();
        if (loginWindow) {
            loginWindow->show();
        }
    }
}

void MainWindow::applyGameStyles()
{
    setStyleSheet(
        "QMainWindow {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #ffeaa7, stop:1 #fab1a0);"
        "}"
        
        "QWidget {"
        "    font-family: 'Microsoft YaHei', 'SimHei', sans-serif;"
        "}"
        
        "#welcomeLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2d3436;"
        "    margin: 20px 0;"
        "    background: rgba(255, 255, 255, 0.8);"
        "    border-radius: 15px;"
        "    padding: 20px;"
        "}"
        
        "#gameButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #74b9ff, stop:1 #0984e3);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 15px;"
        "    padding: 20px 30px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    min-width: 180px;"
        "    min-height: 80px;"
        "}"
        
        "#gameButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #0984e3, stop:1 #0770c4);"
        "    transform: translateY(-2px);"
        "}"
        
        "#gameButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #0770c4, stop:1 #055a9c);"
        "}"
        
        "#logoutButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #fd79a8, stop:1 #e84393);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 15px;"
        "    padding: 20px 30px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    min-width: 180px;"
        "    min-height: 80px;"
        "}"
        
        "#logoutButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #e84393, stop:1 #d63384);"
        "}"
        
        "#logoutButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "                                stop:0 #d63384, stop:1 #c2185b);"
        "}"
    );
}
