#include "usagi_player.h"
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>

UsagiPlayer::UsagiPlayer(QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , lives(USAGI_INITIAL_LIVES)
    , score(0)
    , health(100)
    , maxHealth(100)
    , scoreMultiplier(1.0f)
    , baseSpeed(USAGI_SPEED)
    , speedMultiplier(1.0f)
    , currentDirection(SUGAR_OIL_DIR_NONE)
    , isMoving(false)
    , invincible(false)
    , shieldActive(false)
    , magnetActive(false)
    , rapidFireActive(false)
    , isBlinking(false)
    , movementTimer(nullptr)
    , animationTimer(nullptr)
    , invincibilityTimer(nullptr)
    , shieldTimer(nullptr)
    , speedBoostTimer(nullptr)
    , scoreBoostTimer(nullptr)
    , magnetTimer(nullptr)
    , rapidFireTimer(nullptr)
    , blinkTimer(nullptr)
    , effectsTimer(nullptr)
    , animationFrame(0)
    , animationDirection(1)
    , currentAnimationFrame(0)
{
    // 加载玩家图片
    loadPlayerImages();
    
    // 设置初始图片
    setPixmap(playerImages["idle"]);
    
    // 设置变换原点为中心
    setTransformOriginPoint(boundingRect().center());
    
    // 初始化定时器
    setupTimers();
    
    // 启动空闲动画
    startIdleAnimation();
}

UsagiPlayer::~UsagiPlayer()
{
    // 清理定时器
    if (movementTimer) movementTimer->stop();
    if (animationTimer) animationTimer->stop();
    if (invincibilityTimer) invincibilityTimer->stop();
    if (shieldTimer) shieldTimer->stop();
    if (magnetTimer) magnetTimer->stop();
    if (speedBoostTimer) speedBoostTimer->stop();
    if (scoreBoostTimer) scoreBoostTimer->stop();
    if (blinkTimer) blinkTimer->stop();
}

void UsagiPlayer::loadPlayerImages()
{
    // 加载不同状态的玩家图片 - 修正资源路径
    playerImages["idle"] = QPixmap(":/img/roles/usagi1.png");
    playerImages["move_up"] = QPixmap(":/img/roles/usagi2.png");
    playerImages["move_down"] = QPixmap(":/img/roles/usagi3.png");
    playerImages["move_left"] = QPixmap(":/img/roles/usagi1-mir.png");
    playerImages["move_right"] = QPixmap(":/img/roles/usagi1.png");
    playerImages["damaged"] = QPixmap(":/img/roles/usagi2.png");
    playerImages["shield"] = QPixmap(":/img/roles/usagi3.png");
    playerImages["invincible"] = QPixmap(":/img/roles/usagi1-invincible.png");
    
    // 如果图片加载失败，创建默认图片
    for (auto it = playerImages.begin(); it != playerImages.end(); ++it) {
        if (it.value().isNull()) {
            it.value() = createDefaultPixmap(USAGI_SIZE, USAGI_SIZE, QColor(255, 192, 203)); // 粉色
        }
    }
}

QPixmap UsagiPlayer::createDefaultPixmap(int width, int height, const QColor& color)
{
    QPixmap pixmap(width, height);
    pixmap.fill(color);
    return pixmap;
}

void UsagiPlayer::setupTimers()
{
    // 移动定时器
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &UsagiPlayer::updateMovement);
    
    // 动画定时器
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &UsagiPlayer::updateAnimation);
    
    // 无敌状态定时器
    invincibilityTimer = new QTimer(this);
    invincibilityTimer->setSingleShot(true);
    connect(invincibilityTimer, &QTimer::timeout, this, [this]() {
        setInvincible(false);
    });
    
    // 护盾定时器
    shieldTimer = new QTimer(this);
    shieldTimer->setSingleShot(true);
    connect(shieldTimer, &QTimer::timeout, this, [this]() {
        deactivateShield();
    });
    
    // 磁铁定时器
    magnetTimer = new QTimer(this);
    magnetTimer->setSingleShot(true);
    connect(magnetTimer, &QTimer::timeout, this, [this]() {
        deactivateMagnet();
    });
    
    // 速度提升定时器
    speedBoostTimer = new QTimer(this);
    speedBoostTimer->setSingleShot(true);
    connect(speedBoostTimer, &QTimer::timeout, this, [this]() {
        setSpeedMultiplier(1.0f);
    });
    
    // 分数提升定时器
    scoreBoostTimer = new QTimer(this);
    scoreBoostTimer->setSingleShot(true);
    connect(scoreBoostTimer, &QTimer::timeout, this, [this]() {
        setScoreMultiplier(1.0f);
    });
    
    // 闪烁定时器
    blinkTimer = new QTimer(this);
    connect(blinkTimer, &QTimer::timeout, this, &UsagiPlayer::updateBlinking);
}

