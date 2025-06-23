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
    , carbohydrateGameWindow(nullptr)
    , sugarOilGameWindow(nullptr)
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
    loginWindow = new LoginWindow(nullptr);
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
    // 先隐藏登录窗口
    if (loginWindow) {
        loginWindow->hide();
    }
    
    setupGameUI();
    applyGameStyles();
    this->show();
}

void MainWindow::setupGameUI()
{
    // 创建游戏主界面
    gameWidget = new QWidget();
    setCentralWidget(gameWidget);
    
    gameLayout = new QHBoxLayout(gameWidget);
    gameLayout->setSpacing(30);
    gameLayout->setContentsMargins(50, 50, 50, 50);
    
    // 左侧按钮布局
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(20);
    leftLayout->setContentsMargins(0, 0, 20, 0);
    
    // 按钮布局（垂直排列）
    buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 创建四个主要按钮（只显示图标，不显示文字）
    gameIntroButton = new QPushButton("", this);
    gameIntroButton->setObjectName("imageButton");
    gameIntroButton->setIcon(QIcon(":/img/ui/detailBtn.png"));
    gameIntroButton->setIconSize(QSize(120, 80));
    gameIntroButton->setFixedSize(120, 80);
    gameIntroButton->setFlat(true);
    gameIntroButton->setToolTip("游戏简介");
    
    levelsButton = new QPushButton("", this);
    levelsButton->setObjectName("imageButton");
    levelsButton->setIcon(QIcon(":/img/ui/startBtn.png"));
    levelsButton->setIconSize(QSize(120, 80));
    levelsButton->setFixedSize(120, 80);
    levelsButton->setFlat(true);
    levelsButton->setToolTip("关卡选择");
    
    settingsButton = new QPushButton("", this);
    settingsButton->setObjectName("imageButton");
    settingsButton->setIcon(QIcon(":/img/ui/checkRecordBtn.png"));
    settingsButton->setIconSize(QSize(120, 80));
    settingsButton->setFixedSize(120, 80);
    settingsButton->setFlat(true);
    settingsButton->setToolTip("游戏设置");
    
    logoutButton = new QPushButton("", this);
    logoutButton->setObjectName("imageButton");
    logoutButton->setIcon(QIcon(":/img/ui/exitBtn.png"));
    logoutButton->setIconSize(QSize(120, 80));
    logoutButton->setFixedSize(120, 80);
    logoutButton->setFlat(true);
    logoutButton->setToolTip("退出登录");
    
    // 添加按钮到布局
    buttonLayout->addWidget(gameIntroButton);
    buttonLayout->addWidget(levelsButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(logoutButton);
    buttonLayout->addStretch();
    
    // 将按钮布局添加到左侧布局
    leftLayout->addLayout(buttonLayout);
    
    // 右侧内容布局
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(30);
    
    // 欢迎标题
    welcomeLabel = new QLabel("🎮 ちいかわ营养大冒险 🎮", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setObjectName("welcomeLabel");
    
    rightLayout->addStretch();
    rightLayout->addWidget(welcomeLabel);
    rightLayout->addStretch();
    
    // 添加到主布局
    gameLayout->addLayout(leftLayout);
    gameLayout->addLayout(rightLayout);
    
    // 连接信号槽
    connect(gameIntroButton, &QPushButton::clicked, this, &MainWindow::onGameIntroClicked);
    connect(levelsButton, &QPushButton::clicked, this, &MainWindow::onLevelsClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::onGameIntroClicked()
{
    // 创建自定义对话框显示游戏简介图片
    QDialog *introDialog = new QDialog(this);
    introDialog->setWindowTitle("游戏简介");
    introDialog->setModal(true);
    introDialog->resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(introDialog);
    
    // 创建标签显示图片
    QLabel *imageLabel = new QLabel(introDialog);
    QPixmap pixmap(":/img/ui/details.png");
    
    if (!pixmap.isNull()) {
        // 缩放图片以适应对话框
        pixmap = pixmap.scaled(750, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(pixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    } else {
        imageLabel->setText("无法加载游戏简介图片");
        imageLabel->setAlignment(Qt::AlignCenter);
    }
    
    // 创建关闭按钮
    QPushButton *closeButton = new QPushButton("关闭", introDialog);
    closeButton->setFixedSize(100, 30);
    
    // 连接关闭按钮
    connect(closeButton, &QPushButton::clicked, introDialog, &QDialog::accept);
    
    // 添加到布局
    layout->addWidget(imageLabel);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // 显示对话框
    introDialog->exec();
    
    // 清理内存
    introDialog->deleteLater();
}

void MainWindow::onLevelsClicked()
{
    // 创建关卡选择对话框
    QDialog *levelDialog = new QDialog(this);
    levelDialog->setWindowTitle("关卡选择");
    levelDialog->setModal(true);
    levelDialog->resize(400, 300);
    
    QVBoxLayout *layout = new QVBoxLayout(levelDialog);
    
    // 标题
    QLabel *titleLabel = new QLabel("🎮 选择游戏模式 🎮");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 20px;");
    layout->addWidget(titleLabel);
    
    // 模式一按钮
    QPushButton *mode1Button = new QPushButton("🥬 模式一：碳水化合物之战");
    mode1Button->setStyleSheet(
        "QPushButton {"
        "    background-color: #74b9ff;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0984e3;"
        "}"
    );
    
    // 模式二按钮
    QPushButton *mode2Button = new QPushButton("🍗 模式二：糖油混合物歼灭战");
    mode2Button->setEnabled(true);
    mode2Button->setStyleSheet(
        "QPushButton {"
        "    background-color: #ddd;"
        "    color: #999;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "    font-size: 14px;"
        "}"
    );
    
    // 关闭按钮
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #636e72;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2d3436;"
        "}"
    );
    
    layout->addWidget(mode1Button);
    layout->addWidget(mode2Button);
    layout->addStretch();
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(mode1Button, &QPushButton::clicked, [this, levelDialog]() {
        levelDialog->accept();
        onCarbohydrateBattleClicked();
    });
    connect(mode2Button, &QPushButton::clicked, [this, levelDialog]() {
        levelDialog->accept();
        onSugarOilBattleClicked();
    });
    connect(closeButton, &QPushButton::clicked, levelDialog, &QDialog::accept);
    
    // 显示对话框
    levelDialog->exec();
    levelDialog->deleteLater();
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

void MainWindow::onCarbohydrateBattleClicked()
{
    // 创建并显示碳水化合物之战游戏窗口
    if (!carbohydrateGameWindow) {
        carbohydrateGameWindow = new CarbohydrateGameWindow();
        connect(carbohydrateGameWindow, &CarbohydrateGameWindow::gameWindowClosed,
                this, &MainWindow::onCarbohydrateGameClosed);
    }
    
    // 隐藏主窗口，显示游戏窗口
    this->hide();
    carbohydrateGameWindow->show();
    carbohydrateGameWindow->startNewGame();
}

void MainWindow::onCarbohydrateGameClosed()
{
    // 游戏窗口关闭时，重新显示主窗口
    if (carbohydrateGameWindow) {
        carbohydrateGameWindow->hide();
    }
    this->show();
}

void MainWindow::onSugarOilBattleClicked()
{
    if (!sugarOilGameWindow) {
        sugarOilGameWindow = new SugarOilGameWindow(this);
        connect(sugarOilGameWindow, &SugarOilGameWindow::gameWindowClosed,
                this, &MainWindow::onSugarOilGameClosed);
    }
    
    this->hide();
    sugarOilGameWindow->show();
    sugarOilGameWindow->startNewGame();
}

void MainWindow::onSugarOilGameClosed()
{
    if (sugarOilGameWindow) {
        sugarOilGameWindow->hide();
    }
    this->show();
}

void MainWindow::applyGameStyles()
{
    setStyleSheet(
        "QMainWindow {"
        "    background-image: url(:/img/gameoverBackground.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "    background-size: cover;"
        "}"
        
        "QWidget {"
        "    font-family: 'PingFang SC', 'Hiragino Sans GB', 'Arial Unicode MS', 'Helvetica Neue';"
        "}"
        
        "#welcomeLabel {"
        "    font-size: 32px;"
        "    font-weight: bold;"
        "    color: #2d3436;"
        "    margin: 20px 0;"
        "    background: transparent;"
        "    padding: 20px;"
        "}"
        
        "#imageButton {"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "    margin: 5px;"
        "}"
        
        "#imageButton:hover {"
        "    background: rgba(255, 255, 255, 0.1);"
        "    border-radius: 10px;"
        "}"
        
        "#imageButton:pressed {"
        "    background: rgba(255, 255, 255, 0.2);"
        "    border-radius: 10px;"
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
