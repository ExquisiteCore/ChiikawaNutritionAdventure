#include "carbohydrate_game_scene.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QDebug>
#include <QApplication>

CarbohydrateGameScene::CarbohydrateGameScene(QObject *parent)
    : QGraphicsScene(parent)
    , gameMap(nullptr)
    , player(nullptr)
    , boss(nullptr)
    , currentState(GAME_READY)
    , gameTimeRemaining(300) // 300秒游戏时间
    , fiberValueLabel(nullptr)
    , bossHealthBar(nullptr)
    , gameStatusLabel(nullptr)
    , timeLabel(nullptr)
    , pauseButton(nullptr)
    , resumeButton(nullptr)
    , uiWidget(nullptr)
    , backgroundMusicPlayer(nullptr)
    , musicAudioOutput(nullptr)
    , soundEffect(nullptr)
{
    // 设置场景大小
    setSceneRect(0, 0, GAME_SCENE_WIDTH, GAME_SCENE_HEIGHT);
    
    // 加载背景图片
    backgroundPixmap = QPixmap(":/img/GameBackground.png");
    if (backgroundPixmap.isNull()) {
        // 创建默认背景
        backgroundPixmap = QPixmap(GAME_SCENE_WIDTH, GAME_SCENE_HEIGHT);
        backgroundPixmap.fill(QColor(20, 20, 40));
    }
    
    // 创建游戏定时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &CarbohydrateGameScene::updateGame);
    
    // 创建倒计时定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &CarbohydrateGameScene::updateCountdown);
    
    // 初始化音频系统
    backgroundMusicPlayer = new QMediaPlayer(this);
    
    // Qt 5/6兼容的音频输出设置
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        musicAudioOutput = new QAudioOutput(this);
        backgroundMusicPlayer->setAudioOutput(musicAudioOutput);
    #else
        musicAudioOutput = nullptr; // Qt 5中不需要单独的QAudioOutput
    #endif
    
    soundEffect = new QSoundEffect(this);
    
    // 初始化音频路径映射
    soundPaths["start_bgm"] = "qrc:/Sounds/Main_sound.wav"; // 游戏开始前的背景音乐
    soundPaths["background"] = "qrc:/Sounds/Game_sound.wav"; // 游戏进行中的背景音乐
    soundPaths["collect"] = "qrc:/Sounds/Bean_sound_short.wav";
    soundPaths["attack"] = "qrc:/Sounds/TapButton.wav";
    soundPaths["win"] = "qrc:/Sounds/Win.wav";
    soundPaths["lose"] = "qrc:/Sounds/Lose.wav";
    soundPaths["boss_hit"] = "qrc:/Sounds/Cough_sound.wav";
    soundPaths["double_kill"] = "qrc:/Sounds/Double_Kill.wav";
    soundPaths["triple_kill"] = "qrc:/Sounds/Triple_Kill.wav";
    soundPaths["dominating"] = "qrc:/Sounds/Dominating.wav";
    soundPaths["win_bgm"] = "qrc:/Sounds/Win.wav"; // 胜利背景音乐
    soundPaths["lose_bgm"] = "qrc:/Sounds/Lose_1.wav"; // 失败背景音乐
    
    // 设置默认音量
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (musicAudioOutput) {
            musicAudioOutput->setVolume(0.3f);
        }
    #else
        backgroundMusicPlayer->setVolume(30); // Qt 5使用0-100的音量范围
    #endif
    soundEffect->setVolume(0.5f);
    
    // 初始化游戏
    initializeGame();
    
    // 播放游戏开始前的背景音乐
    playStartBackgroundMusic();
}

CarbohydrateGameScene::~CarbohydrateGameScene()
{
    cleanupFiberSwords();
}