void UsagiPlayer::setDirection(SugarOilDirection dir)
{
    currentDirection = dir;
    
    if (dir != SUGAR_OIL_DIR_NONE) {
        if (!isMoving) {
            move();
        }
        updateSprite();
    }
}

void UsagiPlayer::startMovement()
{
    if (!isMoving) {
        isMoving = true;
        movementTimer->start(25); // 约40FPS，减少卡顿
        startWalkAnimation();
    }
}

void UsagiPlayer::stopMovement()
{
    if (isMoving) {
        isMoving = false;
        currentDirection = SUGAR_OIL_DIR_NONE;
        movementTimer->stop();
        startIdleAnimation();
        updateSprite();
    }
}

void UsagiPlayer::updateMovement()
{
    if (!isMoving || currentDirection == SUGAR_OIL_DIR_NONE) return;
    
    QPointF currentPos = pos();
    QPointF newPos = currentPos;
    
    float actualSpeed = baseSpeed * speedMultiplier;
    
    switch (currentDirection) {
    case SUGAR_OIL_DIR_UP:
        newPos.setY(currentPos.y() - actualSpeed);
        break;
    case SUGAR_OIL_DIR_DOWN:
        newPos.setY(currentPos.y() + actualSpeed);
        break;
    case SUGAR_OIL_DIR_LEFT:
        newPos.setX(currentPos.x() - actualSpeed);
        break;
    case SUGAR_OIL_DIR_RIGHT:
        newPos.setX(currentPos.x() + actualSpeed);
        break;
    default:
        break;
    }
    
    setPos(newPos);
}

void UsagiPlayer::updatePosition()
{
    // 这个方法由场景调用，用于更新玩家位置
    // 实际的移动逻辑在updateMovement()中
}

void UsagiPlayer::constrainToScene(const QRectF& sceneRect)
{
    QPointF currentPos = pos();
    QRectF playerRect = boundingRect().translated(currentPos);
    const int borderWidth = 8; // 与场景边界宽度保持一致
    
    // 限制玩家在场景边界内（考虑边界宽度）
    if (playerRect.left() < sceneRect.left() + borderWidth) {
        currentPos.setX(sceneRect.left() + borderWidth);
    } else if (playerRect.right() > sceneRect.right() - borderWidth) {
        currentPos.setX(sceneRect.right() - borderWidth - boundingRect().width());
    }
    
    if (playerRect.top() < sceneRect.top() + borderWidth) {
        currentPos.setY(sceneRect.top() + borderWidth);
    } else if (playerRect.bottom() > sceneRect.bottom() - borderWidth) {
        currentPos.setY(sceneRect.bottom() - borderWidth - boundingRect().height());
    }
    
    setPos(currentPos);
}

void UsagiPlayer::setPosition(const QPointF& position)
{
    setPos(position);
}

QPointF UsagiPlayer::getCenter() const
{
    return pos() + boundingRect().center();
}

