#include "sugar_oil_game_scene_new.h"
#include <QGraphicsView>
#include <QApplication>
#include <QDebug>
#include <QSet>
#include <QtMath>
#include <QUrl>

SugarOilGameSceneNew::SugarOilGameSceneNew(QObject *parent)
    : QGraphicsScene(parent)
    , mPlayer(nullptr)
    , mBackground(nullptr)
    , mGameTimer(nullptr)
    , mUpdateTimer(nullptr)
    , mSpawnTimer(nullptr)
    , mGameRunning(false)
    , mGamePaused(false)
    , mGameTime(0)
    , mSpawnCounter(0)
    , mMousePressed(false)
    , mBackgroundMusicPlayer(nullptr)
{
    initializeScene();
    initializePlayer();
    initializeTimers();
    initializeAudio();
    loadBackground();
}

SugarOilGameSceneNew::~SugarOilGameSceneNew()
{
    stopGame();
    
    // 清理所有游戏对象
    qDeleteAll(mEnemies);
    qDeleteAll(mPlayerBullets);
    qDeleteAll(mEnemyBullets);
    
    if (mPlayer) {
        delete mPlayer;
    }
}

void SugarOilGameSceneNew::initializeScene()
{
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    setBackgroundBrush(QBrush(QColor(144, 238, 144))); // 浅绿色背景
}

void SugarOilGameSceneNew::initializePlayer()
{
    mPlayer = new SugarOilPlayer();
    mPlayer->setPos(SCENE_WIDTH / 2, SCENE_HEIGHT / 2);
    addItem(mPlayer);
    
    // 连接玩家信号
    connect(mPlayer, &SugarOilPlayer::playerShoot, this, &SugarOilGameSceneNew::onPlayerShoot);
    connect(mPlayer, &SugarOilPlayer::playerDied, this, &SugarOilGameSceneNew::onPlayerDied);
    connect(mPlayer, &SugarOilPlayer::levelUp, this, &SugarOilGameSceneNew::onPlayerLevelUp);
    // connect(mPlayer, &SugarOilPlayer::statsChanged, this, &SugarOilGameSceneNew::playerStatsChanged);
}

void SugarOilGameSceneNew::initializeTimers()
{
    // 游戏时间定时器（每秒更新）
    mGameTimer = new QTimer(this);
    mGameTimer->setInterval(1000);
    connect(mGameTimer, &QTimer::timeout, this, &SugarOilGameSceneNew::onGameTimerTimeout);
    
    // 游戏更新定时器（60 FPS）
    mUpdateTimer = new QTimer(this);
    mUpdateTimer->setInterval(UPDATE_INTERVAL);
    connect(mUpdateTimer, &QTimer::timeout, this, &SugarOilGameSceneNew::updateGame);
    
    // 敌人生成定时器
    mSpawnTimer = new QTimer(this);
    mSpawnTimer->setInterval(SPAWN_INTERVAL);
    connect(mSpawnTimer, &QTimer::timeout, this, &SugarOilGameSceneNew::spawnEnemies);
}

void SugarOilGameSceneNew::initializeAudio()
{
    mBackgroundMusicPlayer = new QMediaPlayer(this);
    mBackgroundMusicAudioOutput = new QAudioOutput(this);
    mBackgroundMusicPlayer->setAudioOutput(mBackgroundMusicAudioOutput);
    mBackgroundMusicPlayer->setSource(QUrl("qrc:/sounds/background_music.mp3"));
    mBackgroundMusicAudioOutput->setVolume(0.3f); // Qt6使用0.0-1.0的音量范围
}

void SugarOilGameSceneNew::loadBackground()
{
    QPixmap backgroundPixmap(":/images/game_background.png");
    if (backgroundPixmap.isNull()) {
        // 如果没有背景图片，创建一个简单的渐变背景
        backgroundPixmap = QPixmap(SCENE_WIDTH, SCENE_HEIGHT);
        backgroundPixmap.fill(QColor(144, 238, 144));
    } else {
        backgroundPixmap = backgroundPixmap.scaled(SCENE_WIDTH, SCENE_HEIGHT, Qt::KeepAspectRatioByExpanding);
    }
    
    mBackground = addPixmap(backgroundPixmap);
    mBackground->setZValue(-1); // 确保背景在最底层
}

