#include "sugar_oil_game_window.h"
#include <QApplication>
#include <QScreen>
#include <QFont>
#include <QGraphicsDropShadowEffect>

SugarOilGameWindow::SugarOilGameWindow(QWidget *parent)
    : QWidget(parent)
    , mainLayout(nullptr)
    , gameAreaLayout(nullptr)
    , controlPanelLayout(nullptr)
    , gameView(nullptr)
    , gameScene(nullptr)
    , controlPanel(nullptr)
    , currentTime(GAME_DURATION_SECONDS)
    , currentLives(USAGI_INITIAL_LIVES)
    , currentScore(0)
    , gameActive(false)
{
    setupUI();
    
    // 设置窗口属性
    setWindowTitle("ちいかわ营养大冒险 - 糖油混合物歼灭战");
    setFixedSize(SUGAR_OIL_WINDOW_WIDTH, SUGAR_OIL_WINDOW_HEIGHT);
    
    // 居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 设置样式
    setStyleSheet(
        "QWidget {"
        "    background-color: #2c3e50;"
        "    color: white;"
        "    font-family: 'PingFang SC', 'Microsoft YaHei', Arial;"
        "}"
        "QLabel {"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 10px 20px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
        "QProgressBar {"
        "    border: 2px solid #34495e;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    color: white;"
        "    font-weight: bold;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #e74c3c;"
        "    border-radius: 3px;"
        "}"
    );
}

SugarOilGameWindow::~SugarOilGameWindow()
{
    if (gameScene) {
        delete gameScene;
    }
}

void SugarOilGameWindow::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    setupGameArea();
    setupControlPanel();
    
    setLayout(mainLayout);
}

void SugarOilGameWindow::setupGameArea()
{
    gameAreaLayout = new QHBoxLayout();
    gameAreaLayout->setSpacing(10);
    
    // 创建游戏场景和视图
    gameScene = new SugarOilGameScene(this);
    gameView = new QGraphicsView(gameScene);
    gameView->setFixedSize(SUGAR_OIL_SCENE_WIDTH + 20, SUGAR_OIL_SCENE_HEIGHT + 20);
    gameView->setRenderHint(QPainter::Antialiasing);
    gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gameView->setFrameStyle(QFrame::Box);
    
    // 连接游戏场景信号
    connect(gameScene, &SugarOilGameScene::gameWon, this, &SugarOilGameWindow::onGameWon);
    connect(gameScene, &SugarOilGameScene::gameLost, this, &SugarOilGameWindow::onGameLost);
    connect(gameScene, &SugarOilGameScene::gameStateChanged, this, &SugarOilGameWindow::onGameStateChanged);
    connect(gameScene, &SugarOilGameScene::timeChanged, this, &SugarOilGameWindow::onTimeChanged);
    connect(gameScene, &SugarOilGameScene::livesChanged, this, &SugarOilGameWindow::onLivesChanged);
    connect(gameScene, &SugarOilGameScene::scoreChanged, this, &SugarOilGameWindow::onScoreChanged);
    connect(gameScene, &SugarOilGameScene::itemCollected, this, &SugarOilGameWindow::onItemCollected);
    connect(gameScene, &SugarOilGameScene::creatureEncountered, this, &SugarOilGameWindow::onCreatureEncountered);
    
    gameAreaLayout->addWidget(gameView);
    mainLayout->addLayout(gameAreaLayout);
}

