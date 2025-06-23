#ifndef USAGI_PLAYER_H
#define USAGI_PLAYER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QMap>
#include "sugar_oil_config.h"

class UsagiPlayer : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit UsagiPlayer(QObject *parent = nullptr);
    ~UsagiPlayer();
    
    // 移动控制
    void setDirection(SugarOilDirection dir);
    void move();
    void startMovement();
    void stopMovement();
    void respawn();
    void updatePosition();
    
    // 生命值管理
    int getLives() const { return lives; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void takeDamage(int damage = 1);
    void heal(int amount = 1);
    void addLife();
    bool isAlive() const { return lives > 0; }
    
    // 分数管理
    int getScore() const { return score; }
    void addScore(int points);
    void setScoreMultiplier(float multiplier) { scoreMultiplier = multiplier; }
    
    // 状态管理
    bool isInvincible() const { return invincible; }
    void setInvincible(bool state, int duration = 0);
    bool hasShield() const { return shieldActive; }
    void activateShield(int duration);
    void deactivateShield();
    bool hasMagnet() const { return magnetActive; }
    
    // 速度管理
    void setSpeedMultiplier(float multiplier);
    float getCurrentSpeed() const { return baseSpeed * speedMultiplier; }
    float getSpeedMultiplier() const { return speedMultiplier; }
    float getScoreMultiplier() const { return scoreMultiplier; }
    
    // 特殊能力
    void activateMagnet(int duration);
    void deactivateMagnet();
    void freezeEnemies(int duration);
    void activateRapidFire(int duration);
    
    // 位置管理
    void setPosition(const QPointF& position);
    QPointF getCenter() const;
    void constrainToScene(const QRectF& sceneRect);
    
    // 碰撞检测
    enum { Type = TYPE_USAGI };
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    
    // 动画控制
    void updateAnimation();
    void startIdleAnimation();
    void startWalkAnimation();
    void pauseAnimation();
    void resumeAnimation();
    void playDamageAnimation();
    void playHealAnimation();
    void playInvincibleAnimation();
    
    // 重置
    void reset();
    
signals:
    void livesChanged(int newLives);
    void scoreChanged(int newScore);
    void playerDamaged();
    void playerHealed();
    void playerDied();
    void invincibilityChanged(bool invincible);
    void shieldChanged(bool active);
    void shieldActivated();
    void shieldDeactivated();
    void magnetDeactivated();
    void speedBoostActivated();
    void speedBoostDeactivated();
    void scoreBoostActivated();
    void scoreBoostDeactivated();
    void healthChanged(int newHealth);
    void magnetActivated();
    
private slots:
    void onInvincibilityTimeout();
    void onShieldTimeout();
    void onSpeedBoostTimeout();
    void onMagnetTimeout();
    void onRapidFireTimeout();
    void updateMovement();
    void updateEffects();
    void startBlinking();
    void stopBlinking();
    void updateBlinking();
    void loadPlayerImages();
    QPixmap createDefaultPixmap(int width, int height, const QColor& color);
    
private:
    void loadSprites();
    void updateSprite();
    void setupTimers();
    void setupAnimations();
    void applyVisualEffects();
    void removeVisualEffects();
    
    // 基本属性
    int lives;
    int score;
    int health;
    int maxHealth;
    float scoreMultiplier;
    float baseSpeed;
    float speedMultiplier;
    
    // 移动相关
    SugarOilDirection currentDirection;
    QPointF velocity;
    bool isMoving;
    
    // 状态效果
    bool invincible;
    bool shieldActive;
    bool magnetActive;
    bool rapidFireActive;
    bool isBlinking;
    
    // 定时器
    QTimer* movementTimer;
    QTimer* animationTimer;
    QTimer* invincibilityTimer;
    QTimer* shieldTimer;
    QTimer* speedBoostTimer;
    QTimer* scoreBoostTimer;
    QTimer* magnetTimer;
    QTimer* rapidFireTimer;
    QTimer* blinkTimer;
    QTimer* effectsTimer;
    
    // 动画相关
    int animationFrame;
    int animationDirection;
    int currentAnimationFrame;
    QList<QPixmap> sprites;
    QList<QPixmap> invincibleSprites;
    QMap<QString, QPixmap> playerImages;
    
    // 视觉效果
    QPropertyAnimation* damageAnimation;
    QPropertyAnimation* healAnimation;
    QPropertyAnimation* invincibleAnimation;
    QGraphicsEffect* currentEffect;
    
    // 常量
    static const int MAX_LIVES = 5;
    static const int ANIMATION_FRAMES = 3;
    static const int INVINCIBILITY_DURATION = 2000; // 2秒
};

#endif // USAGI_PLAYER_H