#include "sugar_oil_game_scene.h"
#include <QPainter>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QRandomGenerator>
#include <QDebug>

SugarOilGameScene::SugarOilGameScene(QObject *parent)
    : QGraphicsScene(parent)
    , currentState(SUGAR_OIL_MENU)
    , remainingTime(GAME_DURATION_SECONDS)
    , gameInitialized(false)
    , player(nullptr)
    , gameTimer(nullptr)
    , countdownTimer(nullptr)
    , enemySpawnTimer(nullptr)
    , itemSpawnTimer(nullptr)
    , creatureSpawnTimer(nullptr)
    , backgroundMusicPlayer(nullptr)
    , soundEffect(nullptr)
    , enemiesSpawned(0)
    , itemsSpawned(0)
    , creaturesSpawned(0)
{
    qDebug() << "SugarOilGameScene构造函数开始";
    
    // 设置场景大小
    setSceneRect(0, 0, SUGAR_OIL_SCENE_WIDTH, SUGAR_OIL_SCENE_HEIGHT);
    qDebug() << "场景大小设置完成:" << SUGAR_OIL_SCENE_WIDTH << "x" << SUGAR_OIL_SCENE_HEIGHT;
    
    // 加载背景图片
    backgroundPixmap = QPixmap(":/img/GameBackground.png");
    if (backgroundPixmap.isNull()) {
        qDebug() << "背景图片加载失败，使用默认背景";
        backgroundPixmap = QPixmap(SUGAR_OIL_SCENE_WIDTH, SUGAR_OIL_SCENE_HEIGHT);
        backgroundPixmap.fill(QColor(34, 139, 34)); // 森林绿色背景
    } else {
        qDebug() << "背景图片加载成功";
    }
    
    qDebug() << "开始初始化游戏";
    initializeGame();
    qDebug() << "SugarOilGameScene构造函数完成";
}

SugarOilGameScene::~SugarOilGameScene()
{
    // 停止并清理所有定时器
    if (gameTimer) {
        gameTimer->stop();
        gameTimer->deleteLater();
    }
    if (countdownTimer) {
        countdownTimer->stop();
        countdownTimer->deleteLater();
    }
    if (enemySpawnTimer) {
        enemySpawnTimer->stop();
        enemySpawnTimer->deleteLater();
    }
    if (itemSpawnTimer) {
        itemSpawnTimer->stop();
        itemSpawnTimer->deleteLater();
    }
    if (creatureSpawnTimer) {
        creatureSpawnTimer->stop();
        creatureSpawnTimer->deleteLater();
    }
    
    // 清理音频
    if (backgroundMusicPlayer) {
        backgroundMusicPlayer->stop();
    }
    
    // 清理玩家对象
    if (player) {
        disconnect(player, nullptr, this, nullptr);
        removeItem(player);
        player->deleteLater();
        player = nullptr;
    }
    
    // 清理游戏对象
    for (auto enemy : enemies) {
        removeItem(enemy);
        enemy->deleteLater();
    }
    enemies.clear();
    
    for (auto item : items) {
        removeItem(item);
        item->deleteLater();
    }
    items.clear();
    
    for (auto creature : creatures) {
        removeItem(creature);
        creature->deleteLater();
    }
    creatures.clear();
}

void SugarOilGameScene::initializeGame()
{
    if (gameInitialized) return;
    
    setupTimers();
    setupAudio();
    
    gameInitialized = true;
}

void SugarOilGameScene::setupTimers()
{
    // 主游戏循环定时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &SugarOilGameScene::updateGame);
    
    // 倒计时定时器
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &SugarOilGameScene::updateTimer);
    
    // 敌人生成定时器
    enemySpawnTimer = new QTimer(this);
    connect(enemySpawnTimer, &QTimer::timeout, this, &SugarOilGameScene::spawnEnemy);
    
    // 道具生成定时器
    itemSpawnTimer = new QTimer(this);
    connect(itemSpawnTimer, &QTimer::timeout, this, &SugarOilGameScene::spawnItem);
    
    // 生物生成定时器
    creatureSpawnTimer = new QTimer(this);
    connect(creatureSpawnTimer, &QTimer::timeout, this, &SugarOilGameScene::spawnCreature);
}

