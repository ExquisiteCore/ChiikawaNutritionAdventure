#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

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
    
    // 开始播放背景音乐
    AudioManager::getInstance()->playBackgroundMusic();
}

void MainWindow::setupGameUI()
{
    // 创建游戏主界面
    gameWidget = new QWidget();
    setCentralWidget(gameWidget);
    
    gameLayout = new QHBoxLayout(gameWidget);
    gameLayout->setSpacing(30);
    gameLayout->setContentsMargins(50, 50, 50, 50);
    
    // 主要内容布局（垂直排列）
    QVBoxLayout* mainContentLayout = new QVBoxLayout();
    mainContentLayout->setSpacing(20);
    
    // 按钮布局（横向排列）
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    
    // 在按钮左右两边添加弹性间距以实现居中
    buttonLayout->addStretch();
    
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
    
    // 添加按钮到布局（左右居中）
    buttonLayout->addWidget(gameIntroButton);
    buttonLayout->addWidget(levelsButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(logoutButton);
    buttonLayout->addStretch();
    
    // 将按钮布局添加到主要内容布局（添加上方间距使按钮位置上移）
    mainContentLayout->addStretch(1);
    mainContentLayout->addLayout(buttonLayout);
    mainContentLayout->addStretch(3);
    
    // 添加到主布局（左右居中）
    gameLayout->addStretch();
    gameLayout->addLayout(mainContentLayout);
    gameLayout->addStretch();
    
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
        "    background-color: #fd79a8;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    padding: 15px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e84393;"
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
    // 创建设置对话框
    QDialog *settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle("游戏设置");
    settingsDialog->setFixedSize(450, 400);
    settingsDialog->setStyleSheet(
        "QDialog {"
        "    background-color: #2d3436;"
        "    border-radius: 10px;"
        "}"
        "QLabel {"
        "    color: #ddd;"
        "    font-size: 14px;"
        "    margin: 5px 0;"
        "}"
        "QSlider::groove:horizontal {"
        "    border: 1px solid #999;"
        "    height: 8px;"
        "    background: #555;"
        "    border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: #74b9ff;"
        "    border: 1px solid #5c5c5c;"
        "    width: 18px;"
        "    margin: -2px 0;"
        "    border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background: #74b9ff;"
        "    border-radius: 4px;"
        "}"
        "QCheckBox {"
        "    color: #ddd;"
        "    font-size: 14px;"
        "    margin: 10px 0;"
        "}"
        "QCheckBox::indicator {"
        "    width: 18px;"
        "    height: 18px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    background-color: #555;"
        "    border: 2px solid #999;"
        "    border-radius: 9px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    background-color: #74b9ff;"
        "    border: 2px solid #74b9ff;"
        "    border-radius: 9px;"
        "}"
        "QPushButton {"
        "    background-color: #636e72;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 24px;"
        "    font-size: 14px;"
        "    min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2d3436;"
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(settingsDialog);
    layout->setSpacing(15);
    layout->setContentsMargins(25, 25, 25, 25);
    
    // 标题
    QLabel *titleLabel = new QLabel("⚙️ 游戏设置");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #ffcc00; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // 音乐音量设置
    QLabel *musicVolumeLabel = new QLabel("🎵 背景音乐音量:");
    layout->addWidget(musicVolumeLabel);
    
    QHBoxLayout *musicLayout = new QHBoxLayout();
    QSlider *musicVolumeSlider = new QSlider(Qt::Horizontal);
    musicVolumeSlider->setRange(0, 100);
    musicVolumeSlider->setValue(30); // 默认30%
    
    QLabel *musicVolumeValueLabel = new QLabel("30%");
    musicVolumeValueLabel->setAlignment(Qt::AlignCenter);
    musicVolumeValueLabel->setMinimumWidth(50);
    musicVolumeValueLabel->setStyleSheet("font-weight: bold; color: #74b9ff;");
    
    musicLayout->addWidget(musicVolumeSlider);
    musicLayout->addWidget(musicVolumeValueLabel);
    layout->addLayout(musicLayout);
    
    // 音效音量设置
    QLabel *soundVolumeLabel = new QLabel("🔊 音效音量:");
    layout->addWidget(soundVolumeLabel);
    
    QHBoxLayout *soundLayout = new QHBoxLayout();
    QSlider *soundVolumeSlider = new QSlider(Qt::Horizontal);
    soundVolumeSlider->setRange(0, 100);
    soundVolumeSlider->setValue(50); // 默认50%
    
    QLabel *soundVolumeValueLabel = new QLabel("50%");
    soundVolumeValueLabel->setAlignment(Qt::AlignCenter);
    soundVolumeValueLabel->setMinimumWidth(50);
    soundVolumeValueLabel->setStyleSheet("font-weight: bold; color: #74b9ff;");
    
    soundLayout->addWidget(soundVolumeSlider);
    soundLayout->addWidget(soundVolumeValueLabel);
    layout->addLayout(soundLayout);
    
    // 音效开关
    QCheckBox *soundEffectCheckBox = new QCheckBox("启用音效");
    soundEffectCheckBox->setChecked(true);
    layout->addWidget(soundEffectCheckBox);
    
    layout->addStretch();
    
    // 按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *applyButton = new QPushButton("应用");
    applyButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #74b9ff;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0984e3;"
        "}"
    );
    
    QPushButton *cancelButton = new QPushButton("取消");
    
    buttonLayout->addStretch(); // 添加弹性空间
    buttonLayout->addWidget(applyButton);
    buttonLayout->addSpacing(10); // 按钮间距
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch(); // 添加弹性空间
    
    layout->addSpacing(20); // 与上面内容的间距
    layout->addLayout(buttonLayout);
    
    // 连接信号
    connect(musicVolumeSlider, &QSlider::valueChanged, [musicVolumeValueLabel](int value) {
        musicVolumeValueLabel->setText(QString("%1%").arg(value));
    });
    
    connect(soundVolumeSlider, &QSlider::valueChanged, [soundVolumeValueLabel](int value) {
        soundVolumeValueLabel->setText(QString("%1%").arg(value));
    });
    
    connect(applyButton, &QPushButton::clicked, [=]() {
        // 应用设置
        float musicVolume = musicVolumeSlider->value() / 100.0f;
        float soundVolume = soundVolumeSlider->value() / 100.0f;
        bool soundEnabled = soundEffectCheckBox->isChecked();
        
        // 应用音频设置
        AudioManager* audioManager = AudioManager::getInstance();
        audioManager->setMusicVolume(musicVolume);
        audioManager->setSoundVolume(soundEnabled ? soundVolume : 0.0f);
        
        QMessageBox::information(settingsDialog, "设置", "设置已保存！");
        settingsDialog->accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, settingsDialog, &QDialog::reject);
    
    // 显示对话框
    settingsDialog->exec();
    settingsDialog->deleteLater();
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
    // 停止背景音乐，播放模式1游戏音乐
    AudioManager::getInstance()->playGameMusic(AudioManager::MusicType::Mode1Game);
    
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
    // 停止游戏音乐，恢复背景音乐
    AudioManager::getInstance()->playBackgroundMusic();
    
    // 游戏窗口关闭时，重新显示主窗口
    if (carbohydrateGameWindow) {
        carbohydrateGameWindow->hide();
    }
    this->show();
}

void MainWindow::onSugarOilBattleClicked()
{
    qDebug() << "模式2按钮被点击";
    
    // 停止背景音乐，播放模式2游戏音乐
    AudioManager::getInstance()->playGameMusic(AudioManager::MusicType::Mode2Game);
    
    if (!sugarOilGameWindow) {
        qDebug() << "创建新的SugarOilGameWindow";
        sugarOilGameWindow = new SugarOilGameWindow(this);
        connect(sugarOilGameWindow, &SugarOilGameWindow::gameWindowClosed,
                this, &MainWindow::onSugarOilGameClosed);
    }
    
    // 检查窗口是否已经可见，避免重复操作
    if (sugarOilGameWindow->isVisible()) {
        qDebug() << "游戏窗口已可见，激活窗口";
        sugarOilGameWindow->raise();
        sugarOilGameWindow->activateWindow();
        return;
    }
    
    qDebug() << "隐藏主窗口";
    this->hide();
    
    qDebug() << "显示游戏窗口";
    sugarOilGameWindow->show();
    
    // 确保窗口完全显示后再进行后续操作
    if (sugarOilGameWindow->isVisible()) {
        sugarOilGameWindow->raise();
        sugarOilGameWindow->activateWindow();
        
        // 只在必要时进行单次刷新
        QApplication::processEvents();
        
        qDebug() << "游戏窗口显示状态:" << sugarOilGameWindow->isVisible();
        qDebug() << "游戏窗口是否为活动窗口:" << sugarOilGameWindow->isActiveWindow();
        
        qDebug() << "启动新游戏";
        sugarOilGameWindow->startNewGame();
    } else {
        qDebug() << "错误：游戏窗口显示失败";
        this->show(); // 恢复主窗口显示
        return;
    }
    
    qDebug() << "模式2启动完成";
}

void MainWindow::onSugarOilGameClosed()
{
    // 停止游戏音乐，恢复背景音乐
    AudioManager::getInstance()->playBackgroundMusic();
    
    if (sugarOilGameWindow) {
        sugarOilGameWindow->hide();
    }
    this->show();
    this->raise();
    this->activateWindow();
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