void SugarOilGameSceneNew::startGame()
{
    if (mGameRunning) {
        return;
    }
    
    resetGame();
    mGameRunning = true;
    mGamePaused = false;
    
    // 启动所有定时器
    mGameTimer->start();
    mUpdateTimer->start();
    mSpawnTimer->start();
    
    // 播放背景音乐
    if (mBackgroundMusicPlayer) {
        mBackgroundMusicPlayer->play();
    }
    
    emit gameStarted();
    qDebug() << "Game started!";
}

void SugarOilGameSceneNew::pauseGame()
{
    if (!mGameRunning || mGamePaused) {
        return;
    }
    
    mGamePaused = true;
    
    // 暂停所有定时器
    mGameTimer->stop();
    mUpdateTimer->stop();
    mSpawnTimer->stop();
    
    // 暂停背景音乐
    if (mBackgroundMusicPlayer) {
        mBackgroundMusicPlayer->pause();
    }
    
    // 暂停所有敌人的AI
    for (EnemyBase* enemy : mEnemies) {
        enemy->stopAI();
    }
    
    emit gamePaused();
    qDebug() << "Game paused!";
}

void SugarOilGameSceneNew::resumeGame()
{
    if (!mGameRunning || !mGamePaused) {
        return;
    }
    
    mGamePaused = false;
    
    // 恢复所有定时器
    mGameTimer->start();
    mUpdateTimer->start();
    mSpawnTimer->start();
    
    // 恢复背景音乐
    if (mBackgroundMusicPlayer) {
        mBackgroundMusicPlayer->play();
    }
    
    // 恢复所有敌人的AI
    for (EnemyBase* enemy : mEnemies) {
        enemy->startAI();
    }
    
    emit gameResumed();
    qDebug() << "Game resumed!";
}

void SugarOilGameSceneNew::stopGame()
{
    if (!mGameRunning) {
        return;
    }
    
    mGameRunning = false;
    mGamePaused = false;
    
    // 停止所有定时器
    if (mGameTimer) mGameTimer->stop();
    if (mUpdateTimer) mUpdateTimer->stop();
    if (mSpawnTimer) mSpawnTimer->stop();
    
    // 停止背景音乐
    if (mBackgroundMusicPlayer) {
        mBackgroundMusicPlayer->stop();
    }
    
    // 停止所有敌人的AI
    for (EnemyBase* enemy : mEnemies) {
        enemy->stopAI();
    }
    
    qDebug() << "Game stopped!";
}

void SugarOilGameSceneNew::resetGame()
{
    mGameTime = 0;
    mSpawnCounter = 0;
    mPressedKeys.clear();
    mMousePressed = false;
    
    // 清理所有游戏对象
    for (EnemyBase* enemy : mEnemies) {
        removeItem(enemy);
        delete enemy;
    }
    mEnemies.clear();
    
    for (BulletBase* bullet : mPlayerBullets) {
        removeItem(bullet);
        delete bullet;
    }
    mPlayerBullets.clear();
    
    for (BulletBase* bullet : mEnemyBullets) {
        removeItem(bullet);
        delete bullet;
    }
    mEnemyBullets.clear();
    
    // 重置玩家
    if (mPlayer) {
        mPlayer->resetPlayer();
        mPlayer->setPos(SCENE_WIDTH / 2, SCENE_HEIGHT / 2);
    }
    
    emit timeChanged(GAME_DURATION - mGameTime);
    emit scoreChanged(getScore());
}

int SugarOilGameSceneNew::getScore() const
{
    return mPlayer ? mPlayer->getScore() : 0;
}

int SugarOilGameSceneNew::getPlayerLevel() const
{
    return mPlayer ? mPlayer->getLevel() : 1;
}

void SugarOilGameSceneNew::handleKeyPress(int key)
{
    mPressedKeys.insert(key);
}

void SugarOilGameSceneNew::handleKeyRelease(int key)
{
    mPressedKeys.remove(key);
}

void SugarOilGameSceneNew::handleMousePress(const QPointF &scenePos)
{
    mLastMousePos = scenePos;
    mMousePressed = true;
    
    // 玩家朝鼠标方向射击
    if (mPlayer && mGameRunning && !mGamePaused) {
        QPointF playerPos = mPlayer->getCenterPos();
        QPointF direction = scenePos - playerPos;
        qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0) {
            direction /= length; // 标准化方向向量
            mPlayer->shoot(direction);
        }
    }
}