void SugarOilGameScene::setupAudio()
{
    // 背景音乐 - Qt版本兼容
    backgroundMusicPlayer = new QMediaPlayer(this);
    
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // Qt6版本
        musicAudioOutput = new QAudioOutput(this);
        backgroundMusicPlayer->setAudioOutput(musicAudioOutput);
        musicAudioOutput->setVolume(0.5); // Qt6中音量范围是0.0-1.0
    #else
        // Qt5版本
        musicAudioOutput = nullptr;
        backgroundMusicPlayer->setVolume(50); // Qt5中音量范围是0-100
    #endif
    
    // 音效
    soundEffect = new QSoundEffect(this);
}

void SugarOilGameScene::startGame()
{
    qDebug() << "SugarOilGameScene::startGame开始";
    
    if (currentState == SUGAR_OIL_RUNNING) {
        qDebug() << "游戏已经在运行中，返回";
        return;
    }
    
    qDebug() << "重置游戏";
    resetGame();
    
    // 创建玩家 - 确保只创建一次
    qDebug() << "创建新的UsagiPlayer";
    player = new UsagiPlayer(this);
    addItem(player);
    player->setPosition(QPointF(SUGAR_OIL_SCENE_WIDTH / 2, SUGAR_OIL_SCENE_HEIGHT / 2));
    
    // 连接玩家信号
    connect(player, &UsagiPlayer::livesChanged, this, &SugarOilGameScene::livesChanged);
    connect(player, &UsagiPlayer::scoreChanged, this, &SugarOilGameScene::scoreChanged);
    connect(player, &UsagiPlayer::playerDamaged, this, &SugarOilGameScene::onPlayerDamaged);
    connect(player, &UsagiPlayer::playerHealed, this, &SugarOilGameScene::onPlayerHealed);
    connect(player, &UsagiPlayer::playerDied, this, [this]() {
        endGame(false);
    });
    
    // 生成初始对象
    spawnInitialObjects();
    
    // 启动定时器
    gameTimer->start(TIMER_UPDATE_INTERVAL);
    countdownTimer->start(1000); // 每秒更新一次倒计时
    enemySpawnTimer->start(SUGAR_OIL_ENEMY_SPAWN_INTERVAL);
    itemSpawnTimer->start(ITEM_SPAWN_INTERVAL);
    creatureSpawnTimer->start(CREATURE_SPAWN_INTERVAL);
    
    // 播放背景音乐
    playBackgroundMusic();
    
    currentState = SUGAR_OIL_RUNNING;
    emit gameStateChanged(currentState);
    
    performanceTimer.start();
}

void SugarOilGameScene::pauseGame()
{
    if (currentState != SUGAR_OIL_RUNNING) return;
    
    // 暂停所有定时器
    gameTimer->stop();
    countdownTimer->stop();
    enemySpawnTimer->stop();
    itemSpawnTimer->stop();
    creatureSpawnTimer->stop();
    
    // 暂停音乐
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (backgroundMusicPlayer->playbackState() == QMediaPlayer::PlayingState) {
            backgroundMusicPlayer->pause();
        }
    #else
        if (backgroundMusicPlayer->state() == QMediaPlayer::PlayingState) {
            backgroundMusicPlayer->pause();
        }
    #endif
    
    // 暂停所有动画
    if (player) player->pauseAnimation();
    for (auto enemy : enemies) {
        enemy->pauseAnimation();
    }
    for (auto item : items) {
        item->stopAllAnimations();
    }
    for (auto creature : creatures) {
        creature->stopAllAnimations();
    }
    
    currentState = SUGAR_OIL_PAUSED;
    emit gameStateChanged(currentState);
}

void SugarOilGameScene::resumeGame()
{
    if (currentState != SUGAR_OIL_PAUSED) return;
    
    // 恢复所有定时器
    gameTimer->start(TIMER_UPDATE_INTERVAL);
    countdownTimer->start(1000);
    enemySpawnTimer->start(SUGAR_OIL_ENEMY_SPAWN_INTERVAL);
    itemSpawnTimer->start(ITEM_SPAWN_INTERVAL);
    creatureSpawnTimer->start(CREATURE_SPAWN_INTERVAL);
    
    // 恢复音乐
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (backgroundMusicPlayer->playbackState() == QMediaPlayer::PausedState) {
            backgroundMusicPlayer->play();
        }
    #else
        if (backgroundMusicPlayer->state() == QMediaPlayer::PausedState) {
            backgroundMusicPlayer->play();
        }
    #endif
    
    // 恢复所有动画
    if (player) player->resumeAnimation();
    for (auto enemy : enemies) {
        enemy->resumeAnimation();
    }
    for (auto item : items) {
        item->startFloatingAnimation();
        item->startGlowAnimation();
    }
    for (auto creature : creatures) {
        creature->startIdleAnimation();
    }
    
    currentState = SUGAR_OIL_RUNNING;
    emit gameStateChanged(currentState);
}

