#include "creature.h"
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

Creature::Creature(CreatureType type, QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , creatureType(type)
    , active(true)
    , friendly(true)
    , encountered(false)
    , velocity(0, 0)
    , speed(2.0f)
    , movementAngle(0.0f)
    , isMoving(false)
    , targetPosition(0, 0)
    , movementDirection(0, 0)
    , abilityActive(false)
    , abilityDuration(0)
    , abilityCooldown(0)
    , movementTimer(nullptr)
    , animationTimer(nullptr)
    , abilityTimer(nullptr)
    , lifetimeTimer(nullptr)
    , animationFrame(0)
    , animationState(0)
    , appearAnimation(nullptr)
    , disappearAnimation(nullptr)
    , encounterAnimation(nullptr)
    , pulseAnimation(nullptr)
    , currentEffect(nullptr)
    , lifetime(DEFAULT_LIFETIME)
{
    initializeCreature();
    // loadCreatureImages() call removed - function not declared in header
    // updateCreatureImage function removed
    setupTimers();
    
    // 设置变换原点为中心
    setTransformOriginPoint(boundingRect().center());
    
    // 设置生命周期 - startLifeTimer function removed
}

Creature::~Creature()
{
    // 清理定时器
    if (movementTimer) movementTimer->stop();
    if (animationTimer) animationTimer->stop();
    if (abilityTimer) abilityTimer->stop();
    if (lifetimeTimer) lifetimeTimer->stop();
}

void Creature::initializeCreature()
{
    switch (creatureType) {
    case CREATURE_HELPER:
        // health and maxHealth variables don't exist
        speed = 2.0f * 0.8f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
        
    case CREATURE_GUARDIAN:
        // health and maxHealth variables don't exist
        speed = 2.0f * 0.6f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
        
    case CREATURE_HEALER:
        // health and maxHealth variables don't exist
        speed = 2.0f * 1.2f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
        
    case CREATURE_SPEEDSTER:
        // health and maxHealth variables don't exist
        speed = 2.0f * 2.0f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
        
    case CREATURE_WARRIOR:
        // health and maxHealth variables don't exist
        speed = 2.0f * 0.9f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
        
    default:
        // health and maxHealth variables don't exist
        speed = 2.0f;
        friendly = true;
        // canMove and abilityValue variables don't exist
        break;
    }
}

// loadCreatureImages function removed - not declared in header

// getCreatureImageName and getCreatureColor functions removed - not declared in header

// createDefaultPixmap function removed - not declared in header

void Creature::setupTimers()
{
    // 移动定时器
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &Creature::onMovementUpdate);
    
    // 动画定时器
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Creature::onAnimationUpdate);
    
    // 能力定时器
    abilityTimer = new QTimer(this);
    abilityTimer->setSingleShot(true);
    connect(abilityTimer, &QTimer::timeout, this, &Creature::activateAbility);
    
    // 生命周期定时器
    lifetimeTimer = new QTimer(this);
    lifetimeTimer->setSingleShot(true);
    connect(lifetimeTimer, &QTimer::timeout, this, [this]() {
        if (!encountered) {
            // disappear() call removed - function not declared in header
            active = false;
        }
    });
}

void Creature::encounter()
{
    if (encountered || !active) return;
    
    encountered = true;
    
    // 播放遭遇动画
    startEncounterAnimation();
    
    // 激活能力
    activateAbility();
    
    emit creatureEncountered(this);
}

// disappear function removed - not declared in header

void Creature::activateAbility()
{
    if (!active) return;
    
    // 播放能力动画
    playAbilityAnimation();
    
    emit abilityActivated(creatureType, 100); // 默认能力值
}

void Creature::setPosition(const QPointF& position)
{
    setPos(position);
    // lastPosition variable removed
}

QPointF Creature::getCenter() const
{
    return pos() + boundingRect().center();
}

void Creature::moveTowardsPlayer(const QPointF& playerPosition)
{
    if (!active || encountered) return;
    
    targetPosition = playerPosition;
    
    if (!movementTimer->isActive()) {
        movementTimer->start(16); // 约60FPS
    }
}

