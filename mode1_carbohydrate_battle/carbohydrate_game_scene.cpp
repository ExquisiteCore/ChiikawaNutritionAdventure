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
    , fiberValueLabel(nullptr)
    , bossHealthBar(nullptr)
    , gameStatusLabel(nullptr)
    , pauseButton(nullptr)
    , resumeButton(nullptr)
    , uiWidget(nullptr)
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
    
    // 初始化游戏
    initializeGame();
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
    uiContainer->setStyleSheet("background: rgba(0, 0, 0, 150); border-radius: 10px; padding: 10px;");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(uiContainer);
    
    // 游戏状态标签
    gameStatusLabel = new QLabel("准备开始游戏");
    gameStatusLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");
    gameStatusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(gameStatusLabel);
    
    // 纤维值显示
    QHBoxLayout* fiberLayout = new QHBoxLayout();
    QLabel* fiberLabel = new QLabel("膳食纤维值:");
    fiberLabel->setStyleSheet("color: white; font-size: 14px;");
    fiberValueLabel = new QLabel(QString::number(INITIAL_FIBER_VALUE));
    fiberValueLabel->setStyleSheet("color: lime; font-size: 14px; font-weight: bold;");
    fiberLayout->addWidget(fiberLabel);
    fiberLayout->addWidget(fiberValueLabel);
    fiberLayout->addStretch();
    mainLayout->addLayout(fiberLayout);
    
    // BOSS血量条
    QLabel* bossLabel = new QLabel("伪蔬菜BOSS血量:");
    bossLabel->setStyleSheet("color: white; font-size: 14px;");
    mainLayout->addWidget(bossLabel);
    
    bossHealthBar = new QProgressBar();
    bossHealthBar->setRange(0, BOSS_HEALTH);
    bossHealthBar->setValue(BOSS_HEALTH);
    bossHealthBar->setStyleSheet(
        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; }"
        "QProgressBar::chunk { background-color: red; }"
    );
    mainLayout->addWidget(bossHealthBar);
    
    // 控制按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    pauseButton = new QPushButton("暂停");
    pauseButton->setStyleSheet("QPushButton { background-color: orange; color: white; font-weight: bold; padding: 5px; border-radius: 3px; }");
    connect(pauseButton, &QPushButton::clicked, this, &CarbohydrateGameScene::onPauseButtonClicked);
    
    resumeButton = new QPushButton("继续");
    resumeButton->setStyleSheet("QPushButton { background-color: green; color: white; font-weight: bold; padding: 5px; border-radius: 3px; }");
    resumeButton->setVisible(false);
    connect(resumeButton, &QPushButton::clicked, this, &CarbohydrateGameScene::onResumeButtonClicked);
    
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resumeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // 操作说明
    QLabel* instructionLabel = new QLabel("WASD/方向键移动\n空格键发射膳食纤维剑");
    instructionLabel->setStyleSheet("color: lightgray; font-size: 12px;");
    instructionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instructionLabel);
    
    // 添加UI到场景
    uiWidget = addWidget(uiContainer);
    uiWidget->setPos(10, 10);
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
        currentState = GAME_RUNNING;
        gameTimer->start(16); // 约60FPS
        
        if (boss) {
            boss->startMovement();
        }
        
        updateUI();
        emit gameStateChanged(currentState);
    }
}

void CarbohydrateGameScene::pauseGame()
{
    if (currentState == GAME_RUNNING) {
        currentState = GAME_PAUSED;
        gameTimer->stop();
        
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
    Q_UNUSED(won);
    gameTimer->stop();
    
    if (player) {
        player->pauseAnimation();
    }
    if (boss) {
        boss->stopMovement();
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

void CarbohydrateGameScene::checkCollisions()
{
    if (!player || !boss) {
        return;
    }
    
    // 检查玩家与BOSS的碰撞
    QPointF playerPos = player->pos();
    QPointF bossPos = boss->pos();
    qreal distance = QLineF(playerPos, bossPos).length();
    
    if (distance < COLLISION_DISTANCE) {
        onPlayerCaught();
    }
}

void CarbohydrateGameScene::onFiberSwordUsed(QPointF position, Direction direction)
{
    if (currentState != GAME_RUNNING) {
        return;
    }
    
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
        gameStatusLabel->setText("胜利！BOSS被击败了！");
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
    
    // 居中显示场景
    centerOn(GAME_SCENE_WIDTH/2, GAME_SCENE_HEIGHT/2);
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
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}