void SugarOilGameScene::endGame(bool won)
{
    // 停止所有定时器
    if (gameTimer) gameTimer->stop();
    if (countdownTimer) countdownTimer->stop();
    if (enemySpawnTimer) enemySpawnTimer->stop();
    if (itemSpawnTimer) itemSpawnTimer->stop();
    if (creatureSpawnTimer) creatureSpawnTimer->stop();
    
    // 停止音乐
    stopBackgroundMusic();
    
    // 播放结束音效
    if (won) {
        playSound("Win.wav");
        currentState = SUGAR_OIL_WON;
        emit gameWon();
    } else {
        playSound("Lose.wav");
        currentState = SUGAR_OIL_LOST;
        emit gameLost();
    }
    
    emit gameStateChanged(currentState);
}

void SugarOilGameScene::resetGame()
{
    // 停止所有定时器
    if (gameTimer) gameTimer->stop();
    if (countdownTimer) countdownTimer->stop();
    if (enemySpawnTimer) enemySpawnTimer->stop();
    if (itemSpawnTimer) itemSpawnTimer->stop();
    if (creatureSpawnTimer) creatureSpawnTimer->stop();
    
    // 清理所有游戏对象 - 使用deleteLater()避免崩溃
    if (player) {
        // 断开信号连接
        disconnect(player, nullptr, this, nullptr);
        removeItem(player);
        player->deleteLater();
        player = nullptr;
    }
    
    for (auto enemy : enemies) {
        removeItem(enemy);
        enemy->deleteLater();
    }
    enemies.clear();
    
    for (auto item : items) {
        removeItem(item);
        item->deleteLater();
    }
    items.clear();
    
    for (auto creature : creatures) {
        removeItem(creature);
        creature->deleteLater();
    }
    creatures.clear();
    
    // 重置游戏状态
    remainingTime = GAME_DURATION_SECONDS;
    enemiesSpawned = 0;
    itemsSpawned = 0;
    creaturesSpawned = 0;
    
    currentState = SUGAR_OIL_MENU;
    emit timeChanged(remainingTime);
}

void SugarOilGameScene::handleKeyPress(QKeyEvent *event)
{
    if (!player || currentState != SUGAR_OIL_RUNNING) return;
    
    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        player->setDirection(SUGAR_OIL_DIR_UP);
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        player->setDirection(SUGAR_OIL_DIR_DOWN);
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        player->setDirection(SUGAR_OIL_DIR_LEFT);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        player->setDirection(SUGAR_OIL_DIR_RIGHT);
        break;
    case Qt::Key_Space:
        // 使用特殊能力
        break;
    case Qt::Key_Escape:
        if (currentState == SUGAR_OIL_RUNNING) {
            pauseGame();
        } else if (currentState == SUGAR_OIL_PAUSED) {
            resumeGame();
        }
        break;
    }
}

void SugarOilGameScene::handleKeyRelease(QKeyEvent *event)
{
    if (!player || currentState != SUGAR_OIL_RUNNING) return;
    
    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
    case Qt::Key_A:
    case Qt::Key_Left:
    case Qt::Key_D:
    case Qt::Key_Right:
        player->stopMovement();
        break;
    }
}

void SugarOilGameScene::updateGame()
{
    if (currentState != SUGAR_OIL_RUNNING) return;
    
    updateGameObjects();
    checkCollisions();
    cleanupObjects();
    checkWinCondition();
    checkLoseCondition();
}

void SugarOilGameScene::updateTimer()
{
    if (currentState != SUGAR_OIL_RUNNING) return;
    
    remainingTime--;
    emit timeChanged(remainingTime);
    
    if (remainingTime <= 0) {
        endGame(true); // 时间到达，玩家获胜
    }
}

