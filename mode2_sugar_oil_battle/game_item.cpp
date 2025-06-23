#include "game_item.h"
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QDebug>
#include <QRandomGenerator>
#include <QtMath>

GameItem::GameItem(ItemType type, QObject *parent)
    : QObject(parent)
    , QGraphicsPixmapItem()
    , itemType(type)
    , active(true)
    , magnetActive(false)
    , floatingAnimation(nullptr)
    , glowAnimation(nullptr)
    , magnetTimer(nullptr)
    , lifetimeTimer(nullptr)
    , floatingOffset(0.0f)
    , glowIntensity(1.0f)
    , magnetTarget(0, 0)
    , magnetSpeed(100.0f)
{
    initializeItem();
    loadItemImages();
    updateItemImage();
    setupTimers();
    
    // 设置变换原点为中心
    setTransformOriginPoint(boundingRect().center());
    
    // 设置生命周期 - startLifeTimer function removed
}

GameItem::~GameItem()
{
    // 清理动画
    if (floatingAnimation) {
        floatingAnimation->stop();
        delete floatingAnimation;
    }
    if (glowAnimation) {
        glowAnimation->stop();
        delete glowAnimation;
    }
    
    // 清理定时器
    if (magnetTimer) magnetTimer->stop();
    if (lifetimeTimer) lifetimeTimer->stop();
}

void GameItem::initializeItem()
{
    // 根据道具类型设置属性
    switch (itemType) {
    case ITEM_ENERGY_DRINK:
    case ITEM_PROTEIN_BAR:
    case ITEM_VITAMIN:
    case ITEM_MINERAL:
    case ITEM_FIBER:
        // 营养类
        scoreValue = 10;
        break;
        
    case ITEM_ANTIOXIDANT:
    case ITEM_OMEGA3:
    case ITEM_CALCIUM:
    case ITEM_IRON:
    case ITEM_ZINC:
    case ITEM_MULTIVITAMIN:
        // 高级营养类
        scoreValue = 15;
        break;
        
    case ITEM_SPEED_BOOST:
    case ITEM_DAMAGE_BOOST:
    case ITEM_SHIELD:
    case ITEM_HEALTH:
    case ITEM_SCORE_BONUS:
        // 基础增益类
        scoreValue = 12;
        break;
        
    case ITEM_TIME_SLOW:
    case ITEM_INVINCIBLE:
    case ITEM_DOUBLE_SCORE:
    case ITEM_EXTRA_LIFE:
    case ITEM_FREEZE_ENEMIES:
        // 高级增益类
        scoreValue = 20;
        break;
        
    case ITEM_MAGNET:
    case ITEM_BOMB:
    case ITEM_RAPID_FIRE:
        // 特殊道具
        scoreValue = 50;
        break;
        
    default:
        // value and rarity variables don't exist
        break;
    }
}

void GameItem::loadItemImages()
{
    QString basePath = ":/img/items/";
    QString itemName = getItemSpritePath();
    
    itemImages["normal"] = QPixmap(basePath + itemName + ".png");
    itemImages["glow"] = QPixmap(basePath + itemName + "_glow.png");
    itemImages["collected"] = QPixmap(basePath + itemName + "_collected.png");
    
    // 如果图片加载失败，创建默认图片
    for (auto it = itemImages.begin(); it != itemImages.end(); ++it) {
        if (it.value().isNull()) {
            QColor itemColor = Qt::blue; // 使用默认颜色
            it.value() = QPixmap(32, 32); // 创建默认大小的图片
            it.value().fill(itemColor);
        }
    }
}

// getItemImageName function removed - using getItemSpritePath instead

// getItemColor function removed

// createDefaultPixmap function removed

void GameItem::setupTimers()
{
    // 磁铁定时器
    magnetTimer = new QTimer(this);
    connect(magnetTimer, &QTimer::timeout, this, &GameItem::updateMagnetMovement);
    
    // 生命周期定时器
    lifetimeTimer = new QTimer(this);
    lifetimeTimer->setSingleShot(true);
    connect(lifetimeTimer, &QTimer::timeout, this, [this]() {
        if (active) {
            destroy();
        }
    });
}

void GameItem::collect()
{
    if (!active || !isActive()) return;
    
    active = false;
    
    // 播放收集动画
    playCollectAnimation();
    
    emit itemCollected(this);
}

void GameItem::destroy()
{
    if (!isActive()) return;
    
    active = false;
    
    // 播放消失动画
    playDestroyAnimation();
    
    emit itemDestroyed(this);
}

