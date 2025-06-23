#ifndef ENEMY_BASE_H
#define ENEMY_BASE_H

#include "game_object_base.h"
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRandomGenerator>

class SugarOilPlayer;

class EnemyBase : public GameObjectBase
{
    Q_OBJECT

public:
    enum class EnemyType {
        FriedChicken = 0,    // 炸鸡
        Barbecue = 1,        // 烧烤
        MilkTea = 2,         // 奶茶
        SpiralShellNoodles = 3, // 螺蛳粉
        SmallCake = 4        // 小蛋糕
    };

    explicit EnemyBase(QObject *parent = nullptr);
    EnemyBase(SugarOilPlayer* player, int hp, int attackPoint, qreal speed, int expValue, EnemyType type, QObject *parent = nullptr);
    virtual ~EnemyBase();

    // 基础属性
    int getHP() const { return mHP; }
    int getMaxHP() const { return mMaxHP; }
    int getAttackPoint() const { return mAttackPoint; }
    qreal getSpeed() const { return mSpeed; }
    int getExpValue() const { return mExpValue; }
    EnemyType getEnemyType() const { return mEnemyType; }
    
    // 移动方向
    void setMoveDirection(bool right) { mMoveRight = right; }
    bool getMoveDirection() const { return mMoveRight; }
    
    // 面朝方向
    void setFaceDirection(bool right) { mFaceRight = right; updatePixmap(); }
    bool getFaceDirection() const { return mFaceRight; }
    
    // 战斗相关
    virtual void takeDamage(int damage);
    virtual void attack();
    
    // 移动
    virtual void moveEnemy();
    
    // AI行为
    virtual void updateAI();
    virtual void startSkill();
    
    // 获取玩家引用
    SugarOilPlayer* getPlayer() const { return mPlayer; }
    
    // 启动/停止AI
    void startAI();
    void stopAI();
    
signals:
    void enemyDied(EnemyBase* enemy);
    void enemyAttack(QPointF position, QPointF direction, int damage);
    void enemyHurt(EnemyBase* enemy);

public slots:
    void onAITimeout();
    void onSkillTimeout();

protected:
    virtual void updatePixmap();
    virtual void playHurtSound();
    virtual void playDeathSound();
    
    // 寻找玩家方向
    QPointF getDirectionToPlayer() const;
    qreal getDistanceToPlayer() const;
    
private:
    // 基础属性
    int mHP;
    int mMaxHP;
    int mAttackPoint;
    qreal mSpeed;
    int mExpValue;
    EnemyType mEnemyType;
    
    // 状态
    bool mMoveRight;
    bool mFaceRight;
    
    // 玩家引用
    SugarOilPlayer* mPlayer;
    
    // 定时器
    QTimer* mAITimer;
    QTimer* mSkillTimer;
    
    // 音效播放器
    QMediaPlayer* mHurtSoundPlayer;
    QMediaPlayer* mDeathSoundPlayer;
    QAudioOutput* mHurtSoundAudioOutput;
    QAudioOutput* mDeathSoundAudioOutput;
    
    // AI相关
    int mAICounter;
    
    static const int AI_UPDATE_INTERVAL = 100; // AI更新间隔
    static const int SKILL_COOLDOWN = 3000; // 技能冷却时间
};

#endif // ENEMY_BASE_H