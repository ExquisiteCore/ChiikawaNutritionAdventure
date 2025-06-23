#include "sugar_oil_enemy.h"
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

SugarOilEnemy::SugarOilEnemy(SugarOilEnemyType type, QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , enemyType(type)
    , health(1)
    , maxHealth(1)
    , damage(1)
    , speed(SUGAR_OIL_ENEMY_SPEED)
    , direction(SUGAR_OIL_DIR_DOWN)
    , active(true)
    , frozen(false)
    , attacking(false)
    , movementTimer(nullptr)
    , animationTimer(nullptr)
    , attackTimer(nullptr)
    , freezeTimer(nullptr)
    , currentAnimationFrame(0)
    , targetPosition(0, 0)
    , lastPosition(0, 0)
{
    initializeEnemy();
    setupTimers();
    loadEnemyImages();
    updateEnemyImage();
    startMovementAnimation();
}

SugarOilEnemy::~SugarOilEnemy()
{
    // 清理定时器
    if (movementTimer) movementTimer->stop();
    if (animationTimer) animationTimer->stop();
    if (attackTimer) attackTimer->stop();
    if (freezeTimer) freezeTimer->stop();
}

void SugarOilEnemy::initializeEnemy()
{
    switch (enemyType) {
    case ENEMY_FRIED_CHICKEN:
        health = maxHealth = 1;
        damage = 1;
        speed = SUGAR_OIL_ENEMY_SPEED * 1.5f;
        break;
    case ENEMY_BARBECUE:
        health = maxHealth = 3;
        damage = 2;
        speed = SUGAR_OIL_ENEMY_SPEED * 0.8f;
        break;
    case ENEMY_MILK_TEA:
        health = maxHealth = 5;
        damage = 3;
        speed = SUGAR_OIL_ENEMY_SPEED * 0.6f;
        break;
    case ENEMY_SNAIL_NOODLES:
        health = maxHealth = 1;
        damage = 1;
        speed = SUGAR_OIL_ENEMY_SPEED * 1.2f;
        break;
    case ENEMY_CAKE:
        health = maxHealth = 4;
        damage = 2;
        speed = SUGAR_OIL_ENEMY_SPEED * 0.5f;
        break;
    default:
        health = maxHealth = 1;
        damage = 1;
        speed = SUGAR_OIL_ENEMY_SPEED;
        break;
    }
}

void SugarOilEnemy::loadEnemyImages()
{
    QString basePath = ":/img/";
    
    switch (enemyType) {
    case ENEMY_FRIED_CHICKEN:
        enemyImages["idle"] = QPixmap(basePath + "enemy_fried_chicken_idle.png");
        enemyImages["move"] = QPixmap(basePath + "enemy_fried_chicken_move.png");
        enemyImages["attack"] = QPixmap(basePath + "enemy_fried_chicken_attack.png");
        enemyImages["damaged"] = QPixmap(basePath + "enemy_fried_chicken_damaged.png");
        enemyImages["frozen"] = QPixmap(basePath + "enemy_fried_chicken_frozen.png");
        break;
    case ENEMY_BARBECUE:
        enemyImages["idle"] = QPixmap(basePath + "enemy_barbecue_idle.png");
        enemyImages["move"] = QPixmap(basePath + "enemy_barbecue_move.png");
        enemyImages["attack"] = QPixmap(basePath + "enemy_barbecue_attack.png");
        enemyImages["damaged"] = QPixmap(basePath + "enemy_barbecue_damaged.png");
        enemyImages["frozen"] = QPixmap(basePath + "enemy_barbecue_frozen.png");
        break;
    case ENEMY_MILK_TEA:
        enemyImages["idle"] = QPixmap(basePath + "enemy_milk_tea_idle.png");
        enemyImages["move"] = QPixmap(basePath + "enemy_milk_tea_move.png");
        enemyImages["attack"] = QPixmap(basePath + "enemy_milk_tea_attack.png");
        enemyImages["damaged"] = QPixmap(basePath + "enemy_milk_tea_damaged.png");
        enemyImages["frozen"] = QPixmap(basePath + "enemy_milk_tea_frozen.png");
        break;
    case ENEMY_SNAIL_NOODLES:
        enemyImages["idle"] = QPixmap(basePath + "enemy_snail_noodles_idle.png");
        enemyImages["move"] = QPixmap(basePath + "enemy_snail_noodles_move.png");
        enemyImages["attack"] = QPixmap(basePath + "enemy_snail_noodles_attack.png");
        enemyImages["damaged"] = QPixmap(basePath + "enemy_snail_noodles_damaged.png");
        enemyImages["frozen"] = QPixmap(basePath + "enemy_snail_noodles_frozen.png");
        break;
    case ENEMY_CAKE:
        enemyImages["idle"] = QPixmap(basePath + "enemy_cake_idle.png");
        enemyImages["move"] = QPixmap(basePath + "enemy_cake_move.png");
        enemyImages["attack"] = QPixmap(basePath + "enemy_cake_attack.png");
        enemyImages["damaged"] = QPixmap(basePath + "enemy_cake_damaged.png");
        enemyImages["frozen"] = QPixmap(basePath + "enemy_cake_frozen.png");
        break;
    }
    
    // 如果图片加载失败，创建默认图片
    for (auto it = enemyImages.begin(); it != enemyImages.end(); ++it) {
        if (it.value().isNull()) {
            QColor enemyColor = getEnemyColor();
            it.value() = createDefaultPixmap(SUGAR_OIL_ENEMY_SIZE, SUGAR_OIL_ENEMY_SIZE, enemyColor);
        }
    }
}

QColor SugarOilEnemy::getEnemyColor() const
{
    switch (enemyType) {
    case ENEMY_FRIED_CHICKEN:
        return QColor(255, 100, 100); // 红色
    case ENEMY_BARBECUE:
        return QColor(100, 100, 255); // 蓝色
    case ENEMY_MILK_TEA:
        return QColor(128, 0, 128); // 紫色
    case ENEMY_SNAIL_NOODLES:
        return QColor(255, 165, 0); // 橙色
    case ENEMY_CAKE:
        return QColor(128, 128, 128); // 灰色
    default:
        return QColor(255, 0, 0); // 默认红色
    }
}

QPixmap SugarOilEnemy::createDefaultPixmap(int width, int height, const QColor& color)
{
    QPixmap pixmap(width, height);
    pixmap.fill(color);
    return pixmap;
}

void SugarOilEnemy::setupTimers()
{
    // 移动定时器
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &SugarOilEnemy::updateMovement);
    
    // 动画定时器
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &SugarOilEnemy::updateAnimation);
    
    // 攻击定时器
    attackTimer = new QTimer(this);
    attackTimer->setSingleShot(true);
    connect(attackTimer, &QTimer::timeout, this, [this]() {
        attacking = false;
        updateEnemyImage();
    });
    
    // 冰冻定时器
    freezeTimer = new QTimer(this);
    freezeTimer->setSingleShot(true);
    connect(freezeTimer, &QTimer::timeout, this, [this]() {
        unfreeze();
    });
}