void GameItem::setPosition(const QPointF& position)
{
    setPos(position);
}

QPointF GameItem::getCenter() const
{
    return pos() + boundingRect().center();
}

void GameItem::updateItemImage()
{
    QString imageKey = "normal";
    
    if (!active) {
        imageKey = "collected";
    } else if (glowIntensity > 0.8f) {
        imageKey = "glow";
    }
    
    setPixmap(itemImages[imageKey]);
}

void GameItem::startFloatingAnimation()
{
    if (floatingAnimation) {
        floatingAnimation->stop();
        delete floatingAnimation;
    }
    
    floatingAnimation = new QPropertyAnimation(this, "pos");
    floatingAnimation->setDuration(2000 + QRandomGenerator::global()->bounded(1000)); // 2-3秒
    floatingAnimation->setLoopCount(-1); // 无限循环
    
    QPointF startPos = pos();
    QPointF endPos = startPos + QPointF(0, 10 + QRandomGenerator::global()->bounded(10));
    
    floatingAnimation->setStartValue(startPos);
    floatingAnimation->setKeyValueAt(0.5, endPos);
    floatingAnimation->setEndValue(startPos);
    floatingAnimation->setEasingCurve(QEasingCurve::InOutSine);
    
    floatingAnimation->start();
}

void GameItem::startGlowAnimation()
{
    if (glowAnimation) {
        glowAnimation->stop();
        delete glowAnimation;
    }
    
    glowAnimation = new QPropertyAnimation(this, "opacity");
    glowAnimation->setDuration(1000 + QRandomGenerator::global()->bounded(500)); // 1-1.5秒
    glowAnimation->setLoopCount(-1); // 无限循环
    
    glowAnimation->setStartValue(0.7);
    glowAnimation->setKeyValueAt(0.5, 1.0);
    glowAnimation->setEndValue(0.7);
    glowAnimation->setEasingCurve(QEasingCurve::InOutSine);
    
    connect(glowAnimation, &QPropertyAnimation::valueChanged, this, [this](const QVariant& value) {
        glowIntensity = value.toFloat();
        updateItemImage();
    });
    
    glowAnimation->start();
}

void GameItem::stopAllAnimations()
{
    if (floatingAnimation) {
        floatingAnimation->stop();
    }
    if (glowAnimation) {
        glowAnimation->stop();
    }
    if (magnetTimer) {
        magnetTimer->stop();
    }
}

void GameItem::setMagnetTarget(const QPointF& target)
{
    magnetTarget = target;
}

void GameItem::setMagnetActive(bool active)
{
    magnetActive = active;
    
    if (active) {
        magnetTimer->start(16); // 约60FPS
    } else {
        magnetTimer->stop();
    }
}

void GameItem::updateMagnetMovement()
{
    if (!magnetActive || !active || !isActive()) return;
    
    QPointF currentPos = pos();
    QPointF direction = magnetTarget - currentPos;
    
    float distance = sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 5.0f) {
        // 归一化方向向量
        direction /= distance;
        
        // 应用磁铁速度
        QPointF newPos = currentPos + direction * magnetSpeed;
        setPos(newPos);
        
        // 停止浮动动画，因为磁铁在控制移动
        if (floatingAnimation && floatingAnimation->state() == QAbstractAnimation::Running) {
            floatingAnimation->pause();
        }
    } else {
        // 距离很近时自动收集
        collect();
    }
}

// startLifeTimer function removed