void UsagiPlayer::takeDamage(int damage)
{
    if (invincible || shieldActive) {
        return; // 无敌或有护盾时不受伤害
    }
    
    health -= damage;
    if (health < 0) health = 0;
    
    // 播放受伤动画
    playDamageAnimation();
    
    // 短暂无敌
    setInvincible(true, 1000); // 1秒无敌
    
    emit playerDamaged();
    emit healthChanged(health);
    
    // 检查是否死亡
    if (health <= 0) {
        lives--;
        emit livesChanged(lives);
        
        if (lives <= 0) {
            emit playerDied();
        } else {
            // 复活
            respawn();
        }
    }
}

void UsagiPlayer::heal(int amount)
{
    int oldHealth = health;
    health += amount;
    if (health > maxHealth) health = maxHealth;
    
    if (health > oldHealth) {
        playHealAnimation();
        emit playerHealed();
        emit healthChanged(health);
    }
}

void UsagiPlayer::addScore(int points)
{
    int actualPoints = static_cast<int>(points * scoreMultiplier);
    score += actualPoints;
    emit scoreChanged(score);
}

void UsagiPlayer::addLife()
{
    lives++;
    emit livesChanged(lives);
}

void UsagiPlayer::respawn()
{
    health = maxHealth;
    setInvincible(true, 3000); // 复活后3秒无敌
    
    // 重置位置到场景中心
    if (scene()) {
        QRectF sceneRect = scene()->sceneRect();
        setPosition(QPointF(sceneRect.width() / 2, sceneRect.height() / 2));
    }
    
    emit healthChanged(health);
}

void UsagiPlayer::setInvincible(bool invincible, int duration)
{
    this->invincible = invincible;
    
    if (invincible) {
        if (duration > 0) {
            invincibilityTimer->start(duration);
        }
        updateBlinking();
        updateSprite();
    } else {
        invincibilityTimer->stop();
        stopBlinking();
        updateSprite();
    }
}

void UsagiPlayer::activateShield(int duration)
{
    shieldActive = true;
    if (duration > 0) {
        shieldTimer->start(duration);
    }
    updateSprite();
    emit shieldActivated();
}

void UsagiPlayer::deactivateShield()
{
    shieldActive = false;
    shieldTimer->stop();
    updateSprite();
    emit shieldDeactivated();
}

void UsagiPlayer::activateMagnet(int duration)
{
    magnetActive = true;
    if (duration > 0) {
        magnetTimer->start(duration);
    }
    emit magnetActivated();
}

void UsagiPlayer::deactivateMagnet()
{
    magnetActive = false;
    magnetTimer->stop();
    emit magnetDeactivated();
}

void UsagiPlayer::setSpeedMultiplier(float multiplier)
{
    speedMultiplier = multiplier;
    
    if (multiplier > 1.0f) {
        speedBoostTimer->start(BUFF_DURATION);
        emit speedBoostActivated();
    } else {
        speedBoostTimer->stop();
        emit speedBoostDeactivated();
    }
}



void UsagiPlayer::updateSprite()
{
    QString imageKey = "idle";
    
    if (invincible) {
        imageKey = "invincible";
    } else if (shieldActive) {
        imageKey = "shield";
    } else if (isMoving) {
        switch (currentDirection) {
        case SUGAR_OIL_DIR_UP:
            imageKey = "move_up";
            break;
        case SUGAR_OIL_DIR_DOWN:
            imageKey = "move_down";
            break;
        case SUGAR_OIL_DIR_LEFT:
            imageKey = "move_left";
            break;
        case SUGAR_OIL_DIR_RIGHT:
            imageKey = "move_right";
            break;
        default:
            imageKey = "idle";
            break;
        }
    }
    
    setPixmap(playerImages[imageKey]);
}

void UsagiPlayer::startIdleAnimation()
{
    animationTimer->start(500); // 每0.5秒切换一次
}

void UsagiPlayer::startWalkAnimation()
{
    animationTimer->start(200); // 每0.2秒切换一次
}

void UsagiPlayer::pauseAnimation()
{
    animationTimer->stop();
}

