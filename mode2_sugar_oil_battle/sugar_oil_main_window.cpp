#include "sugar_oil_main_window.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QGroupBox>
#include <QFont>
#include <QSplitter>

SugarOilMainWindow::SugarOilMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mCentralWidget(nullptr)
    , mMainLayout(nullptr)
    , mGameView(nullptr)
    , mGameScene(nullptr)
    , mControlPanel(nullptr)
    , mControlLayout(nullptr)
    , mScoreLabel(nullptr)
    , mTimeLabel(nullptr)
    , mLevelLabel(nullptr)
    , mHealthBar(nullptr)
    , mExpBar(nullptr)
    , mStartButton(nullptr)
    , mPauseButton(nullptr)
    , mResumeButton(nullptr)
    , mStopButton(nullptr)
    , mResetButton(nullptr)
    , mExitButton(nullptr)
    , mMenuBar(nullptr)
    , mGameMenu(nullptr)
    , mHelpMenu(nullptr)
    , mStatusBar(nullptr)
    , mStatusLabel(nullptr)
    , mGameActive(false)
{
    setWindowTitle("吃糖吃油大冒险 - 模式2");
    setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    resize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    
    setupUI();
    connectSignals();
    updateGameControls();
    
    // 设置焦点策略，确保能接收键盘事件
    setFocusPolicy(Qt::StrongFocus);
    
    mStatusLabel->setText("准备开始游戏");
}

SugarOilMainWindow::~SugarOilMainWindow()
{
    if (mGameScene) {
        mGameScene->stopGame();
    }
}

void SugarOilMainWindow::setupUI()
{
    setupMenuBar();
    setupGameView();
    setupControlPanel();
    setupStatusBar();
    
    // 创建主布局
    mCentralWidget = new QWidget(this);
    setCentralWidget(mCentralWidget);
    
    mMainLayout = new QHBoxLayout(mCentralWidget);
    mMainLayout->setContentsMargins(5, 5, 5, 5);
    mMainLayout->setSpacing(10);
    
    // 添加游戏视图和控制面板
    mMainLayout->addWidget(mGameView, 1); // 游戏视图占据大部分空间
    mMainLayout->addWidget(mControlPanel, 0); // 控制面板固定宽度
}

void SugarOilMainWindow::setupMenuBar()
{
    mMenuBar = menuBar();
    
    // 游戏菜单
    mGameMenu = mMenuBar->addMenu("游戏(&G)");
    
    mStartAction = mGameMenu->addAction("开始游戏(&S)");
    mStartAction->setShortcut(QKeySequence("F1"));
    
    mPauseAction = mGameMenu->addAction("暂停游戏(&P)");
    mPauseAction->setShortcut(QKeySequence("Space"));
    
    mResumeAction = mGameMenu->addAction("继续游戏(&R)");
    mResumeAction->setShortcut(QKeySequence("Space"));
    
    mStopAction = mGameMenu->addAction("停止游戏(&T)");
    mStopAction->setShortcut(QKeySequence("Escape"));
    
    mGameMenu->addSeparator();
    
    mResetAction = mGameMenu->addAction("重置游戏(&E)");
    mResetAction->setShortcut(QKeySequence("F5"));
    
    mGameMenu->addSeparator();
    
    mExitAction = mGameMenu->addAction("退出(&X)");
    mExitAction->setShortcut(QKeySequence("Ctrl+Q"));
    
    // 帮助菜单
    mHelpMenu = mMenuBar->addMenu("帮助(&H)");
    
    mHelpAction = mHelpMenu->addAction("游戏帮助(&H)");
    mHelpAction->setShortcut(QKeySequence("F1"));
    
    mAboutAction = mHelpMenu->addAction("关于(&A)");
}

