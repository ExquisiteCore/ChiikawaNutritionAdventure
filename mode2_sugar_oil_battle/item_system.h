#ifndef ITEM_SYSTEM_H
#define ITEM_SYSTEM_H

#include "sugar_oil_config.h"
#include "game_object_base.h"
#include <QTimer>
#include "../audio_manager.h"
#include <QRandomGenerator>

class SugarOilPlayer;

// 道具效果结构体
struct ItemEffect {
    int duration;           // 持续时间（毫秒），0表示瞬间效果
    float speedMultiplier;  // 速度倍数
    float attackMultiplier; // 攻击力倍数
    float defenseMultiplier;// 防御力倍数
    int healthRestore;      // 恢复生命值
    int scoreBonus;         // 分数奖励
    bool invincible;        // 无敌状态
    bool rapidFire;         // 快速射击
    bool magnetPower;       // 磁力吸引
    
    ItemEffect() : duration(0), speedMultiplier(1.0f), attackMultiplier(1.0f), 
                   defenseMultiplier(1.0f), healthRestore(0), scoreBonus(0),
                   invincible(false), rapidFire(false), magnetPower(false) {}
};

// 道具类
class GameItem : public GameObjectBase
{
    Q_OBJECT
    
public:
    explicit GameItem(ItemType type, QObject *parent = nullptr);
    virtual ~GameItem();
    
    ItemType getItemType() const { return mItemType; }
    ItemEffect getEffect() const { return mEffect; }
    
    // 道具收集效果
    void applyEffect(SugarOilPlayer* player);
    
    // 更新道具动画
    void updateAnimation();
    
protected:
    void updatePixmap();
    void setupEffect();
    
private slots:
    void onAnimationTimeout();
    
private:
    ItemType mItemType;
    ItemEffect mEffect;
    QTimer* mAnimationTimer;
    int mAnimationFrame;
    
    // 音效现在由AudioManager统一管理
    
    static const int ANIMATION_INTERVAL = 300; // 动画帧间隔
};

// 道具管理器
class ItemManager : public QObject
{
    Q_OBJECT
    
public:
    explicit ItemManager(QObject *parent = nullptr);
    virtual ~ItemManager();
    
    // 生成随机道具
    GameItem* spawnRandomItem(const QPointF& position);
    
    // 获取道具效果描述
    static QString getItemDescription(ItemType type);
    
    // 获取道具名称
    static QString getItemName(ItemType type);
    
private:
    QRandomGenerator* mRandomGenerator;
};

#endif // ITEM_SYSTEM_H