void SugarOilGameWindow::setupControlPanel()
{
    controlPanel = new QWidget();
    controlPanel->setFixedHeight(120);
    controlPanelLayout = new QVBoxLayout(controlPanel);
    
    // 标题
    titleLabel = new QLabel("🍗 糖油混合物歼灭战 🍗");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    
    // 游戏信息布局
    QHBoxLayout* infoLayout = new QHBoxLayout();
    
    // 时间显示
    QVBoxLayout* timeLayout = new QVBoxLayout();
    timeLabel = new QLabel("时间: 05:00");
    timeLabel->setFont(QFont("Arial", 12, QFont::Bold));
    timeProgressBar = new QProgressBar();
    timeProgressBar->setRange(0, GAME_DURATION_SECONDS);
    timeProgressBar->setValue(GAME_DURATION_SECONDS);
    timeProgressBar->setFormat("%v 秒");
    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeProgressBar);
    
    // 生命值显示
    livesLabel = new QLabel("生命: ❤️❤️❤️");
    livesLabel->setFont(QFont("Arial", 12, QFont::Bold));
    
    // 分数显示
    scoreLabel = new QLabel("分数: 0");
    scoreLabel->setFont(QFont("Arial", 12, QFont::Bold));
    
    infoLayout->addLayout(timeLayout);
    infoLayout->addWidget(livesLabel);
    infoLayout->addWidget(scoreLabel);
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    backButton = new QPushButton("返回主菜单");
    restartButton = new QPushButton("重新开始");
    instructionsButton = new QPushButton("游戏说明");
    pauseButton = new QPushButton("暂停");
    
    connect(backButton, &QPushButton::clicked, this, &SugarOilGameWindow::onBackButtonClicked);
    connect(restartButton, &QPushButton::clicked, this, &SugarOilGameWindow::onRestartButtonClicked);
    connect(instructionsButton, &QPushButton::clicked, this, &SugarOilGameWindow::onInstructionsButtonClicked);
    
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(instructionsButton);
    buttonLayout->addWidget(pauseButton);
    
    // 状态信息
    statusLabel = new QLabel("准备开始游戏...");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFont(QFont("Arial", 10));
    
    itemInfoLabel = new QLabel("");
    itemInfoLabel->setAlignment(Qt::AlignCenter);
    itemInfoLabel->setFont(QFont("Arial", 9));
    
    creatureInfoLabel = new QLabel("");
    creatureInfoLabel->setAlignment(Qt::AlignCenter);
    creatureInfoLabel->setFont(QFont("Arial", 9));
    
    controlPanelLayout->addWidget(titleLabel);
    controlPanelLayout->addLayout(infoLayout);
    controlPanelLayout->addLayout(buttonLayout);
    controlPanelLayout->addWidget(statusLabel);
    controlPanelLayout->addWidget(itemInfoLabel);
    controlPanelLayout->addWidget(creatureInfoLabel);
    
    mainLayout->addWidget(controlPanel);
}

void SugarOilGameWindow::startNewGame()
{
    if (gameScene) {
        gameScene->resetGame();
        gameScene->startGame();
        gameActive = true;
        statusLabel->setText("游戏进行中...");
        pauseButton->setText("暂停");
        
        // 连接暂停按钮
        disconnect(pauseButton, &QPushButton::clicked, nullptr, nullptr);
        connect(pauseButton, &QPushButton::clicked, [this]() {
            if (gameScene->isGameRunning()) {
                gameScene->pauseGame();
                pauseButton->setText("继续");
            } else {
                gameScene->resumeGame();
                pauseButton->setText("暂停");
            }
        });
    }
}

void SugarOilGameWindow::showGameInstructions()
{
    QMessageBox::information(this, "游戏说明", 
                           "🎮 糖油混合物歼灭战 🎮\n\n"
                           "游戏背景：\n"
                           "乌萨奇想通过健身获得好身材，但总是忍不住吃糖油混合物。\n"
                           "现在他决定在潜意识中消灭这些敌人！\n\n"
                           "游戏规则：\n"
                           "• 坚持300秒即可获胜\n"
                           "• 避免接触糖油混合物敌人\n"
                           "• 收集24种道具获得属性加成\n"
                           "• 遇到5种奇异生物获得特殊能力\n\n"
                           "操作方法：\n"
                           "• WASD 或 方向键：移动\n"
                           "• 空格键：使用特殊能力\n"
                           "• ESC键：暂停游戏\n\n"
                           "祝你好运，成为贵州版彭于晏！");
}

void SugarOilGameWindow::keyPressEvent(QKeyEvent *event)
{
    if (gameScene && gameActive) {
        gameScene->handleKeyPress(event);
    }
    QWidget::keyPressEvent(event);
}

void SugarOilGameWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (gameScene && gameActive) {
        gameScene->handleKeyRelease(event);
    }
    QWidget::keyReleaseEvent(event);
}

void SugarOilGameWindow::closeEvent(QCloseEvent *event)
{
    emit gameWindowClosed();
    QWidget::closeEvent(event);
}

void SugarOilGameWindow::onGameWon()
{
    gameActive = false;
    showGameResult(true);
}

void SugarOilGameWindow::onGameLost()
{
    gameActive = false;
    showGameResult(false);
}

void SugarOilGameWindow::onGameStateChanged(SugarOilGameState newState)
{
    switch (newState) {
    case SUGAR_OIL_RUNNING:
        statusLabel->setText("游戏进行中...");
        pauseButton->setText("暂停");
        break;
    case SUGAR_OIL_PAUSED:
        statusLabel->setText("游戏已暂停");
        pauseButton->setText("继续");
        break;
    case SUGAR_OIL_WON:
        statusLabel->setText("恭喜获胜！");
        break;
    case SUGAR_OIL_LOST:
        statusLabel->setText("游戏失败");
        break;
    default:
        statusLabel->setText("准备开始...");
        break;
    }
}