void SugarOilGameSceneNew::handleMouseMove(const QPointF &scenePos)
{
    mLastMousePos = scenePos;
}

void SugarOilGameSceneNew::handleMouseRelease(const QPointF &scenePos)
{
    mMousePressed = false;
}

void SugarOilGameSceneNew::onGameTimerTimeout()
{
    mGameTime++;
    emit timeChanged(GAME_DURATION - mGameTime);
    
    // 检查游戏是否结束
    if (mGameTime >= GAME_DURATION) {
        stopGame();
        emit gameWon(getScore(), getPlayerLevel());
    }
}

void SugarOilGameSceneNew::updateGame()
{
    if (!mGameRunning || mGamePaused) {
        return;
    }
    
    updatePlayerMovement();
    updateCollisions();
    cleanupObjects();
}

void SugarOilGameSceneNew::updatePlayerMovement()
{
    if (!mPlayer) {
        return;
    }
    
    QPointF movement(0, 0);
    const qreal speed = mPlayer->getSpeed();
    
    // 处理键盘输入
    if (mPressedKeys.contains(Qt::Key_W) || mPressedKeys.contains(Qt::Key_Up)) {
        movement.setY(-speed);
    }
    if (mPressedKeys.contains(Qt::Key_S) || mPressedKeys.contains(Qt::Key_Down)) {
        movement.setY(speed);
    }
    if (mPressedKeys.contains(Qt::Key_A) || mPressedKeys.contains(Qt::Key_Left)) {
        movement.setX(-speed);
    }
    if (mPressedKeys.contains(Qt::Key_D) || mPressedKeys.contains(Qt::Key_Right)) {
        movement.setX(speed);
    }
    
    // 应用移动
    if (movement.x() != 0 || movement.y() != 0) {
        mPlayer->setPos(mPlayer->pos() + movement);
        
        // 确保玩家不会移出场景边界
        QPointF newPos = mPlayer->pos();
        QRectF playerRect = mPlayer->boundingRect();
        
        if (newPos.x() < 0) newPos.setX(0);
        if (newPos.y() < 0) newPos.setY(0);
        if (newPos.x() + playerRect.width() > SCENE_WIDTH) {
            newPos.setX(SCENE_WIDTH - playerRect.width());
        }
        if (newPos.y() + playerRect.height() > SCENE_HEIGHT) {
            newPos.setY(SCENE_HEIGHT - playerRect.height());
        }
        
        mPlayer->setPos(newPos);
    }
}

void SugarOilGameSceneNew::spawnEnemies()
{
    if (!mGameRunning || mGamePaused) {
        return;
    }
    
    updateEnemySpawning();
}

void SugarOilGameSceneNew::updateEnemySpawning()
{
    // 根据游戏时间调整敌人生成
    int enemyCount = 1;
    EnemyBase::EnemyType enemyType = EnemyBase::EnemyType::SmallEnemy;
    
    if (mGameTime < 60) {
        // 前1分钟：只生成普通敌人
        enemyCount = 1;
        enemyType = EnemyBase::EnemyType::SmallEnemy;
    } else if (mGameTime < 120) {
        // 1-2分钟：增加生成数量
        enemyCount = QRandomGenerator::global()->bounded(1, 3);
        enemyType = EnemyBase::EnemyType::SmallEnemy;
    } else if (mGameTime < 180) {
        // 2-3分钟：引入快速敌人
        enemyCount = QRandomGenerator::global()->bounded(1, 3);
        enemyType = QRandomGenerator::global()->bounded(2) == 0 ? 
                   EnemyBase::EnemyType::SmallEnemy : EnemyBase::EnemyType::MediumEnemy;
    } else if (mGameTime < 240) {
        // 3-4分钟：引入强壮敌人
        enemyCount = QRandomGenerator::global()->bounded(2, 4);
        int typeRand = QRandomGenerator::global()->bounded(3);
        if (typeRand == 0) enemyType = EnemyBase::EnemyType::SmallEnemy;
        else if (typeRand == 1) enemyType = EnemyBase::EnemyType::MediumEnemy;
        else enemyType = EnemyBase::EnemyType::LargeEnemy;
    } else {
        // 最后1分钟：所有类型敌人
        enemyCount = QRandomGenerator::global()->bounded(2, 5);
        int typeRand = QRandomGenerator::global()->bounded(4);
        if (typeRand == 0) enemyType = EnemyBase::EnemyType::SmallEnemy;
        else if (typeRand == 1) enemyType = EnemyBase::EnemyType::MediumEnemy;
        else if (typeRand == 2) enemyType = EnemyBase::EnemyType::LargeEnemy;
        else enemyType = EnemyBase::EnemyType::BossEnemy;
    }
    
    // 生成敌人
    for (int i = 0; i < enemyCount; ++i) {
        QPointF spawnPos = getRandomSpawnPosition();
        spawnEnemy(enemyType, spawnPos);
    }
}