void SugarOilEnemy::setTarget(const QPointF& target)
{
    targetPosition = target;
}

void SugarOilEnemy::updateMovement()
{
    if (!active || frozen) return;
    
    QPointF currentPos = pos();
    QPointF direction = targetPosition - currentPos;
    
    // 计算移动距离
    float distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 1.0f) {
        // 归一化方向向量
        direction /= distance;
        
        // 应用速度
        QPointF newPos = currentPos + direction * speed;
        
        // 更新方向枚举
        updateDirection(direction);
        
        setPos(newPos);
        lastPosition = currentPos;
    }
}

void SugarOilEnemy::updateDirection(const QPointF& directionVector)
{
    // 根据移动方向更新方向枚举
    if (abs(directionVector.x()) > abs(directionVector.y())) {
        if (directionVector.x() > 0) {
            direction = SUGAR_OIL_DIR_RIGHT;
        } else {
            direction = SUGAR_OIL_DIR_LEFT;
        }
    } else {
        if (directionVector.y() > 0) {
            direction = SUGAR_OIL_DIR_DOWN;
        } else {
            direction = SUGAR_OIL_DIR_UP;
        }
    }
}

void SugarOilEnemy::takeDamage(int damage)
{
    if (!active) return;
    
    health -= damage;
    if (health < 0) health = 0;
    
    // 播放受伤动画
    playDamageAnimation();
    
    emit enemyDamaged(damage);
    
    if (health <= 0) {
        destroy();
    }
}