void CarbohydrateGameScene::initializeGame()
{
    // 清理现有对象
    clear();
    cleanupFiberSwords();
    
    // 创建游戏地图
    gameMap = new GameMap(this);
    
    // 绘制地图
    drawMap();
    drawFakeVegetables();
    
    // 创建玩家
    player = new Player(gameMap, this);
    addItem(player);
    
    // 连接玩家信号
    connect(player, &Player::fiberSwordUsed, this, &CarbohydrateGameScene::onFiberSwordUsed);
    connect(player, &Player::fiberValueChanged, this, &CarbohydrateGameScene::onFiberValueChanged);
    connect(player, &Player::fakeVegetableCollected, this, &CarbohydrateGameScene::onFakeVegetableCollected);
    
    // 创建BOSS
    boss = new FakeVegetableBoss(gameMap, this);
    addItem(boss);
    
    // 连接BOSS信号
    connect(boss, &FakeVegetableBoss::bossDefeated, this, &CarbohydrateGameScene::onBossDefeated);
    connect(boss, &FakeVegetableBoss::playerCaught, this, &CarbohydrateGameScene::onPlayerCaught);
    connect(boss, &FakeVegetableBoss::healthChanged, this, &CarbohydrateGameScene::onBossHealthChanged);
    
    // 创建UI
    createUI();
    
    // 设置初始状态
    currentState = GAME_READY;
    updateUI();
}

void CarbohydrateGameScene::createUI()
{
    // 创建UI容器
    QWidget* uiContainer = new QWidget();
    uiContainer->setStyleSheet("background: rgba(0, 0, 0, 120); border-radius: 8px; padding: 8px;");
    uiContainer->setFixedWidth(200);  // 固定宽度，避免过度占用空间
    
    QVBoxLayout* mainLayout = new QVBoxLayout(uiContainer);
    mainLayout->setSpacing(5);  // 减少间距，使UI更紧凑
    
    // 游戏状态标签
    gameStatusLabel = new QLabel("准备开始游戏");
    gameStatusLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    gameStatusLabel->setAlignment(Qt::AlignCenter);
    gameStatusLabel->setWordWrap(true);  // 允许文字换行
    mainLayout->addWidget(gameStatusLabel);
    
    // 时间显示
    QHBoxLayout* timeLayout = new QHBoxLayout();
    QLabel* timeTextLabel = new QLabel("剩余时间:");
    timeTextLabel->setStyleSheet("color: white; font-size: 12px;");
    timeLabel = new QLabel("05:00");
    timeLabel->setStyleSheet("color: yellow; font-size: 14px; font-weight: bold;");
    timeLayout->addWidget(timeTextLabel);
    timeLayout->addWidget(timeLabel);
    timeLayout->addStretch();
    mainLayout->addLayout(timeLayout);
    
    // 纤维值显示
    QHBoxLayout* fiberLayout = new QHBoxLayout();
    QLabel* fiberLabel = new QLabel("纤维值:");
    fiberLabel->setStyleSheet("color: white; font-size: 12px;");
    fiberValueLabel = new QLabel(QString::number(INITIAL_FIBER_VALUE));
    fiberValueLabel->setStyleSheet("color: lime; font-size: 12px; font-weight: bold;");
    fiberLayout->addWidget(fiberLabel);
    fiberLayout->addWidget(fiberValueLabel);
    fiberLayout->addStretch();
    mainLayout->addLayout(fiberLayout);
    
    // BOSS血量条
    QLabel* bossLabel = new QLabel("BOSS血量:");
    bossLabel->setStyleSheet("color: white; font-size: 12px;");
    mainLayout->addWidget(bossLabel);
    
    bossHealthBar = new QProgressBar();
    bossHealthBar->setRange(0, BOSS_HEALTH);
    bossHealthBar->setValue(BOSS_HEALTH);
    bossHealthBar->setFixedHeight(20);  // 固定高度，使其更紧凑
    bossHealthBar->setStyleSheet(
        "QProgressBar { border: 1px solid grey; border-radius: 3px; text-align: center; font-size: 10px; }"
        "QProgressBar::chunk { background-color: red; }"
    );
    mainLayout->addWidget(bossHealthBar);
    
    // 控制按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    pauseButton = new QPushButton("暂停");
    pauseButton->setFixedHeight(25);  // 固定按钮高度
    pauseButton->setStyleSheet("QPushButton { background-color: orange; color: white; font-weight: bold; padding: 3px; border-radius: 3px; font-size: 11px; }");
    connect(pauseButton, &QPushButton::clicked, this, &CarbohydrateGameScene::onPauseButtonClicked);
    
    resumeButton = new QPushButton("继续");
    resumeButton->setFixedHeight(25);  // 固定按钮高度
    resumeButton->setStyleSheet("QPushButton { background-color: green; color: white; font-weight: bold; padding: 3px; border-radius: 3px; font-size: 11px; }");
    resumeButton->setVisible(false);
    connect(resumeButton, &QPushButton::clicked, this, &CarbohydrateGameScene::onResumeButtonClicked);
    
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resumeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 操作说明
    QLabel* instructionLabel = new QLabel("WASD/方向键移动\n空格键发射纤维剑");
    instructionLabel->setStyleSheet("color: lightgray; font-size: 10px;");
    instructionLabel->setAlignment(Qt::AlignCenter);
    instructionLabel->setWordWrap(true);
    mainLayout->addWidget(instructionLabel);
    
    // 添加UI到场景 - 放在游戏场景内部右上角
    uiWidget = addWidget(uiContainer);
    uiWidget->setPos(GAME_SCENE_WIDTH - 220, 10);  // 放在游戏场景内部右上角，避免与控制面板重叠
    uiWidget->setZValue(100);
}