QPointF SugarOilGameSceneNew::getRandomSpawnPosition()
{
    // 在场景边缘随机生成位置
    int edge = QRandomGenerator::global()->bounded(4); // 0=上, 1=右, 2=下, 3=左
    QPointF pos;
    
    switch (edge) {
    case 0: // 上边缘
        pos = QPointF(QRandomGenerator::global()->bounded(SCENE_WIDTH), -50);
        break;
    case 1: // 右边缘
        pos = QPointF(SCENE_WIDTH + 50, QRandomGenerator::global()->bounded(SCENE_HEIGHT));
        break;
    case 2: // 下边缘
        pos = QPointF(QRandomGenerator::global()->bounded(SCENE_WIDTH), SCENE_HEIGHT + 50);
        break;
    case 3: // 左边缘
        pos = QPointF(-50, QRandomGenerator::global()->bounded(SCENE_HEIGHT));
        break;
    }
    
    return pos;
}

void SugarOilGameSceneNew::spawnEnemy(EnemyBase::EnemyType type, const QPointF &position)
{
    EnemyBase* enemy = new EnemyBase(mPlayer, 100, 10, 2.0, 50, type);
    enemy->setPos(position);
    addItem(enemy);
    mEnemies.append(enemy);
    
    // 连接敌人信号
    connect(enemy, &EnemyBase::enemyAttack, this, &SugarOilGameSceneNew::onEnemyAttack);
    connect(enemy, &EnemyBase::enemyDied, this, &SugarOilGameSceneNew::onEnemyDied);
    
    // 启动敌人AI
    enemy->startAI();
}

void SugarOilGameSceneNew::updateCollisions()
{
    checkPlayerEnemyCollisions();
    checkPlayerBulletEnemyCollisions();
    checkEnemyBulletPlayerCollisions();
}

void SugarOilGameSceneNew::checkPlayerEnemyCollisions()
{
    if (!mPlayer || mPlayer->isInvincible()) {
        return;
    }
    
    QRectF playerRect = mPlayer->sceneBoundingRect();
    
    for (EnemyBase* enemy : mEnemies) {
        if (enemy->sceneBoundingRect().intersects(playerRect)) {
            mPlayer->takeDamage(enemy->getAttackPoint());
            break; // 一次只处理一个碰撞
        }
    }
}

void SugarOilGameSceneNew::checkPlayerBulletEnemyCollisions()
{
    for (int i = mPlayerBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mPlayerBullets[i];
        QRectF bulletRect = bullet->sceneBoundingRect();
        
        for (int j = mEnemies.size() - 1; j >= 0; --j) {
            EnemyBase* enemy = mEnemies[j];
            if (enemy->sceneBoundingRect().intersects(bulletRect)) {
                // 敌人受伤
                enemy->takeDamage(bullet->getDamage());
                
                // 移除子弹
                removeItem(bullet);
                mPlayerBullets.removeAt(i);
                delete bullet;
                break;
            }
        }
    }
}

void SugarOilGameSceneNew::checkEnemyBulletPlayerCollisions()
{
    if (!mPlayer || mPlayer->isInvincible()) {
        return;
    }
    
    QRectF playerRect = mPlayer->sceneBoundingRect();
    
    for (int i = mEnemyBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mEnemyBullets[i];
        if (bullet->sceneBoundingRect().intersects(playerRect)) {
            // 玩家受伤
            mPlayer->takeDamage(bullet->getDamage());
            
            // 移除子弹
            removeItem(bullet);
            mEnemyBullets.removeAt(i);
            delete bullet;
        }
    }
}

