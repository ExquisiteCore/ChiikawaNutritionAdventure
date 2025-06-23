#include "enemy_base.h"
#include "sugar_oil_player.h"
#include <QPixmap>
#include <QtMath>
#include <QUrl>

EnemyBase::EnemyBase(QObject *parent)
    : GameObjectBase(parent)
    , mHP(50)
    , mMaxHP(50)
    , mAttackPoint(10)
    , mSpeed(1.0)
    , mExpValue(10)
    , mEnemyType(SmallEnemy)
    , mMoveRight(true)
    , mFaceRight(true)
    , mPlayer(nullptr)
    , mAITimer(nullptr)
    , mSkillTimer(nullptr)
    , mHurtSoundPlayer(nullptr)
    , mDeathSoundPlayer(nullptr)

    , mAICounter(0)
{
    // 初始化AI定时器
    mAITimer = new QTimer(this);
    mAITimer->setInterval(AI_UPDATE_INTERVAL);
    connect(mAITimer, &QTimer::timeout, this, &EnemyBase::onAITimeout);
    
    // 初始化技能定时器
    mSkillTimer = new QTimer(this);
    mSkillTimer->setSingleShot(true);
    connect(mSkillTimer, &QTimer::timeout, this, &EnemyBase::onSkillTimeout);
    
    // 初始化音效 (Qt5兼容)
    mHurtSoundPlayer = new QMediaPlayer(this);
    mHurtSoundPlayer->setMedia(QUrl("qrc:/sounds/enemy_hurt.wav"));
    
    mDeathSoundPlayer = new QMediaPlayer(this);
    mDeathSoundPlayer->setMedia(QUrl("qrc:/sounds/enemy_death.wav"));
}

EnemyBase::EnemyBase(SugarOilPlayer* player, int hp, int attackPoint, qreal speed, int expValue, EnemyType type, QObject *parent)
    : GameObjectBase(parent)
    , mHP(hp)
    , mMaxHP(hp)
    , mAttackPoint(attackPoint)
    , mSpeed(speed)
    , mExpValue(expValue)
    , mEnemyType(type)
    , mMoveRight(true)
    , mFaceRight(true)
    , mPlayer(player)
    , mAITimer(nullptr)
    , mSkillTimer(nullptr)
    , mHurtSoundPlayer(nullptr)
    , mDeathSoundPlayer(nullptr)

    , mAICounter(0)
{
    // 初始化AI定时器
    mAITimer = new QTimer(this);
    mAITimer->setInterval(AI_UPDATE_INTERVAL);
    connect(mAITimer, &QTimer::timeout, this, &EnemyBase::onAITimeout);
    
    // 初始化技能定时器
    mSkillTimer = new QTimer(this);
    mSkillTimer->setSingleShot(true);
    connect(mSkillTimer, &QTimer::timeout, this, &EnemyBase::onSkillTimeout);
    
    // 初始化音效 (Qt5兼容)
    mHurtSoundPlayer = new QMediaPlayer(this);
    mHurtSoundPlayer->setMedia(QUrl("qrc:/sounds/enemy_hurt.wav"));
    
    mDeathSoundPlayer = new QMediaPlayer(this);
    mDeathSoundPlayer->setMedia(QUrl("qrc:/sounds/enemy_death.wav"));
    
    updatePixmap();
    setScale(0.6);
    setZValue(3);
}

EnemyBase::~EnemyBase()
{
    if (mAITimer) {
        mAITimer->stop();
    }
    if (mSkillTimer) {
        mSkillTimer->stop();
    }
}



void EnemyBase::takeDamage(int damage)
{
    mHP = qMax(0, mHP - damage);
    
    // 播放受伤音效
    playHurtSound();
    
    emit enemyHurt(this);
    
    if (mHP <= 0) {
        // 播放死亡音效
        playDeathSound();
        
        // 停止AI
        stopAI();
        
        emit enemyDied(this);
    }
}

void EnemyBase::attack()
{
    if (!mPlayer) {
        return;
    }
    
    // 计算攻击方向（朝向玩家）
    QPointF direction = getDirectionToPlayer();
    
    // 发射攻击信号
    emit enemyAttack(getCenterPos(), direction, mAttackPoint);
}

void EnemyBase::moveEnemy()
{
    if (!mPlayer) {
        return;
    }
    
    // 简单的追踪AI：朝玩家方向移动
    QPointF direction = getDirectionToPlayer();
    
    qreal dx = direction.x() * mSpeed;
    qreal dy = direction.y() * mSpeed;
    
    moveBy(dx, dy);
    
    // 更新面朝方向
    if (dx > 0) {
        setFaceDirection(true);
    } else if (dx < 0) {
        setFaceDirection(false);
    }
}