void SugarOilMainWindow::setupGameView()
{
    mGameScene = new SugarOilGameSceneNew(this);
    mGameView = new QGraphicsView(mGameScene, this);
    
    mGameView->setRenderHint(QPainter::Antialiasing);
    mGameView->setDragMode(QGraphicsView::NoDrag);
    mGameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGameView->setFrameStyle(QFrame::NoFrame);
    mGameView->setFocusPolicy(Qt::NoFocus); // 让主窗口处理键盘事件
    
    // 设置视图大小策略
    mGameView->setMinimumSize(800, 600);
    mGameView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void SugarOilMainWindow::setupControlPanel()
{
    mControlPanel = new QWidget(this);
    mControlPanel->setFixedWidth(CONTROL_PANEL_WIDTH);
    mControlPanel->setStyleSheet(
        "QWidget { "
        "    background-color: #f0f0f0; "
        "    border: 1px solid #ccc; "
        "    border-radius: 5px; "
        "}"
    );
    
    mControlLayout = new QVBoxLayout(mControlPanel);
    mControlLayout->setContentsMargins(10, 10, 10, 10);
    mControlLayout->setSpacing(10);
    
    // 游戏信息组
    QGroupBox* infoGroup = new QGroupBox("游戏信息", mControlPanel);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoGroup);
    
    mScoreLabel = new QLabel("分数: 0", infoGroup);
    mTimeLabel = new QLabel("时间: 5:00", infoGroup);
    mLevelLabel = new QLabel("等级: 1", infoGroup);
    
    QFont labelFont;
    labelFont.setPointSize(10);
    labelFont.setBold(true);
    mScoreLabel->setFont(labelFont);
    mTimeLabel->setFont(labelFont);
    mLevelLabel->setFont(labelFont);
    
    infoLayout->addWidget(mScoreLabel);
    infoLayout->addWidget(mTimeLabel);
    infoLayout->addWidget(mLevelLabel);
    
    mControlLayout->addWidget(infoGroup);
    
    // 玩家状态组
    QGroupBox* statusGroup = new QGroupBox("玩家状态", mControlPanel);
    QVBoxLayout* statusLayout = new QVBoxLayout(statusGroup);
    
    QLabel* healthLabel = new QLabel("生命值:", statusGroup);
    mHealthBar = new QProgressBar(statusGroup);
    mHealthBar->setRange(0, 100);
    mHealthBar->setValue(100);
    mHealthBar->setStyleSheet(
        "QProgressBar { "
        "    border: 1px solid #ccc; "
        "    border-radius: 3px; "
        "    text-align: center; "
        "} "
        "QProgressBar::chunk { "
        "    background-color: #4CAF50; "
        "    border-radius: 2px; "
        "}"
    );
    
    QLabel* expLabel = new QLabel("经验值:", statusGroup);
    mExpBar = new QProgressBar(statusGroup);
    mExpBar->setRange(0, 100);
    mExpBar->setValue(0);
    mExpBar->setStyleSheet(
        "QProgressBar { "
        "    border: 1px solid #ccc; "
        "    border-radius: 3px; "
        "    text-align: center; "
        "} "
        "QProgressBar::chunk { "
        "    background-color: #2196F3; "
        "    border-radius: 2px; "
        "}"
    );
    
    statusLayout->addWidget(healthLabel);
    statusLayout->addWidget(mHealthBar);
    statusLayout->addWidget(expLabel);
    statusLayout->addWidget(mExpBar);
    
    mControlLayout->addWidget(statusGroup);
    
    // 控制按钮组
    QGroupBox* controlGroup = new QGroupBox("游戏控制", mControlPanel);
    QVBoxLayout* controlLayout = new QVBoxLayout(controlGroup);
    
    mStartButton = new QPushButton("开始游戏", controlGroup);
    mPauseButton = new QPushButton("暂停游戏", controlGroup);
    mResumeButton = new QPushButton("继续游戏", controlGroup);
    mStopButton = new QPushButton("停止游戏", controlGroup);
    mResetButton = new QPushButton("重置游戏", controlGroup);
    mExitButton = new QPushButton("退出游戏", controlGroup);
    
    // 设置按钮样式
    QString buttonStyle = 
        "QPushButton { "
        "    background-color: #4CAF50; "
        "    border: none; "
        "    color: white; "
        "    padding: 8px 16px; "
        "    text-align: center; "
        "    font-size: 12px; "
        "    border-radius: 4px; "
        "} "
        "QPushButton:hover { "
        "    background-color: #45a049; "
        "} "
        "QPushButton:pressed { "
        "    background-color: #3d8b40; "
        "} "
        "QPushButton:disabled { "
        "    background-color: #cccccc; "
        "    color: #666666; "
        "}";
    
    mStartButton->setStyleSheet(buttonStyle);
    mPauseButton->setStyleSheet(buttonStyle.replace("#4CAF50", "#FF9800").replace("#45a049", "#e68900").replace("#3d8b40", "#cc7a00"));
    mResumeButton->setStyleSheet(buttonStyle.replace("#FF9800", "#4CAF50").replace("#e68900", "#45a049").replace("#cc7a00", "#3d8b40"));
    mStopButton->setStyleSheet(buttonStyle.replace("#4CAF50", "#f44336").replace("#45a049", "#da190b").replace("#3d8b40", "#b71c1c"));
    mResetButton->setStyleSheet(buttonStyle.replace("#f44336", "#2196F3").replace("#da190b", "#0b7dda").replace("#b71c1c", "#0d47a1"));
    mExitButton->setStyleSheet(buttonStyle.replace("#2196F3", "#9E9E9E").replace("#0b7dda", "#757575").replace("#0d47a1", "#424242"));
    
    controlLayout->addWidget(mStartButton);
    controlLayout->addWidget(mPauseButton);
    controlLayout->addWidget(mResumeButton);
    controlLayout->addWidget(mStopButton);
    controlLayout->addWidget(mResetButton);
    controlLayout->addWidget(mExitButton);
    
    mControlLayout->addWidget(controlGroup);
    
    // 添加弹性空间
    mControlLayout->addStretch();
    
    // 操作说明
    QGroupBox* helpGroup = new QGroupBox("操作说明", mControlPanel);
    QVBoxLayout* helpLayout = new QVBoxLayout(helpGroup);
    
    QLabel* helpText = new QLabel(
        "WASD/方向键: 移动\n"
        "鼠标左键: 射击\n"
        "空格键: 暂停/继续\n"
        "ESC键: 停止游戏\n"
        "F5键: 重置游戏",
        helpGroup
    );
    helpText->setWordWrap(true);
    helpText->setStyleSheet("font-size: 9px; color: #666;");
    
    helpLayout->addWidget(helpText);
    mControlLayout->addWidget(helpGroup);
}

