#ifndef CREATURE_SYSTEM_H
#define CREATURE_SYSTEM_H

#include "sugar_oil_config.h"
#include "game_object_base.h"
#include <QTimer>
// 音频现在由AudioManager统一管理
#include "../audio_manager.h"
#include <QRandomGenerator>

class SugarOilPlayer;

// 生物效果结构体
struct CreatureEffect {
    int duration;           // 持续时间（毫秒）
    float speedBonus;       // 速度加成
    float attackBonus;      // 攻击力加成
    float defenseBonus;     // 防御力加成
    float expBonus;         // 经验加成倍数
    int healthRegen;        // 生命恢复速度（每秒）
    bool isActive;          // 效果是否激活
    
    CreatureEffect() : duration(0), speedBonus(0), attackBonus(0), 
                       defenseBonus(0), expBonus(1.0f), healthRegen(0), isActive(false) {}
};

// 奇异生物类
class GameCreature : public GameObjectBase
{
    Q_OBJECT
    
public:
    explicit GameCreature(CreatureType type, QObject *parent = nullptr);
    virtual ~GameCreature();
    
    CreatureType getCreatureType() const { return mCreatureType; }
    CreatureEffect getEffect() const { return mEffect; }
    
    // 激活生物效果
    void activateEffect(SugarOilPlayer* player);
    
    // 更新生物动画和AI
    void updateCreature();
    
    // 生物移动
    void moveTowardsPlayer(const QPointF& playerPos);
    
    // 检查是否靠近玩家
    bool isNearPlayer(const QPointF& playerPos, qreal distance = 50.0) const;
    
protected:
    void updatePixmap();
    void setupEffect();
    
private slots:
    void onAnimationTimeout();
    void onMoveTimeout();
    
private:
    CreatureType mCreatureType;
    CreatureEffect mEffect;
    QTimer* mAnimationTimer;
    QTimer* mMoveTimer;
    int mAnimationFrame;
    qreal mSpeed;
    bool mIsFollowingPlayer;
    
    // 音效现在由AudioManager统一管理
    
    static const int ANIMATION_INTERVAL = 400; // 动画帧间隔
    static const int MOVE_INTERVAL = 30;       // 移动更新间隔，优化性能
};

// 生物管理器
class CreatureManager : public QObject
{
    Q_OBJECT
    
public:
    explicit CreatureManager(QObject *parent = nullptr);
    virtual ~CreatureManager();
    
    // 生成随机生物
    GameCreature* spawnRandomCreature(const QPointF& position);
    
    // 获取生物效果描述
    static QString getCreatureDescription(CreatureType type);
    
    // 获取生物名称
    static QString getCreatureName(CreatureType type);
    
private:
    QRandomGenerator* mRandomGenerator;
};

#endif // CREATURE_SYSTEM_H