void UsagiPlayer::resumeAnimation()
{
    if (isMoving) {
        startWalkAnimation();
    } else {
        startIdleAnimation();
    }
}

void UsagiPlayer::updateAnimation()
{
    // 简单的动画逻辑：轻微缩放
    currentAnimationFrame++;
    
    if (currentAnimationFrame % 10 == 0) {
        float scale = 1.0f + 0.05f * sin(currentAnimationFrame * 0.1f);
        setScale(scale);
    }
}

void UsagiPlayer::playDamageAnimation()
{
    // 创建受伤动画：红色闪烁 + 震动
    QPropertyAnimation* colorAnimation = new QPropertyAnimation(this, "opacity");
    colorAnimation->setDuration(200);
    colorAnimation->setStartValue(1.0);
    colorAnimation->setEndValue(0.5);
    colorAnimation->setLoopCount(3);
    
    QPropertyAnimation* shakeAnimation = new QPropertyAnimation(this, "pos");
    shakeAnimation->setDuration(100);
    QPointF originalPos = pos();
    shakeAnimation->setStartValue(originalPos);
    shakeAnimation->setKeyValueAt(0.25, originalPos + QPointF(5, 0));
    shakeAnimation->setKeyValueAt(0.5, originalPos + QPointF(-5, 0));
    shakeAnimation->setKeyValueAt(0.75, originalPos + QPointF(0, 5));
    shakeAnimation->setEndValue(originalPos);
    shakeAnimation->setLoopCount(2);
    
    QParallelAnimationGroup* damageGroup = new QParallelAnimationGroup(this);
    damageGroup->addAnimation(colorAnimation);
    damageGroup->addAnimation(shakeAnimation);
    
    connect(damageGroup, &QAbstractAnimation::finished, [this]() {
        setOpacity(1.0);
    });
    
    damageGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void UsagiPlayer::playHealAnimation()
{
    // 创建治疗动画：绿色光芒
    QPropertyAnimation* glowAnimation = new QPropertyAnimation(this, "scale");
    glowAnimation->setDuration(300);
    glowAnimation->setStartValue(1.0);
    glowAnimation->setKeyValueAt(0.5, 1.2);
    glowAnimation->setEndValue(1.0);
    
    connect(glowAnimation, &QAbstractAnimation::finished, [this]() {
        setScale(1.0);
    });
    
    glowAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void UsagiPlayer::startBlinking()
{
    if (!isBlinking) {
        isBlinking = true;
        blinkTimer->start(200); // 每0.2秒闪烁一次
    }
}

void UsagiPlayer::stopBlinking()
{
    if (isBlinking) {
        isBlinking = false;
        blinkTimer->stop();
        setOpacity(1.0);
    }
}

void UsagiPlayer::updateBlinking()
{
    if (isBlinking) {
        setOpacity(opacity() > 0.5 ? 0.3 : 1.0);
    }
}

void UsagiPlayer::move()
{
    // 移动函数实现
    if (currentDirection != SUGAR_OIL_DIR_NONE) {
        updateMovement();
    }
}

void UsagiPlayer::onInvincibilityTimeout()
{
    // 无敌状态超时处理
    setInvincible(false);
}

void UsagiPlayer::onShieldTimeout()
{
    // 护盾超时处理
    deactivateShield();
}

void UsagiPlayer::onSpeedBoostTimeout()
{
    // 速度提升超时处理
    setSpeedMultiplier(1.0f);
}

void UsagiPlayer::onMagnetTimeout()
{
    // 磁铁效果超时处理
    deactivateMagnet();
}

void UsagiPlayer::onRapidFireTimeout()
{
    // 快速射击超时处理
    // 这里可以添加快速射击结束的逻辑
}

void UsagiPlayer::updateEffects()
{
    // 更新特效
    // 这里可以添加各种视觉特效的更新逻辑
    // 例如粒子效果、光晕效果等
}

QRectF UsagiPlayer::boundingRect() const
{
    return QRectF(0, 0, USAGI_SIZE, USAGI_SIZE);
}