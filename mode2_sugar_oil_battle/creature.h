#ifndef CREATURE_H
#define CREATURE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QRandomGenerator>
#include "sugar_oil_config.h"

class Creature : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit Creature(CreatureType creatureType, QObject *parent = nullptr);
    ~Creature();
    
    // 基本属性
    CreatureType getCreatureType() const { return creatureType; }
    QString getCreatureName() const;
    QString getCreatureDescription() const;
    QString getCreatureAbility() const;
    bool isActive() const { return active; }
    bool isFriendly() const { return friendly; }
    
    // 状态管理
    void setActive(bool state) { active = state; }
    void encounter();
    void activate();
    void deactivate();
    
    // 位置管理
    void setPosition(const QPointF& position);
    QPointF getCenter() const;
    void moveRandomly();
    void moveTowardsPlayer(const QPointF& playerPosition);
    
    // 碰撞检测
    enum { Type = TYPE_CREATURE };
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    
    // 动画控制
    void startIdleAnimation();
    void startActiveAnimation();
    void startEncounterAnimation();
    void stopAllAnimations();
    void playAppearAnimation();
    void playDisappearAnimation();
    
    // 特殊能力
    void activateAbility();
    bool isAbilityActive() const { return abilityActive; }
    int getAbilityDuration() const { return abilityDuration; }
    
signals:
    void creatureEncountered(Creature* creature);
    void creatureActivated(Creature* creature);
    void creatureDeactivated(Creature* creature);
    void abilityActivated(CreatureType type, int duration);
    void abilityDeactivated(CreatureType type);
    
private slots:
    void onMovementUpdate();
    void onAnimationUpdate();
    void onAbilityTimeout();
    void onLifetimeTimeout();
    void onEncounterAnimationFinished();
    void onAppearAnimationFinished();
    void onDisappearAnimationFinished();
    
private:
    void initializeCreature();
    void loadSprites();
    void updateSprite();
    void setupTimers();
    void setupAnimations();
    void configureCreatureProperties();
    void applyVisualEffects();
    void removeVisualEffects();
    QString getCreatureSpritePath() const;
    void calculateRandomMovement();
    void playAbilityAnimation();
    
    // 基本属性
    CreatureType creatureType;
    bool active;
    bool friendly;
    bool encountered;
    
    // 移动相关
    QPointF velocity;
    float speed;
    float movementAngle;
    bool isMoving;
    QPointF targetPosition;
    QPointF movementDirection;
    
    // 能力相关
    bool abilityActive;
    int abilityDuration;
    int abilityCooldown;
    
    // 定时器
    QTimer* movementTimer;
    QTimer* animationTimer;
    QTimer* abilityTimer;
    QTimer* lifetimeTimer;
    
    // 动画相关
    int animationFrame;
    int animationState; // 0: idle, 1: active, 2: encounter
    QList<QPixmap> idleSprites;
    QList<QPixmap> activeSprites;
    QList<QPixmap> encounterSprites;
    
    // 视觉效果
    QPropertyAnimation* appearAnimation;
    QPropertyAnimation* disappearAnimation;
    QPropertyAnimation* encounterAnimation;
    QPropertyAnimation* pulseAnimation;
    QGraphicsEffect* currentEffect;
    
    // 生命周期
    int lifetime; // 毫秒
    
    // 生物配置
    struct CreatureConfig {
        QString name;
        QString description;
        QString ability;
        QString spritePath;
        bool friendly;
        float speed;
        int abilityDuration;
        int abilityCooldown;
        int lifetime;
        bool hasGlow;
        bool hasPulse;
    };
    
    static const CreatureConfig CREATURE_CONFIGS[5];
    static constexpr int ANIMATION_FRAMES = 3;
    static constexpr int DEFAULT_LIFETIME = 20000; // 20秒
    static constexpr float DEFAULT_SPEED = 2.0f;
    static constexpr int MOVEMENT_UPDATE_INTERVAL = 25; // ~40 FPS，与其他组件保持一致
    static constexpr int ANIMATION_UPDATE_INTERVAL = 350; // ~2.9 FPS，稍微降低动画频率
};

#endif // CREATURE_H