void GameItem::playCollectAnimation()
{
    // 创建收集动画：放大 + 淡出 + 旋转
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setDuration(300);
    scaleAnimation->setStartValue(1.0);
    scaleAnimation->setEndValue(2.0);
    
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(300);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    
    QPropertyAnimation* rotateAnimation = new QPropertyAnimation(this, "rotation");
    rotateAnimation->setDuration(300);
    rotateAnimation->setStartValue(0);
    rotateAnimation->setEndValue(360);
    
    QParallelAnimationGroup* collectGroup = new QParallelAnimationGroup(this);
    collectGroup->addAnimation(scaleAnimation);
    collectGroup->addAnimation(fadeAnimation);
    collectGroup->addAnimation(rotateAnimation);
    
    connect(collectGroup, &QAbstractAnimation::finished, [this]() {
        // 动画完成后，道具将被场景删除
    });
    
    collectGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameItem::playDestroyAnimation()
{
    // 创建消失动画：淡出
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    
    connect(fadeAnimation, &QAbstractAnimation::finished, [this]() {
        // 动画完成后，道具将被场景删除
    });
    
    fadeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameItem::playSpawnAnimation()
{
    // 创建生成动画：从小到大
    setScale(0.1);
    setOpacity(0.5);
    
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
    scaleAnimation->setDuration(300);
    scaleAnimation->setStartValue(0.1);
    scaleAnimation->setEndValue(1.0);
    scaleAnimation->setEasingCurve(QEasingCurve::OutBounce);
    
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(this, "opacity");
    fadeAnimation->setDuration(300);
    fadeAnimation->setStartValue(0.5);
    fadeAnimation->setEndValue(1.0);
    
    QParallelAnimationGroup* spawnGroup = new QParallelAnimationGroup(this);
    spawnGroup->addAnimation(scaleAnimation);
    spawnGroup->addAnimation(fadeAnimation);
    
    connect(spawnGroup, &QAbstractAnimation::finished, [this]() {
        // 生成动画完成后开始其他动画
        startFloatingAnimation();
        startGlowAnimation();
    });
    
    spawnGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

// Getter methods - getItemType is inline in header

// getValue and getRarity functions removed - variables don't exist

// isActive and isCollected functions removed - inline in header or variables don't exist

bool GameItem::isMagnetActive() const
{
    return magnetActive;
}

QPointF GameItem::getMagnetTarget() const
{
    return magnetTarget;
}

float GameItem::getMagnetSpeed() const
{
    return magnetSpeed;
}

QString GameItem::getItemSpritePath() const
{
    switch (itemType) {
    case ITEM_SPEED_BOOST:
        return "itemicon0";
    case ITEM_DAMAGE_BOOST:
        return "itemicon1";
    case ITEM_SHIELD:
        return "itemicon2";
    case ITEM_HEALTH:
        return "itemicon3";
    case ITEM_SCORE_BONUS:
        return "itemicon4";
    case ITEM_TIME_SLOW:
        return "itemicon5";
    case ITEM_INVINCIBLE:
        return "itemicon6";
    case ITEM_DOUBLE_SCORE:
        return "itemicon7";
    case ITEM_EXTRA_LIFE:
        return "itemicon8";
    case ITEM_FREEZE_ENEMIES:
        return "itemicon9";
    case ITEM_MAGNET:
        return "itemicon10";
    case ITEM_BOMB:
        return "itemicon11";
    case ITEM_RAPID_FIRE:
        return "itemicon12";
    case ITEM_ENERGY_DRINK:
        return "itemicon13";
    case ITEM_PROTEIN_BAR:
        return "itemicon14";
    case ITEM_VITAMIN:
        return "itemicon15";
    case ITEM_MINERAL:
        return "itemicon16";
    case ITEM_FIBER:
        return "itemicon17";
    case ITEM_ANTIOXIDANT:
        return "itemicon18";
    case ITEM_OMEGA3:
        return "itemicon19";
    case ITEM_CALCIUM:
        return "itemicon20";
    case ITEM_IRON:
        return "itemicon21";
    case ITEM_ZINC:
        return "itemicon22";
    case ITEM_MULTIVITAMIN:
        return "itemicon23";
    default:
        return "itemicon0";
    }
}

void GameItem::onLifetimeExpired()
{
    // 道具生命周期结束，销毁道具
    destroy();
}

void GameItem::onCollectAnimationFinished()
{
    // 收集动画完成后，从场景中移除道具
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

void GameItem::onFloatingUpdate()
{
    // 浮动动画更新
    // 这个函数可以用于自定义浮动效果
}

void GameItem::onGlowUpdate()
{
    // 发光效果更新
    updateItemImage();
}

void GameItem::onMagnetUpdate()
{
    // 磁铁效果更新
    if (magnetActive && !magnetTarget.isNull()) {
        QPointF currentPos = pos();
        QPointF direction = magnetTarget - currentPos;
        qreal distance = QLineF(currentPos, magnetTarget).length();
        
        if (distance > 5.0) {
            // 计算移动向量
            direction = direction / distance * magnetSpeed;
            setPos(currentPos + direction);
        } else {
            // 到达目标位置，停止磁铁效果
            magnetActive = false;
            magnetTimer->stop();
        }
    }
}

QRectF GameItem::boundingRect() const
{
    return QRectF(0, 0, ITEM_SIZE, ITEM_SIZE);
}