void SugarOilEnemy::destroy()
{
    if (!active) return;
    
    active = false;
    
    // 播放死亡动画
    playDeathAnimation();
    
    emit enemyDestroyed(this);
}

void SugarOilEnemy::freeze(int duration)
{
    if (!active) return;
    
    frozen = true;
    movementTimer->stop();
    
    if (duration > 0) {
        freezeTimer->start(duration);
    }
    
    updateEnemyImage();
    emit enemyFrozen();
}

void SugarOilEnemy::unfreeze()
{
    if (!frozen) return;
    
    frozen = false;
    freezeTimer->stop();
    
    if (active) {
        movementTimer->start(16); // 约60FPS
    }
    
    updateEnemyImage();
    emit enemyUnfrozen();
}

void SugarOilEnemy::attack()
{
    if (!active || frozen || attacking) return;
    
    attacking = true;
    attackTimer->start(500); // 攻击动画持续0.5秒
    
    updateEnemyImage();
    
    // 发射攻击信号
    emit playerHit(damage);
    emit enemyAttacked();
}

void SugarOilEnemy::setPosition(const QPointF& position)
{
    setPos(position);
    lastPosition = position;
}

QPointF SugarOilEnemy::getCenter() const
{
    return pos() + boundingRect().center();
}

void SugarOilEnemy::updateEnemyImage()
{
    QString imageKey = "idle";
    
    if (frozen) {
        imageKey = "frozen";
    } else if (attacking) {
        imageKey = "attack";
    } else if (isMoving()) {
        imageKey = "move";
    }
    
    setPixmap(enemyImages[imageKey]);
}

bool SugarOilEnemy::isMoving() const
{
    QPointF currentPos = pos();
    return QLineF(lastPosition, currentPos).length() > 0.1f;
}

void SugarOilEnemy::startMovementAnimation()
{
    if (active && !frozen) {
        movementTimer->start(16); // 约60FPS
        animationTimer->start(200); // 每0.2秒更新动画帧
    }
}

void SugarOilEnemy::pauseAnimation()
{
    movementTimer->stop();
    animationTimer->stop();
}

void SugarOilEnemy::resumeAnimation()
{
    if (active && !frozen) {
        movementTimer->start(16);
        animationTimer->start(200);
    }
}

void SugarOilEnemy::updateAnimation()
{
    if (!active) return;
    
    currentAnimationFrame++;
    
    // 简单的动画效果：轻微旋转或缩放
    switch (enemyType) {
    case ENEMY_FRIED_CHICKEN:
        // 快速敌人：快速旋转
        setRotation(currentAnimationFrame * 5);
        break;
    case ENEMY_BARBECUE:
        // 强壮敌人：轻微缩放
        {
            float scale = 1.0f + 0.1f * sin(currentAnimationFrame * 0.2f);
            setScale(scale);
        }
        break;
    case ENEMY_MILK_TEA:
        // Boss：复杂动画
        {
            float scale = 1.0f + 0.05f * sin(currentAnimationFrame * 0.1f);
            setScale(scale);
            setRotation(sin(currentAnimationFrame * 0.05f) * 10);
            QPointF currentPos = pos();
            float offset = 5.0f * sin(currentAnimationFrame * 0.3f);
            setPos(currentPos.x(), lastPosition.y() + offset);
        }
        break;
    default:
        // 其他敌人：稳定，无特殊动画
        break;
    }
}