void SugarOilGameScene::spawnEnemy()
{
    if (currentState != SUGAR_OIL_RUNNING || enemies.size() >= SUGAR_OIL_ENEMY_COUNT) return;
    
    // 随机选择敌人类型
    SugarOilEnemyType enemyType = static_cast<SugarOilEnemyType>(
        QRandomGenerator::global()->bounded(5));
    
    SugarOilEnemy* enemy = new SugarOilEnemy(enemyType, this);
    
    // 设置随机生成位置（避开玩家附近）
    QPointF spawnPos;
    do {
        spawnPos = getRandomSpawnPosition();
    } while (!isPositionSafe(spawnPos));
    
    enemy->setPosition(spawnPos);
    
    // 设置目标为玩家位置
    if (player) {
        enemy->setTarget(player->getCenter());
    }
    
    // 连接信号
    connect(enemy, &SugarOilEnemy::enemyDestroyed, this, [this](SugarOilEnemy* enemy) {
        enemies.removeOne(enemy);
        removeItem(enemy);
        enemy->deleteLater();
    });
    
    connect(enemy, &SugarOilEnemy::playerHit, this, [this](int damage) {
        if (player) {
            player->takeDamage(damage);
        }
    });
    
    enemies.append(enemy);
    addItem(enemy);
    enemiesSpawned++;
}

void SugarOilGameScene::spawnItem()
{
    if (currentState != SUGAR_OIL_RUNNING || items.size() >= ITEM_COUNT) return;
    
    // 随机选择道具类型
    ItemType itemType = static_cast<ItemType>(
        QRandomGenerator::global()->bounded(24));
    
    GameItem* item = new GameItem(itemType, this);
    
    // 设置随机生成位置
    QPointF spawnPos = getRandomSpawnPosition();
    item->setPosition(spawnPos);
    
    // 启动动画
    item->startFloatingAnimation();
    item->startGlowAnimation();
    
    // 连接信号
    connect(item, &GameItem::itemCollected, this, [this](GameItem* item) {
        items.removeOne(item);
        removeItem(item);
        
        // 应用道具效果
        applyItemEffect(item->getItemType());
        
        // 播放音效
        playSound("Bean_sound_short.wav");
        
        // 发送信号
        emit itemCollected(item->getItemType());
        
        item->deleteLater();
    });
    
    connect(item, &GameItem::itemDestroyed, this, [this](GameItem* item) {
        items.removeOne(item);
        removeItem(item);
        item->deleteLater();
    });
    
    items.append(item);
    addItem(item);
    itemsSpawned++;
}

void SugarOilGameScene::spawnCreature()
{
    if (currentState != SUGAR_OIL_RUNNING || creatures.size() >= CREATURE_COUNT) return;
    
    // 随机选择生物类型
    CreatureType creatureType = static_cast<CreatureType>(
        QRandomGenerator::global()->bounded(5));
    
    Creature* creature = new Creature(creatureType, this);
    
    // 设置随机生成位置
    QPointF spawnPos = getRandomSpawnPosition();
    creature->setPosition(spawnPos);
    
    // 启动动画
    creature->startIdleAnimation();
    creature->playAppearAnimation();
    
    // 连接信号
    connect(creature, &Creature::creatureEncountered, this, [this](Creature* creature) {
        creatures.removeOne(creature);
        removeItem(creature);
        
        // 应用生物效果
        applyCreatureEffect(creature->getCreatureType());
        
        // 播放音效
        playSound("Bean_sound.wav");
        
        // 发送信号
        emit creatureEncountered(creature->getCreatureType());
        
        creature->deleteLater();
    });
    
    creatures.append(creature);
    addItem(creature);
    creaturesSpawned++;
}

void SugarOilGameScene::updateGameObjects()
{
    // 更新玩家
    if (player) {
        player->updatePosition();
        player->constrainToScene(sceneRect());
    }
    
    // 更新敌人
    for (auto enemy : enemies) {
        if (player) {
            enemy->setTarget(player->getCenter());
        }
        enemy->updateMovement();
    }
    
    // 更新道具磁铁效果
    for (auto item : items) {
        if (player && player->hasShield()) { // 假设磁铁效果与护盾关联
            item->setMagnetTarget(player->getCenter());
            item->setMagnetActive(true);
            item->updateMagnetMovement();
        }
    }
    
    // 更新生物
    for (auto creature : creatures) {
        if (creature->isFriendly() && player) {
            creature->moveTowardsPlayer(player->getCenter());
        } else {
            creature->moveRandomly();
        }
    }
}

