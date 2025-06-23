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
    , mEnemyType(EnemyType::FriedChicken)
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
    mHurtSoundAudioOutput = new QAudioOutput(this);
    mHurtSoundPlayer->setAudioOutput(mHurtSoundAudioOutput);
    mHurtSoundPlayer->setSource(QUrl("qrc:/sounds/enemy_hurt.wav"));

    mDeathSoundPlayer = new QMediaPlayer(this);
    mDeathSoundAudioOutput = new QAudioOutput(this);
    mDeathSoundPlayer->setAudioOutput(mDeathSoundAudioOutput);
    mDeathSoundPlayer->setSource(QUrl("qrc:/sounds/enemy_death.wav"));
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
    mHurtSoundAudioOutput = new QAudioOutput(this);
    mHurtSoundPlayer->setAudioOutput(mHurtSoundAudioOutput);
    mHurtSoundPlayer->setSource(QUrl("qrc:/sounds/enemy_hurt.wav"));

    mDeathSoundPlayer = new QMediaPlayer(this);
    mDeathSoundAudioOutput = new QAudioOutput(this);
    mDeathSoundPlayer->setAudioOutput(mDeathSoundAudioOutput);
    mDeathSoundPlayer->setSource(QUrl("qrc:/sounds/enemy_death.wav"));
    
    updatePixmap();
    setScale(0.12);
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
    
    // 根据糖油混合物敌人类型执行不同的AI行为
    switch (mEnemyType) {
    case EnemyType::FriedChicken:
        // 炸鸡：每2秒攻击一次
        if (mAICounter % 20 == 0) {
            attack();
        }
        break;
        
    case EnemyType::Barbecue:
        // 烧烤：每1.5秒攻击一次
        if (mAICounter % 15 == 0) {
            attack();
        }
        break;
        
    case EnemyType::MilkTea:
        // 奶茶：每1秒攻击一次，偶尔使用技能
        if (mAICounter % 10 == 0) {
            attack();
        }
        if (mAICounter % 50 == 0) {
            startSkill();
        }
        break;
        
    case EnemyType::SpiralShellNoodles:
        // 螺蛳粉：频繁攻击和技能
        if (mAICounter % 8 == 0) {
            attack();
        }
        if (mAICounter % 30 == 0) {
            startSkill();
        }
        break;
        
    case EnemyType::SmallCake:
        // 小蛋糕：快速攻击
        if (mAICounter % 12 == 0) {
            attack();
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
    
    // 根据糖油混合物敌人类型选择图像
    switch (mEnemyType) {
    case EnemyType::FriedChicken:
        imagePath = mFaceRight ? ":/img/roles/chimera1.png" : ":/img/roles/chimera1-mir.png";
        break;
    case EnemyType::Barbecue:
        imagePath = mFaceRight ? ":/img/roles/chimera2.png" : ":/img/roles/chimera2-mir.png";
        break;
    case EnemyType::MilkTea:
        imagePath = mFaceRight ? ":/img/roles/chimera3.png" : ":/img/roles/chimera3-mir.png";
        break;
    case EnemyType::SpiralShellNoodles:
        imagePath = mFaceRight ? ":/img/roles/chimera4.png" : ":/img/roles/chimera4-mir.png";
        break;
    case EnemyType::SmallCake:
        imagePath = mFaceRight ? ":/img/roles/chimera5.png" : ":/img/roles/chimera5-mir.png";
        break;
    }
    
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    } else {
        // 如果找不到图像，创建一个简单的彩色矩形
        QPixmap defaultPixmap(40, 40);
        switch (mEnemyType) {
        case EnemyType::FriedChicken:
            defaultPixmap.fill(QColor(255, 165, 0)); // 橙色 - 炸鸡
            break;
        case EnemyType::Barbecue:
            defaultPixmap.fill(QColor(139, 69, 19)); // 棕色 - 烧烤
            break;
        case EnemyType::MilkTea:
            defaultPixmap.fill(QColor(210, 180, 140)); // 奶茶色
            break;
        case EnemyType::SpiralShellNoodles:
            defaultPixmap.fill(QColor(255, 0, 0)); // 红色 - 螺蛳粉
            break;
        case EnemyType::SmallCake:
            defaultPixmap.fill(QColor(255, 192, 203)); // 粉色 - 小蛋糕
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