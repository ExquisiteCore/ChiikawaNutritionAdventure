#ifndef SUGAR_OIL_PLAYER_H
#define SUGAR_OIL_PLAYER_H

#include "game_object_base.h"
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPropertyAnimation>

class SugarOilPlayer : public GameObjectBase
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit SugarOilPlayer(QObject *parent = nullptr);
    virtual ~SugarOilPlayer();

    // 基础属性获取
    int getHP() const { return mHP; }
    int getMaxHP() const { return mMaxHP; }
    qreal getSpeed() const { return mSpeed; }
    int getAttackPoint() const { return mAttackPoint; }
    int getDefence() const { return mDefence; }
    int getLevel() const { return mLevel; }
    int getExp() const { return mExp; }
    int getScore() const { return mScore; }
    
    // 移动相关
    void setFaceDirection(bool right) { mFaceRight = right; updatePixmap(); }
    bool getFaceDirection() const { return mFaceRight; }
    
    // 游戏逻辑
    void takeDamage(int damage);
    void heal(int amount);
    void gainExp(int exp);
    void gainScore(int score) { mScore += score; }
    
    // 射击
    void shoot(const QPointF &direction);
    
    // 状态效果
    void setInvincible(bool invincible);
    bool isInvincible() const { return mInvincible; }
    
    // 重置玩家状态
    void resetPlayer();
    
    // 更新动画帧
    void updateAnimation();
    
    // 移动控制
    void startMoving() { mIsMoving = true; }
    void stopMoving() { mIsMoving = false; }
    bool isMoving() const { return mIsMoving; }

signals:
    void healthChanged(int health, int maxHealth);
    void levelUp(int newLevel);
    void expChanged(int exp, int expToNext);
    void scoreChanged(int score);
    void playerDied();
    void playerShoot(QPointF position, QPointF direction, int damage);

public slots:
    void onInvincibilityTimeout();
    void onAnimationTimeout();

protected:
    void updatePixmap();
    void checkLevelUp();
    
private:
    // 基础属性
    int mHP;
    int mMaxHP;
    qreal mSpeed;
    int mAttackPoint;
    int mDefence;
    int mLevel;
    int mExp;
    int mScore;
    
    // 状态
    bool mFaceRight;
    bool mInvincible;
    bool mIsMoving;
    int mAnimationFrame;
    
    // 定时器
    QTimer* mInvincibilityTimer;
    QTimer* mAnimationTimer;
    
    // 音效
    QMediaPlayer* mShootSoundPlayer;
    QMediaPlayer* mHurtSoundPlayer;
    QAudioOutput* mShootSoundAudioOutput;
    QAudioOutput* mHurtSoundAudioOutput;
    
    // 动画相关
    QPropertyAnimation* mBlinkAnimation;
    
    // 常量
    static const int INVINCIBILITY_DURATION = 1000; // 1秒无敌时间
    static const int ANIMATION_INTERVAL = 200; // 动画帧间隔
    static const int EXP_PER_LEVEL = 100; // 每级所需经验
};

#endif // SUGAR_OIL_PLAYER_H