void SugarOilGameScene::checkCollisions()
{
    if (!player) return;
    
    QPointF playerCenter = player->getCenter();
    float playerRadius = USAGI_SIZE / 2.0f;
    
    // 检查与敌人的碰撞 - 改进碰撞检测，考虑对象大小
    for (int i = enemies.size() - 1; i >= 0; i--) {
        auto enemy = enemies[i];
        if (enemy->isActive()) {
            QPointF enemyCenter = enemy->getCenter();
            float enemyRadius = SUGAR_OIL_ENEMY_SIZE / 2.0f;
            float distance = QLineF(playerCenter, enemyCenter).length();
            float collisionDistance = playerRadius + enemyRadius;
            
            if (distance < collisionDistance) {
                if (!player->isInvincible()) {
                    player->takeDamage(enemy->getDamage());
                    playSound("Cough_sound.wav");
                }
            }
        }
    }
    
    // 检查与道具的碰撞 - 改进碰撞检测，考虑对象大小
    for (int i = items.size() - 1; i >= 0; i--) {
        auto item = items[i];
        if (item->isActive()) {
            QPointF itemCenter = item->getCenter();
            float itemRadius = ITEM_SIZE / 2.0f;
            float distance = QLineF(playerCenter, itemCenter).length();
            float collisionDistance = playerRadius + itemRadius;
            
            if (distance < collisionDistance) {
                item->collect();
            }
        }
    }
    
    // 检查与生物的碰撞 - 改进碰撞检测，考虑对象大小
    for (int i = creatures.size() - 1; i >= 0; i--) {
        auto creature = creatures[i];
        if (creature->isActive()) {
            QPointF creatureCenter = creature->getCenter();
            float creatureRadius = CREATURE_SIZE / 2.0f;
            float distance = QLineF(playerCenter, creatureCenter).length();
            float collisionDistance = playerRadius + creatureRadius;
            
            if (distance < collisionDistance) {
                creature->encounter();
            }
        }
    }
}

void SugarOilGameScene::cleanupObjects()
{
    removeOffscreenObjects();
}

void SugarOilGameScene::removeOffscreenObjects()
{
    QRectF sceneArea = sceneRect().adjusted(-100, -100, 100, 100);
    
    // 清理超出屏幕的敌人 - 使用deleteLater()避免崩溃
    for (int i = enemies.size() - 1; i >= 0; i--) {
        if (!sceneArea.contains(enemies[i]->getCenter())) {
            removeItem(enemies[i]);
            enemies[i]->deleteLater();
            enemies.removeAt(i);
        }
    }
    
    // 清理超出屏幕的道具 - 使用deleteLater()避免崩溃
    for (int i = items.size() - 1; i >= 0; i--) {
        if (!sceneArea.contains(items[i]->getCenter())) {
            removeItem(items[i]);
            items[i]->deleteLater();
            items.removeAt(i);
        }
    }
    
    // 清理超出屏幕的生物 - 使用deleteLater()避免崩溃
    for (int i = creatures.size() - 1; i >= 0; i--) {
        if (!sceneArea.contains(creatures[i]->getCenter())) {
            removeItem(creatures[i]);
            creatures[i]->deleteLater();
            creatures.removeAt(i);
        }
    }
}

void SugarOilGameScene::checkWinCondition()
{
    // 胜利条件：坚持300秒
    // 这个检查在updateTimer()中进行
}

void SugarOilGameScene::checkLoseCondition()
{
    // 失败条件：生命值为0
    if (player && !player->isAlive()) {
        endGame(false);
    }
}

void SugarOilGameScene::spawnInitialObjects()
{
    // 生成一些初始道具
    for (int i = 0; i < 3; i++) {
        spawnItem();
    }
    
    // 生成一个初始敌人
    spawnEnemy();
}

QPointF SugarOilGameScene::getRandomSpawnPosition()
{
    int margin = 50;
    int x = QRandomGenerator::global()->bounded(margin, SUGAR_OIL_SCENE_WIDTH - margin);
    int y = QRandomGenerator::global()->bounded(margin, SUGAR_OIL_SCENE_HEIGHT - margin);
    return QPointF(x, y);
}

bool SugarOilGameScene::isPositionSafe(const QPointF& position)
{
    if (!player) return true;
    
    QPointF playerPos = player->getCenter();
    float distance = QLineF(playerPos, position).length();
    return distance > 100; // 至少距离玩家100像素
}