void CarbohydrateGameScene::drawMap()
{
    // 清理现有墙体
    for (auto* item : wallItems) {
        removeItem(item);
        delete item;
    }
    wallItems.clear();
    
    // 绘制墙体
    for (int row = 0; row < gameMap->getRows(); ++row) {
        for (int col = 0; col < gameMap->getCols(); ++col) {
            if (gameMap->isWall(row, col)) {
                QGraphicsPixmapItem* wallItem = new QGraphicsPixmapItem();
                
                // 创建墙体图片
                QPixmap wallPixmap(CELL_SIZE, CELL_SIZE);
                wallPixmap.fill(QColor(100, 50, 0)); // 棕色墙体
                
                wallItem->setPixmap(wallPixmap);
                QPointF pos = gameMap->cellToPixel(row, col);
                wallItem->setPos(pos.x() - CELL_SIZE/2, pos.y() - CELL_SIZE/2);
                
                addItem(wallItem);
                wallItems.append(wallItem);
            }
        }
    }
}

void CarbohydrateGameScene::drawFakeVegetables()
{
    // 清理现有假蔬菜
    for (auto* item : fakeVegetableItems) {
        removeItem(item);
        delete item;
    }
    fakeVegetableItems.clear();
    
    // 绘制假蔬菜
    for (int row = 0; row < gameMap->getRows(); ++row) {
        for (int col = 0; col < gameMap->getCols(); ++col) {
            if (gameMap->hasFakeVegetable(row, col)) {
                QGraphicsPixmapItem* vegItem = new QGraphicsPixmapItem();
                
                // 创建假蔬菜图片
                QPixmap vegPixmap(FAKE_VEGETABLE_SIZE, FAKE_VEGETABLE_SIZE);
                vegPixmap.fill(QColor(255, 255, 0)); // 黄色假蔬菜
                
                vegItem->setPixmap(vegPixmap);
                QPointF pos = gameMap->cellToPixel(row, col);
                vegItem->setPos(pos.x() - FAKE_VEGETABLE_SIZE/2, pos.y() - FAKE_VEGETABLE_SIZE/2);
                
                addItem(vegItem);
                fakeVegetableItems.append(vegItem);
            }
        }
    }
}

void CarbohydrateGameScene::startGame()
{
    if (currentState == GAME_READY || currentState == GAME_PAUSED) {
        // 重置游戏时间（仅在新游戏开始时）
        if (currentState == GAME_READY) {
            gameTimeRemaining = 300;
        }
        
        currentState = GAME_RUNNING;
        gameTimer->start(16); // 约60FPS
        
        // 启动倒计时定时器
        countdownTimer->start(1000); // 每秒更新一次
        
        if (boss) {
            boss->startMovement();
        }
        
        // 停止开始前音乐，播放游戏背景音乐
        stopBackgroundMusic();
        playBackgroundMusic();
        
        updateUI();
        emit gameStateChanged(currentState);
    }
}

void CarbohydrateGameScene::pauseGame()
{
    if (currentState == GAME_RUNNING) {
        currentState = GAME_PAUSED;
        gameTimer->stop();
        countdownTimer->stop();
        
        if (player) {
            player->pauseAnimation();
        }
        if (boss) {
            boss->pauseMovement();
        }
        
        updateUI();
        emit gameStateChanged(currentState);
    }
}