void Creature::moveRandomly()
{
    if (!active || encountered) return;
    
    // 每隔一段时间改变随机移动方向
    static int frameCount = 0;
    frameCount++;
    
    if (frameCount % 120 == 0) { // 每2秒改变一次方向
        float angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0f;
        movementDirection = QPointF(cos(angle), sin(angle));
    }
    
    if (!movementTimer->isActive()) {
        movementTimer->start(16);
    }
}

// updateMovement function removed - not declared in header

// updateCreatureImage function removed - not declared in header

// isMoving函数已移除，直接使用成员变量

void Creature::startIdleAnimation()
{
    if (active) {
        animationTimer->start(300); // 每0.3秒更新动画帧
    }
}

void Creature::stopAllAnimations()
{
    if (movementTimer) movementTimer->stop();
    if (animationTimer) animationTimer->stop();
    if (abilityTimer) abilityTimer->stop();
}

void Creature::onAnimationUpdate()
{
    if (!active) return;
    
    animationFrame++;
    
    // 根据生物类型播放不同的动画
    switch (creatureType) {
    case CREATURE_HELPER:
        // 助手：轻微上下浮动
        {
            float offset = 3.0f * sin(animationFrame * 0.2f);
            QPointF currentPos = pos();
            setPos(currentPos.x(), currentPos.y() + offset);
        }
        break;
        
    case CREATURE_GUARDIAN:
        // 守护者：光环效果（缩放）
        {
            float scale = 1.0f + 0.1f * sin(animationFrame * 0.1f);
            setScale(scale);
        }
        break;
        
    case CREATURE_HEALER:
        // 治疗师：绿色光芒（透明度变化）
        {
            float opacity = 0.8f + 0.2f * sin(animationFrame * 0.3f);
            setOpacity(opacity);
        }
        break;
        
    case CREATURE_SPEEDSTER:
        // 速度者：快速移动残影（位置抖动）
        {
            float offsetX = 2.0f * sin(animationFrame * 0.5f);
            float offsetY = 2.0f * cos(animationFrame * 0.5f);
            QPointF currentPos = pos();
            setPos(currentPos.x() + offsetX, currentPos.y() + offsetY);
        }
        break;
        
    case CREATURE_WARRIOR:
        // 战士：威武姿态（轻微旋转）
        {
            float rotation = 5.0f * sin(animationFrame * 0.1f);
            setRotation(rotation);
        }
        break;
    }
    
    // updateCreatureImage() call removed - function not declared in header
}

// startLifeTimer function removed - lifetime is now set in constructor