void EnemyBase::updateAI()
{
    mAICounter++;
    
    // 移动
    moveEnemy();
    
    // 根据敌人类型执行不同的AI行为
    switch (mEnemyType) {
    case SmallEnemy:
        // 小敌人：每2秒攻击一次
        if (mAICounter % 20 == 0) {
            attack();
        }
        break;
        
    case MediumEnemy:
        // 中等敌人：每1.5秒攻击一次
        if (mAICounter % 15 == 0) {
            attack();
        }
        break;
        
    case LargeEnemy:
        // 大敌人：每1秒攻击一次，偶尔使用技能
        if (mAICounter % 10 == 0) {
            attack();
        }
        if (mAICounter % 50 == 0) {
            startSkill();
        }
        break;
        
    case BossEnemy:
        // Boss：频繁攻击和技能
        if (mAICounter % 8 == 0) {
            attack();
        }
        if (mAICounter % 30 == 0) {
            startSkill();
        }
        break;
    }
}

void EnemyBase::startSkill()
{
    // 基础技能：连续攻击
    for (int i = 0; i < 3; i++) {
        QTimer::singleShot(i * 200, this, [this]() {
            attack();
        });
    }
    
    // 设置技能冷却
    mSkillTimer->start(SKILL_COOLDOWN);
}

void EnemyBase::startAI()
{
    if (mAITimer && !mAITimer->isActive()) {
        mAITimer->start();
    }
}

void EnemyBase::stopAI()
{
    if (mAITimer) {
        mAITimer->stop();
    }
    if (mSkillTimer) {
        mSkillTimer->stop();
    }
}

void EnemyBase::onAITimeout()
{
    updateAI();
}

void EnemyBase::onSkillTimeout()
{
    // 技能冷却结束，可以再次使用技能
}

void EnemyBase::updatePixmap()
{
    QString imagePath;
    
    // 根据敌人类型和面朝方向选择图像
    switch (mEnemyType) {
    case SmallEnemy:
        imagePath = mFaceRight ? ":/img/enemies/small_enemy.png" : ":/img/enemies/small_enemy_left.png";
        break;
    case MediumEnemy:
        imagePath = mFaceRight ? ":/img/enemies/medium_enemy.png" : ":/img/enemies/medium_enemy_left.png";
        break;
    case LargeEnemy:
        imagePath = mFaceRight ? ":/img/enemies/large_enemy.png" : ":/img/enemies/large_enemy_left.png";
        break;
    case BossEnemy:
        imagePath = mFaceRight ? ":/img/enemies/boss_enemy.png" : ":/img/enemies/boss_enemy_left.png";
        break;
    }
    
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    } else {
        // 如果找不到图像，创建一个简单的彩色矩形
        QPixmap defaultPixmap(40, 40);
        switch (mEnemyType) {
        case SmallEnemy:
            defaultPixmap.fill(Qt::green);
            break;
        case MediumEnemy:
            defaultPixmap.fill(Qt::yellow);
            break;
        case LargeEnemy:
            defaultPixmap.fill(Qt::magenta);
            break;
        case BossEnemy:
            defaultPixmap.fill(Qt::darkRed);
            break;
        }
        setPixmap(defaultPixmap);
    }
}

void EnemyBase::playHurtSound()
{
    if (mHurtSoundPlayer) {
        mHurtSoundPlayer->setPosition(0);
        mHurtSoundPlayer->play();
    }
}

void EnemyBase::playDeathSound()
{
    if (mDeathSoundPlayer) {
        mDeathSoundPlayer->setPosition(0);
        mDeathSoundPlayer->play();
    }
}

QPointF EnemyBase::getDirectionToPlayer() const
{
    if (!mPlayer) {
        return QPointF(1.0, 0.0);
    }
    
    QPointF playerPos = mPlayer->getCenterPos();
    QPointF myPos = getCenterPos();
    
    QPointF direction = playerPos - myPos;
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (length > 0) {
        return QPointF(direction.x() / length, direction.y() / length);
    }
    
    return QPointF(1.0, 0.0);
}

qreal EnemyBase::getDistanceToPlayer() const
{
    if (!mPlayer) {
        return 0.0;
    }
    
    QPointF playerPos = mPlayer->getCenterPos();
    QPointF myPos = getCenterPos();
    
    QPointF diff = playerPos - myPos;
    return qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
}