void CarbohydrateGameScene::resumeGame()
{
    if (currentState == GAME_PAUSED) {
        currentState = GAME_RUNNING;
        gameTimer->start();
        countdownTimer->start();
        
        if (player) {
            player->resumeAnimation();
        }
        if (boss) {
            boss->resumeMovement();
        }
        
        updateUI();
        emit gameStateChanged(currentState);
    }
}

void CarbohydrateGameScene::endGame(bool won)
{
    gameTimer->stop();
    countdownTimer->stop();
    
    if (player) {
        player->pauseAnimation();
    }
    if (boss) {
        boss->stopMovement();
    }
    
    // 停止背景音乐
    stopBackgroundMusic();
    
    // 播放结束音乐（较长的背景音乐而非短音效）
    if (won) {
        playEndMusic("win_bgm");
    } else {
        playEndMusic("lose_bgm");
    }
    
    // 同时播放短音效
    if (won) {
        playSound("win");
    } else {
        playSound("lose");
    }
    
    cleanupFiberSwords();
}

void CarbohydrateGameScene::updateGame()
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
    // 更新BOSS目标位置
    if (boss && player) {
        boss->setTargetPosition(player->getCurrentCell());
    }
    
    // 检查碰撞
    checkCollisions();
    
    // 更新假蔬菜显示
    drawFakeVegetables();
}

void CarbohydrateGameScene::updateCountdown()
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
    gameTimeRemaining--;
    
    // 更新时间显示
    if (timeLabel) {
        int minutes = gameTimeRemaining / 60;
        int seconds = gameTimeRemaining % 60;
        QString timeText = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
        timeLabel->setText(timeText);
        
        // 时间不足30秒时变红色警告
        if (gameTimeRemaining <= 30) {
            timeLabel->setStyleSheet("color: red; font-size: 14px; font-weight: bold;");
        } else if (gameTimeRemaining <= 60) {
            timeLabel->setStyleSheet("color: orange; font-size: 14px; font-weight: bold;");
        }
    }
    
    // 检查是否时间到达胜利条件
    if (gameTimeRemaining <= 0) {
        onTimeUp();
    }
}

void CarbohydrateGameScene::onTimeUp()
{
    currentState = GAME_WIN;
    
    // 播放特殊胜利音效（坚持到最后）
    playSound("dominating");
    
    // 延迟播放胜利背景音乐
    QTimer::singleShot(2000, this, [this]() {
        playEndMusic("win_bgm");
    });
    
    endGame(true);
    updateUI();
    emit gameWon();
}

void CarbohydrateGameScene::checkCollisions()
{
    if (!player || !boss) {
        return;
    }
    
    // 检查玩家与BOSS的碰撞
    QPointF playerPos = player->pos();
    QPointF bossPos = boss->pos();
    qreal distance = QLineF(playerPos, bossPos).length();
    
    if (distance < CARBOHYDRATE_COLLISION_DISTANCE) {
        onPlayerCaught();
    }
}

void CarbohydrateGameScene::onFiberSwordUsed(QPointF position, Direction direction)
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
    // 播放攻击音效
    playSound("attack");
    
    FiberSword* sword = new FiberSword(position, direction, gameMap, this);
    connect(sword, &FiberSword::hitTarget, this, &CarbohydrateGameScene::onFiberSwordHit);
    connect(sword, &FiberSword::swordDestroyed, this, &CarbohydrateGameScene::onFiberSwordDestroyed);
    
    addItem(sword);
    fiberSwords.append(sword);
    sword->startMovement();
}

void CarbohydrateGameScene::onFiberSwordHit(QGraphicsItem* target)
{
    if (target && target->type() == TYPE_BOSS) {
        FakeVegetableBoss* hitBoss = static_cast<FakeVegetableBoss*>(target);
        hitBoss->takeDamage(FIBER_SWORD_DAMAGE);
        
        // 播放Boss受击音效
        playSound("boss_hit");
        
        // 检查连击数并播放相应音效
        static int hitCount = 0;
        static QTimer* resetTimer = nullptr;
        
        hitCount++;
        
        // 重置连击计数器（3秒后重置）
        if (resetTimer) {
            resetTimer->stop();
            delete resetTimer;
        }
        resetTimer = new QTimer();
        resetTimer->setSingleShot(true);
        connect(resetTimer, &QTimer::timeout, []() {
            hitCount = 0;
        });
        resetTimer->start(3000);
        
        // 根据连击数播放特殊音效
        if (hitCount == 2) {
            QTimer::singleShot(500, this, [this]() {
                playSound("double_kill");
            });
        } else if (hitCount == 3) {
            QTimer::singleShot(500, this, [this]() {
                playSound("triple_kill");
            });
        } else if (hitCount >= 5) {
            QTimer::singleShot(500, this, [this]() {
                playSound("dominating");
            });
        }
    }
}

