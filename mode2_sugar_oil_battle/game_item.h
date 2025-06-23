#ifndef GAME_ITEM_H
#define GAME_ITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QMap>
#include "sugar_oil_config.h"

class GameItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
    explicit GameItem(ItemType itemType, QObject *parent = nullptr);
    ~GameItem();
    
    // 基本属性
    ItemType getItemType() const { return itemType; }
    QString getItemName() const;
    QString getItemDescription() const;
    int getScoreValue() const { return scoreValue; }
    bool isActive() const { return active; }
    
    // 状态管理
    void setActive(bool state) { active = state; }
    void collect();
    void destroy();
    
    // 位置管理
    void setPosition(const QPointF& position);
    QPointF getCenter() const;
    
    // 碰撞检测
    enum { Type = TYPE_GAME_ITEM };
    int type() const override { return Type; }
    QRectF boundingRect() const override;
    
    // 动画控制
    void startFloatingAnimation();
    void startGlowAnimation();
    void startRotationAnimation();
    void stopAllAnimations();
    void playCollectAnimation();
    void playSpawnAnimation();
    void playDestroyAnimation();
    
    // 磁铁效果
    void setMagnetTarget(const QPointF& targetPosition);
    void updateMagnetMovement();
    void setMagnetActive(bool active);
    bool isMagnetActive() const;
    QPointF getMagnetTarget() const;
    float getMagnetSpeed() const;
    
signals:
    void itemCollected(GameItem* item);
    void itemDestroyed(GameItem* item);
    
private slots:
    void onFloatingUpdate();
    void onGlowUpdate();
    void onMagnetUpdate();
    void onCollectAnimationFinished();
    void onLifetimeExpired();
    
private:
    void loadSprite();
    void loadItemImages();
    void updateItemImage();
    void setupAnimations();
    void initializeItem();
    void setupTimers();
    void applyVisualEffects();
    void removeVisualEffects();
    QString getItemSpritePath() const;
    void configureItemProperties();
    
    // 基本属性
    ItemType itemType;
    int scoreValue;
    bool active;
    
    // 动画相关
    QTimer* floatingTimer;
    QTimer* glowTimer;
    QTimer* magnetTimer;
    QTimer* lifetimeTimer;
    
    QPropertyAnimation* floatingAnimation;
    QPropertyAnimation* glowAnimation;
    QPropertyAnimation* rotationAnimation;
    QPropertyAnimation* collectAnimation;
    QPropertyAnimation* magnetAnimation;
    
    // 浮动效果
    float floatingOffset;
    float floatingDirection;
    QPointF originalPosition;
    
    // 发光效果
    float glowIntensity;
    bool glowIncreasing;
    QGraphicsEffect* glowEffect;
    
    // 磁铁效果
    bool magnetActive;
    QPointF magnetTarget;
    float magnetSpeed;
    
    // 生命周期
    int lifetime; // 毫秒
    
    // 图像资源
    QMap<QString, QPixmap> itemImages;
    
    // 道具配置
    struct ItemConfig {
        QString name;
        QString description;
        QString spritePath;
        int scoreValue;
        int lifetime;
        bool hasGlow;
        bool hasFloating;
        bool hasRotation;
    };
    
    static const ItemConfig ITEM_CONFIGS[24];
    static constexpr int DEFAULT_LIFETIME = 15000; // 15秒
    static constexpr float FLOATING_AMPLITUDE = 5.0f;
    static constexpr float FLOATING_SPEED = 2.0f;
    static constexpr float MAGNET_SPEED = 8.0f;
};

#endif // GAME_ITEM_H