void Creature::playAppearAnimation()
{
    // 创建出现动画：从小到大 + 淡入
    setScale(0.1);
    setOpacity(0.0);
    
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setDuration(500);
    scaleAnimation->setStartValue(0.1);
    scaleAnimation->setEndValue(1.0);
    scaleAnimation->setEasingCurve(QEasingCurve::OutBounce);
    
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    
    QParallelAnimationGroup* appearGroup = new QParallelAnimationGroup(this);
    appearGroup->addAnimation(scaleAnimation);
    appearGroup->addAnimation(fadeAnimation);
    
    connect(appearGroup, &QAbstractAnimation::finished, [this]() {
        // 出现动画完成后开始其他动画
        startIdleAnimation();
        moveRandomly();
    });
    
    // 临时显示出现图片
    if (!idleSprites.isEmpty()) {
        setPixmap(idleSprites[0]);
    }
    
    appearGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

// playEncounterAnimation function removed - not declared in header

void Creature::playAbilityAnimation()
{
    // 创建能力动画：根据生物类型播放不同效果
    QPropertyAnimation* abilityAnimation = nullptr;
    
    switch (creatureType) {
    case CREATURE_HELPER:
        // 助手：金色光芒
        abilityAnimation = new QPropertyAnimation(this, "rotation");
        abilityAnimation->setDuration(600);
        abilityAnimation->setStartValue(0);
        abilityAnimation->setEndValue(360);
        break;
        
    case CREATURE_GUARDIAN:
        // 守护者：蓝色护盾
        abilityAnimation = new QPropertyAnimation(this, "scale");
        abilityAnimation->setDuration(800);
        abilityAnimation->setStartValue(1.0);
        abilityAnimation->setKeyValueAt(0.5, 2.0);
        abilityAnimation->setEndValue(1.0);
        break;
        
    case CREATURE_HEALER:
        // 治疗师：绿色治疗光
        abilityAnimation = new QPropertyAnimation(this, "opacity");
        abilityAnimation->setDuration(600);
        abilityAnimation->setStartValue(1.0);
        abilityAnimation->setKeyValueAt(0.5, 0.3);
        abilityAnimation->setEndValue(1.0);
        abilityAnimation->setLoopCount(3);
        break;
        
    case CREATURE_SPEEDSTER:
        {
            // 速度者：快速闪烁
            abilityAnimation = new QPropertyAnimation(this, "pos");
            abilityAnimation->setDuration(400);
            QPointF originalPos = pos();
            abilityAnimation->setStartValue(originalPos);
            abilityAnimation->setKeyValueAt(0.25, originalPos + QPointF(10, 0));
            abilityAnimation->setKeyValueAt(0.5, originalPos + QPointF(-10, 0));
            abilityAnimation->setKeyValueAt(0.75, originalPos + QPointF(0, 10));
            abilityAnimation->setEndValue(originalPos);
            break;
        }
        
    case CREATURE_WARRIOR:
        // 战士：红色战斗光环
        abilityAnimation = new QPropertyAnimation(this, "scale");
        abilityAnimation->setDuration(500);
        abilityAnimation->setStartValue(1.0);
        abilityAnimation->setKeyValueAt(0.3, 1.3);
        abilityAnimation->setKeyValueAt(0.7, 0.8);
        abilityAnimation->setEndValue(1.0);
        break;
    }
    
    if (abilityAnimation) {
        connect(abilityAnimation, &QAbstractAnimation::finished, [this]() {
            setScale(1.0);
            setOpacity(1.0);
            setRotation(0);
            // updateCreatureImage() call removed - function not declared in header
        });
        
        // 临时显示能力图片
        if (!activeSprites.isEmpty()) {
            setPixmap(activeSprites[0]);
        }
        
        abilityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

// playDisappearAnimation function removed - not declared in header

// Getter methods
// getCreatureType is inline defined in header

void Creature::onEncounterAnimationFinished()
{
    // 遭遇动画完成后的处理
    // 可以在这里添加遭遇后的逻辑
}

void Creature::onAppearAnimationFinished()
{
    // 出现动画完成后的处理
    active = true;
}

void Creature::onDisappearAnimationFinished()
{
    // 消失动画完成后的处理
    active = false;
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

void Creature::onLifetimeTimeout()
{
    // 生命周期超时处理
    active = false;
    // 开始消失动画或直接移除
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

void Creature::startEncounterAnimation()
{
    // 开始遭遇动画
    // 这里可以添加动画效果
    // 动画完成后会调用 onEncounterAnimationFinished()
}

void Creature::onMovementUpdate()
{
    // 移动更新逻辑
    if (!active) return;
    
    // 这里可以添加生物的移动逻辑
    // 例如随机移动、追踪玩家等
}

void Creature::onAbilityTimeout()
{
    // 能力超时处理
    // 这里可以添加能力结束后的逻辑
}

QRectF Creature::boundingRect() const
{
    return QRectF(0, 0, CREATURE_SIZE, CREATURE_SIZE);
}

// Getter methods
// These functions are inline defined in header:
// getMovementSpeed, isActive, isEncountered, isFriendly

// getTargetPosition function removed - not declared in header

void Creature::pauseMovement()
{
    if (movementTimer && movementTimer->isActive()) {
        movementTimer->stop();
    }
}

void Creature::resumeMovement()
{
    if (movementTimer && !movementTimer->isActive() && active) {
        movementTimer->start(16); // 约60FPS
    }
}