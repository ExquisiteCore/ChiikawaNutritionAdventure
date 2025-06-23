#ifndef SUGAR_OIL_ENEMY_H
#define SUGAR_OIL_ENEMY_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include "sugar_oil_config.h"

class SugarOilEnemy : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit SugarOilEnemy(SugarOilEnemyType enemyType, QObject *parent = nullptr);
    ~SugarOilEnemy();
    
    // 基本属性
    SugarOilEnemyType getEnemyType() const { return enemyType; }
    int getHealth() const;
    int getMaxHealth() const;
    int getDamage() const { return damage; }
    float getSpeed() const { return speed; }
    SugarOilDirection getDirection() const;
    bool isActive() const { return active; }
    bool isAttacking() const;
    QPointF getTargetPosition() const;
    
    // 移动控制
    void setTarget(const QPointF& targetPosition);
    void updateMovement();
    void updateDirection(const QPointF& directionVector);
    void setSpeed(float newSpeed);
    void freeze(int duration);
    void unfreeze();
    bool isMoving() const;
    
    // 位置管理
    void setPosition(const QPointF& position);
    QPointF getCenter() const;
    void moveTowardsTarget();
    void moveRandomly();
    
    // 状态管理
    void setActive(bool state) { active = state; }
    bool isFrozen() const { return frozen; }
    void destroy();
    
    // 碰撞检测
    enum { Type = TYPE_SUGAR_OIL_ENEMY };
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    
    // 动画控制
    void playAttackAnimation();
    void playDeathAnimation();
    void startMovementAnimation();
    void pauseAnimation();
    void resumeAnimation();
    void updateAnimation();
    void playDestroyAnimation();
    void playFreezeAnimation();
    void playDamageAnimation();
    void loadEnemyImages();
    void initializeEnemy();
    
    // 战斗相关
    void takeDamage(int damage);
    
    // 辅助函数
    QColor getEnemyColor() const;
    QPixmap createDefaultPixmap(int width, int height, const QColor& color);
    
    // 特殊行为
    void activateSpecialAbility();
    void setAggressive(bool aggressive);
    void attack();
    
signals:
    void enemyDestroyed(SugarOilEnemy* enemy);
    void playerHit(int damage);
    void enemyDamaged(int damage);
    void enemyFrozen();
    void enemyUnfrozen();
    void enemyAttacked();
    
private slots:
    void onMovementUpdate();
    void onAnimationUpdate();
    void onFreezeTimeout();
    void onSpecialAbilityTimeout();
    void onDestroyAnimationFinished();
    
private:
    void loadSprites();
    void updateSprite();
    void setupTimers();
    void setupAnimations();
    void calculateMovement();
    void applyFreezeEffect();
    void removeFreezeEffect();
    void updateEnemyImage();
    QString getEnemyName() const;
    QPixmap getEnemySprite() const;
    
    // 基本属性
    SugarOilEnemyType enemyType;
    int health;
    int maxHealth;
    int damage;
    float speed;
    float baseSpeed;
    bool active;
    bool frozen;
    bool aggressive;
    bool attacking;
    
    // 移动相关
    QPointF targetPosition;
    QPointF lastPosition;
    QPointF velocity;
    float movementAngle;
    bool hasTarget;
    SugarOilDirection direction;
    
    // 定时器
    QTimer* movementTimer;
    QTimer* animationTimer;
    QTimer* freezeTimer;
    QTimer* specialAbilityTimer;
    QTimer* attackTimer;
    
    // 动画相关
    int animationFrame;
    int currentAnimationFrame;
    QList<QPixmap> sprites;
    QList<QPixmap> frozenSprites;
    QMap<QString, QPixmap> enemyImages;
    
    // 视觉效果
    QPropertyAnimation* destroyAnimation;
    QPropertyAnimation* freezeAnimation;
    
    // 特殊能力
    bool specialAbilityActive;
    int specialAbilityCooldown;
    
    // 敌人类型配置
    struct EnemyConfig {
        QString name;
        QString spritePath;
        int damage;
        float speed;
        int specialAbilityCooldown;
    };
    
    static const EnemyConfig ENEMY_CONFIGS[5];
    static const int ANIMATION_FRAMES = 4;
    static const int MOVEMENT_UPDATE_INTERVAL = 25; // ~40 FPS，与其他组件保持一致
    static const int ANIMATION_UPDATE_INTERVAL = 250; // 4 FPS，稍微降低动画频率
};

#endif // SUGAR_OIL_ENEMY_H