void SugarOilGameScene::applyItemEffect(ItemType itemType)
{
    if (!player) return;
    
    switch (itemType) {
    case ITEM_SPEED_BOOST:
        player->setSpeedMultiplier(SPEED_BUFF_MULTIPLIER);
        break;
    case ITEM_SHIELD:
        player->activateShield(SHIELD_BUFF_DURATION);
        break;
    case ITEM_HEALTH:
        player->heal(1);
        break;
    case ITEM_SCORE_BONUS:
        player->addScore(100);
        break;
    case ITEM_INVINCIBLE:
        player->setInvincible(true, SHIELD_BUFF_DURATION);
        break;
    case ITEM_DOUBLE_SCORE:
        player->setScoreMultiplier(2.0f);
        break;
    case ITEM_EXTRA_LIFE:
        player->addLife();
        break;
    case ITEM_FREEZE_ENEMIES:
        for (auto enemy : enemies) {
            enemy->freeze(5000); // 冰冻5秒
        }
        break;
    case ITEM_MAGNET:
        player->activateMagnet(BUFF_DURATION);
        break;
    default:
        player->addScore(50); // 默认给分数
        break;
    }
}

void SugarOilGameScene::applyCreatureEffect(CreatureType creatureType)
{
    if (!player) return;
    
    switch (creatureType) {
    case CREATURE_HELPER:
        player->addScore(200);
        break;
    case CREATURE_GUARDIAN:
        player->activateShield(BUFF_DURATION * 2);
        break;
    case CREATURE_HEALER:
        player->heal(2);
        break;
    case CREATURE_SPEEDSTER:
        player->setSpeedMultiplier(SPEED_BUFF_MULTIPLIER * 1.5f);
        break;
    case CREATURE_WARRIOR:
        player->setInvincible(true, BUFF_DURATION);
        break;
    }
}

int SugarOilGameScene::getPlayerLives() const
{
    return player ? player->getLives() : 0;
}

int SugarOilGameScene::getPlayerScore() const
{
    return player ? player->getScore() : 0;
}

void SugarOilGameScene::playBackgroundMusic()
{
    if (backgroundMusicPlayer) {
        #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            // Qt6使用setSource
            backgroundMusicPlayer->setSource(QUrl("qrc:/Sounds/Game_sound.wav"));
        #else
            // Qt5使用setMedia
            backgroundMusicPlayer->setMedia(QUrl("qrc:/Sounds/Game_sound.wav"));
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

void SugarOilGameScene::stopBackgroundMusic()
{
    if (backgroundMusicPlayer) {
        backgroundMusicPlayer->stop();
    }
}

void SugarOilGameScene::playSound(const QString& soundName)
{
    if (soundEffect) {
        soundEffect->setSource(QUrl(QString("qrc:/Sounds/%1").arg(soundName)));
        soundEffect->play();
    }
}

void SugarOilGameScene::setMusicVolume(float volume)
{
    #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // Qt6版本
        if (musicAudioOutput) {
            musicAudioOutput->setVolume(qBound(0.0f, volume, 1.0f)); // Qt6中音量范围是0.0-1.0
        }
    #else
        // Qt5版本
        if (backgroundMusicPlayer) {
            backgroundMusicPlayer->setVolume(qBound(0, int(volume * 100), 100)); // Qt5中音量范围是0-100
        }
    #endif
}

void SugarOilGameScene::setSoundVolume(float volume)
{
    if (soundEffect) {
        soundEffect->setVolume(volume);
    }
}

void SugarOilGameScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // 绘制背景
    painter->drawPixmap(rect.toRect(), backgroundPixmap, backgroundPixmap.rect());
    
    // 绘制网格（可选）
    if (false) { // 调试时可以启用
        painter->setPen(QPen(QColor(255, 255, 255, 50), 1));
        for (int x = 0; x < SUGAR_OIL_SCENE_WIDTH; x += 50) {
            painter->drawLine(x, 0, x, SUGAR_OIL_SCENE_HEIGHT);
        }
        for (int y = 0; y < SUGAR_OIL_SCENE_HEIGHT; y += 50) {
            painter->drawLine(0, y, SUGAR_OIL_SCENE_WIDTH, y);
        }
    }
}

void SugarOilGameScene::onPlayerDamaged()
{
    // 玩家受伤时的处理
    playSound("Cough_sound.wav");
}

void SugarOilGameScene::onPlayerHealed()
{
    // 玩家治疗时的处理
    playSound("Bean_sound_short_2.wav");
}

void SugarOilGameScene::onItemPickedUp(ItemType itemType)
{
    emit itemCollected(itemType);
}

void SugarOilGameScene::onCreatureMet(CreatureType creatureType)
{
    emit creatureEncountered(creatureType);
}