void SugarOilMainWindow::setupStatusBar()
{
    mStatusBar = statusBar();
    mStatusLabel = new QLabel("就绪", this);
    mStatusBar->addWidget(mStatusLabel);
    mStatusBar->showMessage("欢迎来到吃糖吃油大冒险！");
}

void SugarOilMainWindow::connectSignals()
{
    // 连接按钮信号
    connect(mStartButton, &QPushButton::clicked, this, &SugarOilMainWindow::onStartGame);
    connect(mPauseButton, &QPushButton::clicked, this, &SugarOilMainWindow::onPauseGame);
    connect(mResumeButton, &QPushButton::clicked, this, &SugarOilMainWindow::onResumeGame);
    connect(mStopButton, &QPushButton::clicked, this, &SugarOilMainWindow::onStopGame);
    connect(mResetButton, &QPushButton::clicked, this, &SugarOilMainWindow::onResetGame);
    connect(mExitButton, &QPushButton::clicked, this, &SugarOilMainWindow::onExitGame);
    
    // 连接菜单动作
    connect(mStartAction, &QAction::triggered, this, &SugarOilMainWindow::onStartGame);
    connect(mPauseAction, &QAction::triggered, this, &SugarOilMainWindow::onPauseGame);
    connect(mResumeAction, &QAction::triggered, this, &SugarOilMainWindow::onResumeGame);
    connect(mStopAction, &QAction::triggered, this, &SugarOilMainWindow::onStopGame);
    connect(mResetAction, &QAction::triggered, this, &SugarOilMainWindow::onResetGame);
    connect(mExitAction, &QAction::triggered, this, &SugarOilMainWindow::onExitGame);
    connect(mHelpAction, &QAction::triggered, this, &SugarOilMainWindow::onShowHelp);
    connect(mAboutAction, &QAction::triggered, this, &SugarOilMainWindow::onShowAbout);
    
    // 连接游戏场景信号
    if (mGameScene) {
        connect(mGameScene, &SugarOilGameSceneNew::gameStarted, this, &SugarOilMainWindow::onGameStarted);
        connect(mGameScene, &SugarOilGameSceneNew::gamePaused, this, &SugarOilMainWindow::onGamePaused);
        connect(mGameScene, &SugarOilGameSceneNew::gameResumed, this, &SugarOilMainWindow::onGameResumed);
        connect(mGameScene, &SugarOilGameSceneNew::gameOver, this, &SugarOilMainWindow::onGameOver);
        connect(mGameScene, &SugarOilGameSceneNew::gameWon, this, &SugarOilMainWindow::onGameWon);
        connect(mGameScene, &SugarOilGameSceneNew::scoreChanged, this, &SugarOilMainWindow::onScoreChanged);
        connect(mGameScene, &SugarOilGameSceneNew::timeChanged, this, &SugarOilMainWindow::onTimeChanged);
        connect(mGameScene, &SugarOilGameSceneNew::playerStatsChanged, this, &SugarOilMainWindow::onPlayerStatsChanged);
    }
}