void SugarOilGameSceneNew::cleanupObjects()
{
    removeDeadEnemies();
    removeOutOfBoundsBullets();
}

void SugarOilGameSceneNew::removeDeadEnemies()
{
    for (int i = mEnemies.size() - 1; i >= 0; --i) {
        EnemyBase* enemy = mEnemies[i];
        if (enemy->getHP() <= 0) {
            removeItem(enemy);
            mEnemies.removeAt(i);
            delete enemy;
        }
    }
}

void SugarOilGameSceneNew::removeOutOfBoundsBullets()
{
    QRectF sceneRect = this->sceneRect();
    
    // 检查玩家子弹
    for (int i = mPlayerBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mPlayerBullets[i];
        if (!sceneRect.intersects(bullet->sceneBoundingRect())) {
            removeItem(bullet);
            mPlayerBullets.removeAt(i);
            delete bullet;
        }
    }
    
    // 检查敌人子弹
    for (int i = mEnemyBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mEnemyBullets[i];
        if (!sceneRect.intersects(bullet->sceneBoundingRect())) {
            removeItem(bullet);
            mEnemyBullets.removeAt(i);
            delete bullet;
        }
    }
}

void SugarOilGameSceneNew::onPlayerShoot(QPointF position, QPointF direction, int damage)
{
    createPlayerBullet(position, direction, damage);
}

void SugarOilGameSceneNew::onEnemyAttack(QPointF position, QPointF direction, int damage)
{
    createEnemyBullet(position, direction, damage);
}

void SugarOilGameSceneNew::createPlayerBullet(const QPointF &position, const QPointF &direction, int damage)
{
    BulletBase* bullet = new BulletBase(mPlayer, BulletBase::BulletType::PlayerBullet);
    bullet->setPos(position);
    bullet->setMoveDirection(direction);
    bullet->setSpeed(8.0);
    bullet->setDamage(damage);
    addItem(bullet);
    mPlayerBullets.append(bullet);
    
    connect(bullet, &BulletBase::bulletOutOfBounds, this, &SugarOilGameSceneNew::onBulletOutOfBounds);
    bullet->startMoving();
}

void SugarOilGameSceneNew::createEnemyBullet(const QPointF &position, const QPointF &direction, int damage)
{
    BulletBase* bullet = new BulletBase(nullptr, BulletBase::BulletType::EnemyBullet);
    bullet->setPos(position);
    bullet->setMoveDirection(direction);
    bullet->setSpeed(6.0);
    bullet->setDamage(damage);
    addItem(bullet);
    mEnemyBullets.append(bullet);
    
    connect(bullet, &BulletBase::bulletOutOfBounds, this, &SugarOilGameSceneNew::onBulletOutOfBounds);
    bullet->startMoving();
}

void SugarOilGameSceneNew::onEnemyDied(EnemyBase* enemy)
{
    if (!enemy || !mPlayer) {
        return;
    }
    
    // 玩家获得经验和分数
    mPlayer->gainExp(enemy->getExpValue());
    
    emit scoreChanged(getScore());
}

void SugarOilGameSceneNew::onPlayerDied()
{
    stopGame();
    emit gameOver(getScore(), getPlayerLevel());
}

void SugarOilGameSceneNew::onPlayerLevelUp(int newLevel)
{
    qDebug() << "Player leveled up to level" << newLevel;
}

void SugarOilGameSceneNew::onBulletOutOfBounds(BulletBase* bullet)
{
    // 从列表中移除子弹
    mPlayerBullets.removeAll(bullet);
    mEnemyBullets.removeAll(bullet);
    
    // 从场景中移除并删除
    removeItem(bullet);
    bullet->deleteLater();
}

void SugarOilGameSceneNew::keyPressEvent(QKeyEvent *event)
{
    handleKeyPress(event->key());
    QGraphicsScene::keyPressEvent(event);
}

void SugarOilGameSceneNew::keyReleaseEvent(QKeyEvent *event)
{
    handleKeyRelease(event->key());
    QGraphicsScene::keyReleaseEvent(event);
}