void CarbohydrateGameScene::onFiberSwordDestroyed()
{
    FiberSword* sword = qobject_cast<FiberSword*>(sender());
    if (sword) {
        fiberSwords.removeOne(sword);
        removeItem(sword);
        sword->deleteLater();
    }
}

void CarbohydrateGameScene::onBossDefeated()
{
    currentState = GAME_WIN;
    
    // 播放特殊胜利音效（主宰）
    playSound("dominating");
    
    // 延迟播放胜利背景音乐
    QTimer::singleShot(2000, this, [this]() {
        playEndMusic("win_bgm");
    });
    
    endGame(true);
    updateUI();
    emit gameWon();
}

void CarbohydrateGameScene::onPlayerCaught()
{
    currentState = GAME_LOSE;
    endGame(false);
    updateUI();
    emit gameLost();
}

void CarbohydrateGameScene::onFiberValueChanged(int newValue)
{
    if (fiberValueLabel) {
        fiberValueLabel->setText(QString::number(newValue));
    }
}

void CarbohydrateGameScene::onFakeVegetableCollected()
{
    // 播放收集音效
    playSound("collect");
    
    // 更新假蔬菜显示
    drawFakeVegetables();
}

void CarbohydrateGameScene::onBossHealthChanged(int newHealth)
{
    if (bossHealthBar) {
        bossHealthBar->setValue(newHealth);
    }
}

void CarbohydrateGameScene::onPauseButtonClicked()
{
    pauseGame();
}

void CarbohydrateGameScene::onResumeButtonClicked()
{
    resumeGame();
}

void CarbohydrateGameScene::updateUI()
{
    if (!gameStatusLabel || !pauseButton || !resumeButton) {
        return;
    }
    
    switch (currentState) {
    case GAME_READY:
        gameStatusLabel->setText("按任意键开始游戏");
        pauseButton->setVisible(false);
        resumeButton->setVisible(false);
        break;
    case GAME_RUNNING:
        gameStatusLabel->setText("击败伪蔬菜BOSS!");
        pauseButton->setVisible(true);
        resumeButton->setVisible(false);
        break;
    case GAME_PAUSED:
        gameStatusLabel->setText("游戏暂停");
        pauseButton->setVisible(false);
        resumeButton->setVisible(true);
        break;
    case GAME_WIN:
        if (gameTimeRemaining <= 0) {
            gameStatusLabel->setText("胜利！坚持300秒成功！\n成为贵州版彭于晏！");
        } else {
            gameStatusLabel->setText("胜利！BOSS被击败了！");
        }
        pauseButton->setVisible(false);
        resumeButton->setVisible(false);
        break;
    case GAME_LOSE:
        gameStatusLabel->setText("失败！被BOSS抓住了！");
        pauseButton->setVisible(false);
        resumeButton->setVisible(false);
        break;
    }
}

void CarbohydrateGameScene::cleanupFiberSwords()
{
    for (FiberSword* sword : fiberSwords) {
        removeItem(sword);
        sword->deleteLater();
    }
    fiberSwords.clear();
}

void CarbohydrateGameScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->drawPixmap(rect.toRect(), backgroundPixmap, rect.toRect());
}

void CarbohydrateGameScene::handleKeyPress(QKeyEvent *event)
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
    if (player) {
        player->handleKeyPress(event->key());
    }
}

void CarbohydrateGameScene::handleKeyRelease(QKeyEvent *event)
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
    if (player) {
        player->handleKeyRelease(event->key());
    }
}