void SugarOilMainWindow::updateGameControls()
{
    bool gameRunning = mGameScene && mGameScene->isGameRunning();
    bool gamePaused = mGameScene && mGameScene->isGamePaused();
    
    mStartButton->setEnabled(!gameRunning);
    mStartAction->setEnabled(!gameRunning);
    
    mPauseButton->setEnabled(gameRunning && !gamePaused);
    mPauseAction->setEnabled(gameRunning && !gamePaused);
    
    mResumeButton->setEnabled(gameRunning && gamePaused);
    mResumeAction->setEnabled(gameRunning && gamePaused);
    
    mStopButton->setEnabled(gameRunning);
    mStopAction->setEnabled(gameRunning);
    
    mResetButton->setEnabled(!gameRunning);
    mResetAction->setEnabled(!gameRunning);
}

QPointF SugarOilMainWindow::mapToScene(const QPoint &viewPos)
{
    if (mGameView) {
        return mGameView->mapToScene(mGameView->mapFromGlobal(mapToGlobal(viewPos)));
    }
    return QPointF();
}

void SugarOilMainWindow::keyPressEvent(QKeyEvent *event)
{
    if (mGameScene) {
        mGameScene->handleKeyPress(event->key());
        
        // 处理特殊快捷键
        switch (event->key()) {
        case Qt::Key_Space:
            if (mGameScene->isGameRunning()) {
                if (mGameScene->isGamePaused()) {
                    onResumeGame();
                } else {
                    onPauseGame();
                }
            }
            break;
        case Qt::Key_Escape:
            if (mGameScene->isGameRunning()) {
                onStopGame();
            }
            break;
        case Qt::Key_F5:
            if (!mGameScene->isGameRunning()) {
                onResetGame();
            }
            break;
        }
    }
    
    QMainWindow::keyPressEvent(event);
}

void SugarOilMainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (mGameScene) {
        mGameScene->handleKeyRelease(event->key());
    }
    
    QMainWindow::keyReleaseEvent(event);
}

void SugarOilMainWindow::mousePressEvent(QMouseEvent *event)
{
    if (mGameScene && event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        mGameScene->handleMousePress(scenePos);
    }
    
    QMainWindow::mousePressEvent(event);
}

void SugarOilMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mGameScene) {
        QPointF scenePos = mapToScene(event->pos());
        mGameScene->handleMouseMove(scenePos);
    }
    
    QMainWindow::mouseMoveEvent(event);
}

void SugarOilMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (mGameScene && event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        mGameScene->handleMouseRelease(scenePos);
    }
    
    QMainWindow::mouseReleaseEvent(event);
}