void SugarOilEnemy::playDamageAnimation()
{
    // 创建受伤动画：红色闪烁
    QPropertyAnimation* flashAnimation = new QPropertyAnimation(this, "opacity");
    flashAnimation->setDuration(150);
    flashAnimation->setStartValue(1.0);
    flashAnimation->setKeyValueAt(0.5, 0.3);
    flashAnimation->setEndValue(1.0);
    flashAnimation->setLoopCount(2);
    
    connect(flashAnimation, &QAbstractAnimation::finished, [this]() {
        setOpacity(1.0);
        updateEnemyImage();
    });
    
    flashAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    
    // 临时显示受伤图片
    setPixmap(enemyImages["damaged"]);
}

void SugarOilEnemy::playDeathAnimation()
{
    // 创建死亡动画：淡出 + 旋转
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    
    QPropertyAnimation* rotateAnimation = new QPropertyAnimation(this, "rotation");
    rotateAnimation->setDuration(500);
    rotateAnimation->setStartValue(0);
    rotateAnimation->setEndValue(360);
    
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setDuration(500);
    scaleAnimation->setStartValue(1.0);
    scaleAnimation->setEndValue(0.1);
    
    QParallelAnimationGroup* deathGroup = new QParallelAnimationGroup(this);
    deathGroup->addAnimation(fadeAnimation);
    deathGroup->addAnimation(rotateAnimation);
    deathGroup->addAnimation(scaleAnimation);
    
    connect(deathGroup, &QAbstractAnimation::finished, [this]() {
        // 动画完成后，敌人将被场景删除
    });
    
    deathGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void SugarOilEnemy::playAttackAnimation()
{
    // 创建攻击动画：快速放大缩小
    QPropertyAnimation* attackAnimation = new QPropertyAnimation(this, "scale");
    attackAnimation->setDuration(200);
    attackAnimation->setStartValue(1.0);
    attackAnimation->setKeyValueAt(0.5, 1.3);
    attackAnimation->setEndValue(1.0);
    
    connect(attackAnimation, &QAbstractAnimation::finished, [this]() {
        setScale(1.0);
    });
    
    attackAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

// Getter methods
int SugarOilEnemy::getHealth() const
{
    return health;
}

int SugarOilEnemy::getMaxHealth() const
{
    return maxHealth;
}



SugarOilDirection SugarOilEnemy::getDirection() const
{
    return direction;
}



bool SugarOilEnemy::isAttacking() const
{
    return attacking;
}

QPointF SugarOilEnemy::getTargetPosition() const
{
    return targetPosition;
}

void SugarOilEnemy::onDestroyAnimationFinished()
{
    // 销毁动画完成后的处理
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

void SugarOilEnemy::onSpecialAbilityTimeout()
{
    // 特殊能力超时处理
    // 这里可以添加特殊能力结束后的逻辑
}

void SugarOilEnemy::onAnimationUpdate()
{
    // 动画更新处理
    // 这里可以添加敌人特有的动画更新逻辑
}

void SugarOilEnemy::onMovementUpdate()
{
    // 移动更新处理
    if (!active) return;
    
    // 这里可以添加敌人的移动逻辑
    // 例如巡逻、追踪玩家等
}

void SugarOilEnemy::onFreezeTimeout()
{
    // 冰冻超时处理
    // 这里可以添加冰冻效果结束后的逻辑
}

QRectF SugarOilEnemy::boundingRect() const
{
    return QRectF(0, 0, SUGAR_OIL_ENEMY_SIZE, SUGAR_OIL_ENEMY_SIZE);
}