void CarbohydrateGameScene::keyPressEvent(QKeyEvent *event)
{
    if (currentState == GAME_READY) {
        startGame();
        return;
    }
    
    handleKeyPress(event);
    QGraphicsScene::keyPressEvent(event);
}

void CarbohydrateGameScene::keyReleaseEvent(QKeyEvent *event)
{
    handleKeyRelease(event);
    QGraphicsScene::keyReleaseEvent(event);
}

// CarbohydrateGameView 实现
CarbohydrateGameView::CarbohydrateGameView(CarbohydrateGameScene* scene, QWidget *parent)
    : QGraphicsView(scene, parent), gameScene(scene)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::NoDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::StrongFocus);
    
    // 设置视图大小
    setFixedSize(GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
}

void CarbohydrateGameView::keyPressEvent(QKeyEvent *event)
{
    if (gameScene) {
        gameScene->handleKeyPress(event);
    }
    QGraphicsView::keyPressEvent(event);
}

void CarbohydrateGameView::keyReleaseEvent(QKeyEvent *event)
{
    if (gameScene) {
        gameScene->handleKeyRelease(event);
    }
    QGraphicsView::keyReleaseEvent(event);
}

void CarbohydrateGameView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (scene()) {
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    }
}

// 音频控制方法实现
void CarbohydrateGameScene::playBackgroundMusic()
{
    if (backgroundMusicPlayer && soundPaths.contains("background")) {
        // 先停止当前音乐并清理连接
        stopBackgroundMusic();
        
        // Qt 5/6兼容的媒体设置
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            backgroundMusicPlayer->setSource(QUrl(soundPaths["background"]));
        #else
            backgroundMusicPlayer->setMedia(QUrl(soundPaths["background"]));
        #endif
        
        // 连接信号实现循环播放
        connect(backgroundMusicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
                backgroundMusicPlayer->setPosition(0);
                backgroundMusicPlayer->play();
            }
        });
        backgroundMusicPlayer->play();
    }
}

void CarbohydrateGameScene::playStartBackgroundMusic()
{
    if (backgroundMusicPlayer && soundPaths.contains("start_bgm")) {
        // 先停止当前音乐并清理连接
        stopBackgroundMusic();
        
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            backgroundMusicPlayer->setSource(QUrl(soundPaths["start_bgm"]));
        #else
            backgroundMusicPlayer->setMedia(QUrl(soundPaths["start_bgm"]));
        #endif
        
        // 设置循环播放
        connect(backgroundMusicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
                backgroundMusicPlayer->setPosition(0);
                backgroundMusicPlayer->play();
            }
        });
        
        backgroundMusicPlayer->play();
    }
}

void CarbohydrateGameScene::stopBackgroundMusic()
{
    if (backgroundMusicPlayer) {
        backgroundMusicPlayer->stop();
        // 断开之前的循环播放连接
        disconnect(backgroundMusicPlayer, &QMediaPlayer::mediaStatusChanged, nullptr, nullptr);
    }
}

void CarbohydrateGameScene::playEndMusic(const QString& musicName)
{
    if (backgroundMusicPlayer && soundPaths.contains(musicName)) {
        // 先停止当前音乐
        stopBackgroundMusic();
        
        // Qt 5/6兼容的媒体设置
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            backgroundMusicPlayer->setSource(QUrl(soundPaths[musicName]));
        #else
            backgroundMusicPlayer->setMedia(QUrl(soundPaths[musicName]));
        #endif
        
        // 设置循环播放（结束音乐也循环播放）
        connect(backgroundMusicPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
                backgroundMusicPlayer->setPosition(0);
                backgroundMusicPlayer->play();
            }
        });
        
        backgroundMusicPlayer->play();
    }
}

void CarbohydrateGameScene::playSound(const QString& soundName)
{
    if (soundEffect && soundPaths.contains(soundName)) {
        soundEffect->setSource(QUrl(soundPaths[soundName]));
        soundEffect->play();
    }
}

void CarbohydrateGameScene::setMusicVolume(float volume)
{
    if (musicAudioOutput) {
        musicAudioOutput->setVolume(qBound(0.0f, volume, 1.0f));
    }
}

void CarbohydrateGameScene::setSoundVolume(float volume)
{
    if (soundEffect) {
        soundEffect->setVolume(qBound(0.0f, volume, 1.0f));
    }
}