void SugarOilMainWindow::closeEvent(QCloseEvent *event)
{
    if (mGameScene && mGameScene->isGameRunning()) {
        int ret = QMessageBox::question(this, "退出确认", 
                                       "游戏正在进行中，确定要退出吗？",
                                       QMessageBox::Yes | QMessageBox::No,
                                       QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
        
        mGameScene->stopGame();
    }
    
    event->accept();
}

void SugarOilMainWindow::onStartGame()
{
    if (mGameScene) {
        mGameScene->startGame();
    }
}

void SugarOilMainWindow::onPauseGame()
{
    if (mGameScene) {
        mGameScene->pauseGame();
    }
}

void SugarOilMainWindow::onResumeGame()
{
    if (mGameScene) {
        mGameScene->resumeGame();
    }
}

void SugarOilMainWindow::onStopGame()
{
    if (mGameScene) {
        mGameScene->stopGame();
    }
}

void SugarOilMainWindow::onResetGame()
{
    if (mGameScene) {
        mGameScene->resetGame();
        mStatusLabel->setText("游戏已重置");
    }
}

void SugarOilMainWindow::onExitGame()
{
    close();
}

void SugarOilMainWindow::onShowHelp()
{
    QMessageBox::information(this, "游戏帮助",
        "<h3>吃糖吃油大冒险 - 模式2</h3>"
        "<p><b>游戏目标：</b>在5分钟内生存并获得尽可能高的分数</p>"
        "<p><b>操作方法：</b></p>"
        "<ul>"
        "<li>WASD键或方向键：移动角色</li>"
        "<li>鼠标左键：朝鼠标方向射击</li>"
        "<li>空格键：暂停/继续游戏</li>"
        "<li>ESC键：停止游戏</li>"
        "<li>F5键：重置游戏</li>"
        "</ul>"
        "<p><b>游戏提示：</b></p>"
        "<ul>"
        "<li>击败敌人可以获得经验值和分数</li>"
        "<li>升级可以提升角色属性</li>"
        "<li>避免与敌人碰撞以免受伤</li>"
        "<li>随着时间推移，敌人会变得更强</li>"
        "</ul>"
    );
}

void SugarOilMainWindow::onShowAbout()
{
    QMessageBox::about(this, "关于",
        "<h3>吃糖吃油大冒险 - 模式2</h3>"
        "<p>版本：1.0</p>"
        "<p>一个有趣的射击生存游戏</p>"
        "<p>使用Qt框架开发</p>"
        "<p>© 2024 游戏开发团队</p>"
    );
}

void SugarOilMainWindow::onGameStarted()
{
    mGameActive = true;
    updateGameControls();
    mStatusLabel->setText("游戏进行中");
    setFocus(); // 确保窗口获得焦点以接收键盘事件
}

void SugarOilMainWindow::onGamePaused()
{
    updateGameControls();
    mStatusLabel->setText("游戏已暂停");
}

void SugarOilMainWindow::onGameResumed()
{
    updateGameControls();
    mStatusLabel->setText("游戏进行中");
    setFocus();
}

void SugarOilMainWindow::onGameOver(int finalScore, int finalLevel)
{
    mGameActive = false;
    updateGameControls();
    mStatusLabel->setText("游戏结束");
    
    showGameOverDialog("游戏结束", "很遗憾，你没能坚持到最后！", finalScore, finalLevel);
}

void SugarOilMainWindow::onGameWon(int finalScore, int finalLevel)
{
    mGameActive = false;
    updateGameControls();
    mStatusLabel->setText("游戏胜利");
    
    showGameOverDialog("恭喜胜利", "太棒了！你成功坚持了5分钟！", finalScore, finalLevel);
}

void SugarOilMainWindow::onScoreChanged(int score)
{
    mScoreLabel->setText(QString("分数: %1").arg(score));
}

void SugarOilMainWindow::onTimeChanged(int timeLeft)
{
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    mTimeLabel->setText(QString("时间: %1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0')));
}

void SugarOilMainWindow::onPlayerStatsChanged(int hp, int maxHp, int level, int exp)
{
    mLevelLabel->setText(QString("等级: %1").arg(level));
    
    if (maxHp > 0) {
        mHealthBar->setMaximum(maxHp);
        mHealthBar->setValue(hp);
        mHealthBar->setFormat(QString("%1/%2").arg(hp).arg(maxHp));
    }
    
    // 计算经验值百分比（假设每级需要100经验）
    int expForCurrentLevel = exp % 100;
    mExpBar->setValue(expForCurrentLevel);
    mExpBar->setFormat(QString("%1/100").arg(expForCurrentLevel));
}

void SugarOilMainWindow::showGameOverDialog(const QString &title, const QString &message, int score, int level)
{
    QString fullMessage = QString("%1\n\n最终分数: %2\n最终等级: %3\n\n是否要重新开始游戏？")
                         .arg(message).arg(score).arg(level);
    
    int ret = QMessageBox::question(this, title, fullMessage,
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::Yes);
    
    if (ret == QMessageBox::Yes) {
        onResetGame();
        QTimer::singleShot(100, this, &SugarOilMainWindow::onStartGame);
    }
}