void SugarOilGameWindow::onBackButtonClicked()
{
    int ret = QMessageBox::question(this, "返回主菜单", 
                                  "确定要返回主菜单吗？当前游戏进度将丢失。",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        emit gameWindowClosed();
        close();
    }
}

void SugarOilGameWindow::onRestartButtonClicked()
{
    int ret = QMessageBox::question(this, "重新开始", 
                                  "确定要重新开始游戏吗？",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        startNewGame();
    }
}

void SugarOilGameWindow::onInstructionsButtonClicked()
{
    showGameInstructions();
}

void SugarOilGameWindow::onTimeChanged(int remainingSeconds)
{
    currentTime = remainingSeconds;
    updateTimeDisplay(remainingSeconds);
}

void SugarOilGameWindow::onLivesChanged(int lives)
{
    currentLives = lives;
    updateLivesDisplay(lives);
}

void SugarOilGameWindow::onScoreChanged(int score)
{
    currentScore = score;
    updateScoreDisplay(score);
}

void SugarOilGameWindow::onItemCollected(ItemType itemType)
{
    QString itemName;
    switch (itemType) {
    case ITEM_SPEED_BOOST: itemName = "速度提升"; break;
    case ITEM_DAMAGE_BOOST: itemName = "攻击力提升"; break;
    case ITEM_SHIELD: itemName = "护盾"; break;
    case ITEM_HEALTH: itemName = "生命恢复"; break;
    case ITEM_SCORE_BONUS: itemName = "分数奖励"; break;
    case ITEM_TIME_SLOW: itemName = "时间减缓"; break;
    case ITEM_INVINCIBLE: itemName = "无敌"; break;
    case ITEM_DOUBLE_SCORE: itemName = "双倍分数"; break;
    case ITEM_EXTRA_LIFE: itemName = "额外生命"; break;
    case ITEM_FREEZE_ENEMIES: itemName = "冰冻敌人"; break;
    case ITEM_MAGNET: itemName = "磁铁"; break;
    case ITEM_BOMB: itemName = "炸弹"; break;
    default: itemName = "神秘道具"; break;
    }
    
    itemInfoLabel->setText(QString("获得道具: %1").arg(itemName));
    
    // 3秒后清除信息
    QTimer::singleShot(3000, [this]() {
        itemInfoLabel->setText("");
    });
}

void SugarOilGameWindow::onCreatureEncountered(CreatureType creatureType)
{
    QString creatureName;
    switch (creatureType) {
    case CREATURE_HELPER: creatureName = "助手生物"; break;
    case CREATURE_GUARDIAN: creatureName = "守护生物"; break;
    case CREATURE_HEALER: creatureName = "治疗生物"; break;
    case CREATURE_SPEEDSTER: creatureName = "速度生物"; break;
    case CREATURE_WARRIOR: creatureName = "战士生物"; break;
    default: creatureName = "神秘生物"; break;
    }
    
    creatureInfoLabel->setText(QString("遇到生物: %1").arg(creatureName));
    
    // 5秒后清除信息
    QTimer::singleShot(5000, [this]() {
        creatureInfoLabel->setText("");
    });
}

void SugarOilGameWindow::updateTimeDisplay(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    timeLabel->setText(QString("时间: %1:%2").arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')));
    timeProgressBar->setValue(seconds);
}

void SugarOilGameWindow::updateLivesDisplay(int lives)
{
    QString heartsText = "生命: ";
    for (int i = 0; i < lives; i++) {
        heartsText += "❤️";
    }
    livesLabel->setText(heartsText);
}

void SugarOilGameWindow::updateScoreDisplay(int score)
{
    scoreLabel->setText(QString("分数: %1").arg(score));
}

void SugarOilGameWindow::showGameResult(bool won)
{
    QString title = won ? "游戏胜利!" : "游戏失败!";
    QString message;
    
    if (won) {
        message = "🎉 恭喜！🎉\n\n"
                 "乌萨奇成功抵抗住了糖油混合物的诱惑！\n"
                 "消灭了所有糖油混合物！\n"
                 "现在他可以成为贵州版彭于晏了！\n\n" +
                 QString("最终分数: %1").arg(currentScore);
    } else {
        message = "😢 很遗憾！😢\n\n"
                 "乌萨奇没能抵抗住糖油混合物的诱惑...\n"
                 "不过不要气馁，再试一次吧！\n\n" +
                 QString("最终分数: %1").arg(currentScore);
    }
    
    QMessageBox::information(this, title, message);
}

void SugarOilGameWindow::updateControlPanel()
{
    updateTimeDisplay(currentTime);
    updateLivesDisplay(currentLives);
    updateScoreDisplay(currentScore);
}