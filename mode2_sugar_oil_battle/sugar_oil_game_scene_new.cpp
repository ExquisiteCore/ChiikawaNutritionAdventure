#include "sugar_oil_game_scene_new.h"
#include "creature.h"
#include "../audio_manager.h"
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
    , mItemManager(nullptr)
    , mCreatureManager(nullptr)
    , mItemSpawnTimer(nullptr)
    , mCreatureSpawnTimer(nullptr)
    , mItemSpawnCounter(0)
    , mCreatureSpawnCounter(0)
{
    initializeScene();
    initializePlayer();
    initializeTimers();
    initializeAudio();
    initializeManagers();
    loadBackground();
    
    // 初始化性能监控
    mPerformanceTimer.start();
}

SugarOilGameSceneNew::~SugarOilGameSceneNew()
{
    stopGame();
    
    // 安全清理所有游戏对象，防止重复删除
    for (EnemyBase* enemy : mEnemies) {
        if (enemy) {
            enemy->stopAI();
            removeItem(enemy);
            enemy->deleteLater();
        }
    }
    mEnemies.clear();
    
    // 子弹使用对象池，需要正确归还
    for (BulletBase* bullet : mPlayerBullets) {
        if (bullet) {
            removeItem(bullet);
            BulletBase::returnBulletToPool(bullet);
        }
    }
    mPlayerBullets.clear();
    
    for (BulletBase* bullet : mEnemyBullets) {
        if (bullet) {
            removeItem(bullet);
            BulletBase::returnBulletToPool(bullet);
        }
    }
    mEnemyBullets.clear();
    
    // 清理道具和生物
    for (GameItem* item : mItems) {
        if (item) {
            removeItem(item);
            item->deleteLater();
        }
    }
    mItems.clear();
    
    for (GameCreature* creature : mCreatures) {
        if (creature) {
            removeItem(creature);
            creature->deleteLater();
        }
    }
    mCreatures.clear();
    
    // 清理玩家
    if (mPlayer) {
        removeItem(mPlayer);
        mPlayer->deleteLater();
        mPlayer = nullptr;
    }
}

void SugarOilGameSceneNew::initializeScene()
{
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    // 使用与模式1相同的背景图片
    QPixmap backgroundPixmap(":/img/GameBackground.png");
    if (backgroundPixmap.isNull()) {
        // 如果背景图片加载失败，使用默认背景
        backgroundPixmap = QPixmap(SCENE_WIDTH, SCENE_HEIGHT);
        backgroundPixmap.fill(QColor(20, 20, 40));
    } else {
        // 缩放背景图片以适应场景大小
        backgroundPixmap = backgroundPixmap.scaled(SCENE_WIDTH, SCENE_HEIGHT, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    setBackgroundBrush(QBrush(backgroundPixmap));
    
    // 添加地图边界
    drawMapBoundaries();
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
    connect(mPlayer, &SugarOilPlayer::healthChanged, this, [this](int health, int maxHealth) {
        emit playerStatsChanged(health, maxHealth, mPlayer->getLevel(), mPlayer->getExp());
    });
    // 初始发送一次状态
    emit playerStatsChanged(mPlayer->getHP(), mPlayer->getMaxHP(), mPlayer->getLevel(), mPlayer->getExp());
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
    
    // 道具生成定时器
    mItemSpawnTimer = new QTimer(this);
    connect(mItemSpawnTimer, &QTimer::timeout, this, &SugarOilGameSceneNew::updateItemSpawning);
    
    // 生物生成定时器
    mCreatureSpawnTimer = new QTimer(this);
    connect(mCreatureSpawnTimer, &QTimer::timeout, this, &SugarOilGameSceneNew::updateCreatureSpawning);
}

void SugarOilGameSceneNew::initializeAudio()
{
    // 音频现在由AudioManager统一管理
    // 无需在此处初始化音频设备
}

void SugarOilGameSceneNew::initializeManagers()
{
    mItemManager = new ItemManager(this);
    mCreatureManager = new CreatureManager(this);
}

void SugarOilGameSceneNew::loadBackground()
{
    // 使用与模式1相同的背景图片
    QPixmap backgroundPixmap(":/img/GameBackground.png");
    if (backgroundPixmap.isNull()) {
        // 如果背景图片加载失败，使用默认背景
        backgroundPixmap = QPixmap(SCENE_WIDTH, SCENE_HEIGHT);
        backgroundPixmap.fill(QColor(20, 20, 40));
    } else {
        // 缩放背景图片以适应场景大小
        backgroundPixmap = backgroundPixmap.scaled(SCENE_WIDTH, SCENE_HEIGHT, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
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
    mItemSpawnTimer->start(8000); // 每8秒生成一个道具
    mCreatureSpawnTimer->start(15000); // 每15秒生成一个生物
    
    // 音频切换由主窗口统一管理
    
    emit gameStarted();
    emit gameStateChanged(SUGAR_OIL_RUNNING);
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
    mItemSpawnTimer->stop();
    mCreatureSpawnTimer->stop();
    
    // 音频暂停由AudioManager统一管理
    AudioManager::getInstance()->pauseCurrentMusic();
    
    // 暂停所有敌人的AI和技能定时器
    for (EnemyBase* enemy : mEnemies) {
        if (enemy) {
            enemy->stopAI();
            // 确保敌人的所有定时器都被停止，包括技能定时器
            enemy->stopAllTimers();
        }
    }
    
    // 暂停所有生物的移动
    for (GameCreature* creature : mCreatures) {
        if (creature) {
            creature->pauseMovement();
        }
    }
    
    // 暂停所有子弹的移动
    for (BulletBase* bullet : mPlayerBullets) {
        if (bullet) {
            bullet->stopMoving();
        }
    }
    for (BulletBase* bullet : mEnemyBullets) {
        if (bullet) {
            bullet->stopMoving();
        }
    }
    
    // 暂停玩家的所有定时器
    if (mPlayer) {
        mPlayer->pauseAllTimers();
    }
    
    emit gamePaused();
    emit gameStateChanged(SUGAR_OIL_PAUSED);
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
    mItemSpawnTimer->start();
    mCreatureSpawnTimer->start();
    
    // 音频恢复由AudioManager统一管理
    AudioManager::getInstance()->resumeCurrentMusic();
    
    // 恢复所有敌人的AI和技能定时器
    for (EnemyBase* enemy : mEnemies) {
        if (enemy) {
            enemy->startAI();
            // 恢复敌人的所有定时器
            enemy->resumeAllTimers();
        }
    }
    
    // 恢复所有生物的移动
    for (GameCreature* creature : mCreatures) {
        if (creature) {
            creature->resumeMovement();
        }
    }
    
    // 恢复所有子弹的移动
    for (BulletBase* bullet : mPlayerBullets) {
        if (bullet) {
            bullet->startMoving();
        }
    }
    for (BulletBase* bullet : mEnemyBullets) {
        if (bullet) {
            bullet->startMoving();
        }
    }
    
    // 恢复玩家的所有定时器
    if (mPlayer) {
        mPlayer->resumeAllTimers();
    }
    
    emit gameResumed();
    emit gameStateChanged(SUGAR_OIL_RUNNING);
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
    if (mItemSpawnTimer) mItemSpawnTimer->stop();
    if (mCreatureSpawnTimer) mCreatureSpawnTimer->stop();
    
    // 停止游戏音乐
    AudioManager::getInstance()->stopCurrentMusic();
    
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
    mItemSpawnCounter = 0;
    mCreatureSpawnCounter = 0;
    mPressedKeys.clear();
    mMousePressed = false;
    
    // 清理所有游戏对象
    for (EnemyBase* enemy : mEnemies) {
        if (enemy) {
            enemy->stopAI();
            removeItem(enemy);
            enemy->deleteLater();
        }
    }
    mEnemies.clear();
    
    for (BulletBase* bullet : mPlayerBullets) {
        if (bullet) {
            removeItem(bullet);
            BulletBase::returnBulletToPool(bullet);
        }
    }
    mPlayerBullets.clear();
    
    for (BulletBase* bullet : mEnemyBullets) {
        if (bullet) {
            removeItem(bullet);
            BulletBase::returnBulletToPool(bullet);
        }
    }
    mEnemyBullets.clear();
    
    // 清理所有道具
    for (GameItem* item : mItems) {
        if (item) {
            removeItem(item);
            item->deleteLater();
        }
    }
    mItems.clear();
    
    // 清理所有生物
    for (GameCreature* creature : mCreatures) {
        if (creature) {
            removeItem(creature);
            creature->deleteLater();
        }
    }
    mCreatures.clear();
    
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
    Q_UNUSED(scenePos)
    mMousePressed = false;
}

void SugarOilGameSceneNew::onGameTimerTimeout()
{
    mGameTime++;
    emit timeChanged(GAME_DURATION - mGameTime);
    
    // 检查游戏是否结束
    if (mGameTime >= GAME_DURATION) {
        stopGame();
        // 播放胜利音效
        AudioManager::getInstance()->playGameMusic(AudioManager::MusicType::Victory);
        emit gameWon(getScore(), getPlayerLevel());
        emit gameStateChanged(SUGAR_OIL_WON);
    }
}

void SugarOilGameSceneNew::updateGame()
{
    if (!mGameRunning || mGamePaused) {
        return;
    }
    
    // 性能监控
    mFrameCount++;
    if (mFrameCount % 100 == 0) {
        qint64 elapsed = mPerformanceTimer.elapsed();
        qreal fps = (mFrameCount * 1000.0) / elapsed;
        qDebug() << "Performance Stats - FPS:" << fps 
                 << "Enemies:" << mEnemies.size()
                 << "Player Bullets:" << mPlayerBullets.size()
                 << "Enemy Bullets:" << mEnemyBullets.size()
                 << "Items:" << mItems.size()
                 << "Creatures:" << mCreatures.size();
    }
    
    updatePlayerMovement();
    updateItems();
    updateCreatures();
    updateCollisions();
    
    // 优化清理频率，每5帧清理一次以提升性能
    static int cleanupCounter = 0;
    if (++cleanupCounter % 5 == 0) {
        cleanupObjects();
    }
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
        
        // 允许角色移动到场景边缘，只要角色中心点在场景内即可
        const qreal margin = playerRect.width() / 4; // 使用角色宽度的四分之一作为边距，增加移动自由度
        
        if (newPos.x() < -margin) newPos.setX(-margin);
        if (newPos.y() < -margin) newPos.setY(-margin);
        if (newPos.x() > SCENE_WIDTH - margin) {
            newPos.setX(SCENE_WIDTH - margin);
        }
        if (newPos.y() > SCENE_HEIGHT - margin) {
            newPos.setY(SCENE_HEIGHT - margin);
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
    mSpawnCounter++;
    
    // 根据游戏时间调整生成频率（降低刷新速度）
    int spawnInterval = 3000; // 基础间隔3秒
    if (mGameTime > 60) {
        spawnInterval = 2500; // 1分钟后加快到2.5秒
    }
    if (mGameTime > 120) {
        spawnInterval = 2000; // 2分钟后加快到2秒
    }
    if (mGameTime > 180) {
        spawnInterval = 1500; // 3分钟后加快到1.5秒
    }
    if (mGameTime > 240) {
        spawnInterval = 1200; // 4分钟后加快到1.2秒
    }
    
    mSpawnTimer->setInterval(spawnInterval);
    
    // 生成敌人
    QPointF spawnPos = getRandomSpawnPosition();
    
    // 随机选择糖油混合物敌人类型
    EnemyBase::EnemyType enemyType = static_cast<EnemyBase::EnemyType>(QRandomGenerator::global()->bounded(5));
    
    spawnEnemy(enemyType, spawnPos);
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
    checkPlayerItemCollisions();
    checkPlayerCreatureCollisions();
}

void SugarOilGameSceneNew::checkPlayerEnemyCollisions()
{
    if (!mPlayer || mPlayer->isInvincible()) {
        return;
    }
    
    QRectF playerRect = mPlayer->sceneBoundingRect();
    
    for (EnemyBase* enemy : mEnemies) {
        if (!enemy) {
            continue;
        }
        
        if (enemy->sceneBoundingRect().intersects(playerRect)) {
            mPlayer->takeDamage(enemy->getAttackPoint());
            break; // 一次只处理一个碰撞
        }
    }
}

void SugarOilGameSceneNew::checkPlayerBulletEnemyCollisions()
{
    // 使用临时列表存储需要移除的对象，避免在循环中修改列表导致索引问题
    QList<BulletBase*> bulletsToRemove;
    QList<EnemyBase*> enemiesToRemove;
    
    for (int i = mPlayerBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mPlayerBullets[i];
        if (!bullet) {
            mPlayerBullets.removeAt(i);
            continue;
        }
        
        // 跳过已销毁的子弹
        if (bullet->isDestroyed()) {
            continue;
        }
        
        bool bulletHit = false;
        
        for (int j = 0; j < mEnemies.size(); ++j) {
            EnemyBase* enemy = mEnemies[j];
            if (!enemy || enemy->getHP() <= 0) {
                continue; // 跳过无效或已死亡的敌人
            }
            
            // 使用距离检测提高碰撞精度
            QPointF bulletPos = bullet->pos();
            QPointF enemyPos = enemy->pos();
            qreal distance = QLineF(bulletPos, enemyPos).length();
            
            if (distance < SUGAR_OIL_COLLISION_DISTANCE) {
                // 添加调试信息
                qDebug() << "Bullet collision detected! Distance:" << distance << "Bullet damage:" << bullet->getDamage();
                
                // 立即标记子弹为已销毁，避免重复处理
                bullet->markForDestruction();
                
                // 敌人受伤
                enemy->takeDamage(bullet->getDamage());
                
                // 如果敌人死亡，标记为需要移除
                if (enemy->getHP() <= 0) {
                    qDebug() << "Enemy killed by bullet";
                    enemy->stopAI();
                    enemiesToRemove.append(enemy);
                }
                
                // 标记子弹为需要移除
                qDebug() << "Marking bullet for removal";
                bulletsToRemove.append(bullet);
                bulletHit = true;
                break;
            }
        }
        
        if (bulletHit) {
            break;
        }
    }
    
    // 移除被击中的子弹
    qDebug() << "Removing" << bulletsToRemove.size() << "bullets from collision";
    for (BulletBase* bullet : bulletsToRemove) {
        qDebug() << "Removing bullet from scene and returning to pool";
        mPlayerBullets.removeOne(bullet);
        removeItem(bullet);
        BulletBase::returnBulletToPool(bullet);
    }
    
    // 移除死亡的敌人
    for (EnemyBase* enemy : enemiesToRemove) {
        mEnemies.removeOne(enemy);
        removeItem(enemy);
        enemy->deleteLater();
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
            BulletBase::returnBulletToPool(bullet);
        }
    }
}

void SugarOilGameSceneNew::cleanupObjects()
{
    removeDeadEnemies();
    removeOutOfBoundsBullets();
    removeCollectedItems();
    removeActivatedCreatures();
}

void SugarOilGameSceneNew::removeDeadEnemies()
{
    for (int i = mEnemies.size() - 1; i >= 0; --i) {
        EnemyBase* enemy = mEnemies[i];
        if (!enemy) {
            mEnemies.removeAt(i);
            continue;
        }
        
        if (enemy->getHP() <= 0) {
            // 先停止AI避免在删除过程中继续执行
            enemy->stopAI();
            
            // 从场景中移除
            removeItem(enemy);
            
            // 从列表中移除
            mEnemies.removeAt(i);
            
            // 延迟删除，避免在信号处理过程中删除对象
            enemy->deleteLater();
        }
    }
}

// 道具系统方法
void SugarOilGameSceneNew::updateItemSpawning()
{
    mItemSpawnCounter++;
    spawnRandomItem();
}

void SugarOilGameSceneNew::spawnRandomItem()
{
    if (!mItemManager) return;
    
    // 在屏幕边缘随机位置生成道具
    QPointF spawnPos = getRandomSpawnPosition();
    GameItem* item = mItemManager->spawnRandomItem(spawnPos);
    if (item) {
        addItem(item);
        mItems.append(item);
    }
}

void SugarOilGameSceneNew::updateItems()
{
    for (int i = mItems.size() - 1; i >= 0; --i) {
        GameItem* item = mItems[i];
        if (!item) {
            mItems.removeAt(i);
            continue;
        }
        
        item->updateAnimation();
    }
}

void SugarOilGameSceneNew::checkPlayerItemCollisions()
{
    if (!mPlayer) return;
    
    QRectF playerRect = mPlayer->boundingRect().translated(mPlayer->pos());
    
    for (int i = mItems.size() - 1; i >= 0; --i) {
        GameItem* item = mItems[i];
        if (!item) {
            mItems.removeAt(i);
            continue;
        }
        
        QRectF itemRect = item->boundingRect().translated(item->pos());
        if (playerRect.intersects(itemRect)) {
            // 应用道具效果
            item->applyEffect(mPlayer);
            // 移除道具
            removeItem(item);
            mItems.removeAt(i);
            item->deleteLater();
        }
    }
}

void SugarOilGameSceneNew::removeCollectedItems()
{
    // 道具在碰撞检测中已经被移除，这里可以处理其他清理逻辑
    // 例如：移除超出屏幕边界的道具
    for (int i = mItems.size() - 1; i >= 0; --i) {
        GameItem* item = mItems[i];
        if (!item) {
            mItems.removeAt(i);
            continue;
        }
        
        QPointF itemPos = item->pos();
        // 如果道具超出屏幕边界，移除它
        if (itemPos.x() < -100 || itemPos.x() > width() + 100 ||
            itemPos.y() < -100 || itemPos.y() > height() + 100) {
            removeItem(item);
            mItems.removeAt(i);
            item->deleteLater();
        }
    }
}

// 生物系统方法
void SugarOilGameSceneNew::updateCreatureSpawning()
{
    mCreatureSpawnCounter++;
    spawnRandomCreature();
}

void SugarOilGameSceneNew::spawnRandomCreature()
{
    if (!mCreatureManager) return;
    
    // 在屏幕边缘随机位置生成生物
    QPointF spawnPos = getRandomSpawnPosition();
    GameCreature* creature = mCreatureManager->spawnRandomCreature(spawnPos);
    if (creature) {
        addItem(creature);
        mCreatures.append(creature);
    }
}

void SugarOilGameSceneNew::updateCreatures()
{
    if (!mPlayer) return;
    
    for (int i = mCreatures.size() - 1; i >= 0; --i) {
        GameCreature* creature = mCreatures[i];
        if (!creature) {
            mCreatures.removeAt(i);
            continue;
        }
        
        creature->updateCreature();
        
        // 检查是否靠近玩家
        if (creature->isNearPlayer(mPlayer->pos(), 50.0)) {
            creature->activateEffect(mPlayer);
        } else {
            // 向玩家移动
            creature->moveTowardsPlayer(mPlayer->pos());
        }
    }
}

void SugarOilGameSceneNew::checkPlayerCreatureCollisions()
{
    if (!mPlayer) return;
    
    QRectF playerRect = mPlayer->boundingRect().translated(mPlayer->pos());
    
    for (GameCreature* creature : mCreatures) {
        if (!creature) {
            continue;
        }
        
        QRectF creatureRect = creature->boundingRect().translated(creature->pos());
        if (playerRect.intersects(creatureRect)) {
            creature->activateEffect(mPlayer);
        }
    }
}

void SugarOilGameSceneNew::removeActivatedCreatures()
{
    // 暂时保留所有生物，可以根据需要添加移除逻辑
    // 例如：生物激活效果后一段时间后自动消失
}

void SugarOilGameSceneNew::removeOutOfBoundsBullets()
{
    // 使用简单坐标比较优化性能
    const qreal margin = 50.0; // 边界缓冲区
    const qreal minX = -margin;
    const qreal maxX = SUGAR_OIL_SCENE_WIDTH + margin;
    const qreal minY = -margin;
    const qreal maxY = SUGAR_OIL_SCENE_HEIGHT + margin;
    
    // 检查玩家子弹
    for (int i = mPlayerBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mPlayerBullets[i];
        if (!bullet) {
            mPlayerBullets.removeAt(i);
            continue;
        }
        
        QPointF pos = bullet->pos();
        if (pos.x() < minX || pos.x() > maxX || pos.y() < minY || pos.y() > maxY) {
            removeItem(bullet);
            mPlayerBullets.removeAt(i);
            BulletBase::returnBulletToPool(bullet);
        }
    }
    
    // 检查敌人子弹
    for (int i = mEnemyBullets.size() - 1; i >= 0; --i) {
        BulletBase* bullet = mEnemyBullets[i];
        if (!bullet) {
            mEnemyBullets.removeAt(i);
            continue;
        }
        
        QPointF pos = bullet->pos();
        if (pos.x() < minX || pos.x() > maxX || pos.y() < minY || pos.y() > maxY) {
            removeItem(bullet);
            mEnemyBullets.removeAt(i);
            BulletBase::returnBulletToPool(bullet);
        }
    }
}

void SugarOilGameSceneNew::onPlayerShoot(QPointF position, QPointF direction, int damage)
{
    createPlayerBullet(position, direction, damage);
}

void SugarOilGameSceneNew::onEnemyAttack(EnemyBase* enemy, QPointF position, QPointF direction, int damage)
{
    createEnemyBullet(enemy, position, direction, damage);
}

void SugarOilGameSceneNew::createPlayerBullet(const QPointF &position, const QPointF &direction, int damage)
{
    // 使用对象池获取子弹，减少内存分配开销
    BulletBase* bullet = BulletBase::getBulletFromPool(mPlayer, BulletBase::BulletType::PlayerBullet);
    bullet->setPos(position);
    bullet->setMoveDirection(direction);
    bullet->setSpeed(5.0); // 降低速度减少穿透问题
    bullet->setDamage(damage);
    addItem(bullet);
    mPlayerBullets.append(bullet);
    
    connect(bullet, &BulletBase::bulletOutOfBounds, this, &SugarOilGameSceneNew::onBulletOutOfBounds);
    bullet->startMoving();
}

void SugarOilGameSceneNew::createEnemyBullet(EnemyBase* enemy, const QPointF &position, const QPointF &direction, int damage)
{
    // 使用对象池获取子弹，减少内存分配开销
    BulletBase* bullet = BulletBase::getBulletFromPool(enemy, BulletBase::BulletType::EnemyBullet);
    if (!bullet) {
        qWarning("Failed to create enemy bullet!");
        return;
    }
    
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
    
    // 确保敌人从列表中移除（如果还在的话）
    mEnemies.removeAll(enemy);
}

void SugarOilGameSceneNew::onPlayerDied()
{
    qDebug() << "Player died!";
    stopGame();
    // 播放失败音效
    AudioManager::getInstance()->playGameMusic(AudioManager::MusicType::Defeat);
    emit gameOver(getScore(), getPlayerLevel());
    emit gameStateChanged(SUGAR_OIL_LOST);
}

void SugarOilGameSceneNew::onPlayerLevelUp(int newLevel)
{
    qDebug() << "Player leveled up to level" << newLevel;
}

void SugarOilGameSceneNew::onBulletOutOfBounds(BulletBase* bullet)
{
    // 检查子弹是否已经被销毁，避免重复处理
    if (!bullet || bullet->isDestroyed()) {
        qDebug() << "onBulletOutOfBounds: Bullet already destroyed, skipping";
        return;
    }
    
    qDebug() << "onBulletOutOfBounds: Processing bullet removal";
    
    // 标记为已销毁
    bullet->markForDestruction();
    
    // 从列表中移除子弹
    mPlayerBullets.removeAll(bullet);
    mEnemyBullets.removeAll(bullet);
    
    // 从场景中移除
    removeItem(bullet);
    
    // 使用对象池回收子弹，提高性能
    BulletBase::returnBulletToPool(bullet);
}

void SugarOilGameSceneNew::drawMapBoundaries()
{
    // 创建边界线条，参考模式1的墙体样式
    QPen boundaryPen(QColor(100, 50, 0), 4); // 棕色边界线，4像素宽度
    QBrush boundaryBrush(QColor(100, 50, 0, 100)); // 半透明棕色填充
    
    const int borderWidth = 4; // 减少边界宽度，给角色更多移动空间
    
    // 上边界
    QGraphicsRectItem* topBorder = addRect(0, 0, SCENE_WIDTH, borderWidth, boundaryPen, boundaryBrush);
    topBorder->setZValue(10);
    
    // 下边界
    QGraphicsRectItem* bottomBorder = addRect(0, SCENE_HEIGHT - borderWidth, SCENE_WIDTH, borderWidth, boundaryPen, boundaryBrush);
    bottomBorder->setZValue(10);
    
    // 左边界
    QGraphicsRectItem* leftBorder = addRect(0, 0, borderWidth, SCENE_HEIGHT, boundaryPen, boundaryBrush);
    leftBorder->setZValue(10);
    
    // 右边界
    QGraphicsRectItem* rightBorder = addRect(SCENE_WIDTH - borderWidth, 0, borderWidth, SCENE_HEIGHT, boundaryPen, boundaryBrush);
    rightBorder->setZValue(10);
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