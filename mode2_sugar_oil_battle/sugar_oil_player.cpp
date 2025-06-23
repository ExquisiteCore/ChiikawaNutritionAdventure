#include "sugar_oil_player.h"
#include <QPixmap>
#include <QUrl>
#include <QRandomGenerator>

SugarOilPlayer::SugarOilPlayer(QObject *parent)
    : GameObjectBase(parent)
    , mHP(100)
    , mMaxHP(100)
    , mSpeed(3.0)
    , mAttackPoint(20)
    , mDefence(5)
    , mLevel(1)
    , mExp(0)
    , mScore(0)
    , mFaceRight(true)
    , mInvincible(false)
    , mIsMoving(false)
    , mAnimationFrame(0)
    , mInvincibilityTimer(nullptr)
    , mAnimationTimer(nullptr)
    , mShootSoundPlayer(nullptr)
    , mHurtSoundPlayer(nullptr)

    , mBlinkAnimation(nullptr)
{
    // 设置初始图像
    setPixmap(QPixmap(":/img/roles/usagi1.png"));
    setScale(0.8);
    setZValue(10); // 确保玩家在最上层
    
    // 初始化定时器
    mInvincibilityTimer = new QTimer(this);
    mInvincibilityTimer->setSingleShot(true);
    connect(mInvincibilityTimer, &QTimer::timeout, this, &SugarOilPlayer::onInvincibilityTimeout);
    
    mAnimationTimer = new QTimer(this);
    mAnimationTimer->setInterval(ANIMATION_INTERVAL);
    connect(mAnimationTimer, &QTimer::timeout, this, &SugarOilPlayer::onAnimationTimeout);
    mAnimationTimer->start();
    
    // 初始化音效播放器 (Qt5兼容)
    mShootSoundPlayer = new QMediaPlayer(this);
    mShootSoundPlayer->setMedia(QUrl("qrc:/sounds/shoot.wav"));
    
    mHurtSoundPlayer = new QMediaPlayer(this);
    mHurtSoundPlayer->setMedia(QUrl("qrc:/sounds/hurt.wav"));
    
    // 初始化闪烁动画
    mBlinkAnimation = new QPropertyAnimation(this, "opacity", this);
    mBlinkAnimation->setDuration(100);
    mBlinkAnimation->setStartValue(1.0);
    mBlinkAnimation->setEndValue(0.3);
    mBlinkAnimation->setLoopCount(5);
}

SugarOilPlayer::~SugarOilPlayer()
{
    if (mInvincibilityTimer) {
        mInvincibilityTimer->stop();
    }
    if (mAnimationTimer) {
        mAnimationTimer->stop();
    }
    if (mBlinkAnimation) {
        mBlinkAnimation->stop();
    }
}

void SugarOilPlayer::takeDamage(int damage)
{
    if (mInvincible) {
        return;
    }
    
    int actualDamage = qMax(1, damage - mDefence);
    mHP = qMax(0, mHP - actualDamage);
    
    // 播放受伤音效
    if (mHurtSoundPlayer) {
        mHurtSoundPlayer->setPosition(0);
        mHurtSoundPlayer->play();
    }
    
    // 设置无敌状态
    setInvincible(true);
    
    // 播放闪烁动画
    if (mBlinkAnimation) {
        mBlinkAnimation->start();
    }
    
    emit healthChanged(mHP, mMaxHP);
    
    if (mHP <= 0) {
        emit playerDied();
    }
}

void SugarOilPlayer::heal(int amount)
{
    mHP = qMin(mMaxHP, mHP + amount);
    emit healthChanged(mHP, mMaxHP);
}

void SugarOilPlayer::gainExp(int exp)
{
    mExp += exp;
    emit expChanged(mExp, EXP_PER_LEVEL);
    checkLevelUp();
}

void SugarOilPlayer::shoot(const QPointF &direction)
{
    // 播放射击音效
    if (mShootSoundPlayer) {
        mShootSoundPlayer->setPosition(0);
        mShootSoundPlayer->play();
    }
    
    // 发射子弹信号
    emit playerShoot(getCenterPos(), direction, mAttackPoint);
}

void SugarOilPlayer::setInvincible(bool invincible)
{
    mInvincible = invincible;
    
    if (invincible) {
        mInvincibilityTimer->start(INVINCIBILITY_DURATION);
    } else {
        mInvincibilityTimer->stop();
        setOpacity(1.0);
    }
}

void SugarOilPlayer::resetPlayer()
{
    mHP = mMaxHP;
    mLevel = 1;
    mExp = 0;
    mScore = 0;
    mInvincible = false;
    mIsMoving = false;
    mAnimationFrame = 0;
    
    setOpacity(1.0);
    setPosition(400, 300); // 重置到中心位置
    
    if (mInvincibilityTimer) {
        mInvincibilityTimer->stop();
    }
    if (mBlinkAnimation) {
        mBlinkAnimation->stop();
    }
    
    updatePixmap();
    
    emit healthChanged(mHP, mMaxHP);
    emit expChanged(mExp, EXP_PER_LEVEL);
    emit scoreChanged(mScore);
}

void SugarOilPlayer::updateAnimation()
{
    if (mIsMoving) {
        mAnimationFrame = (mAnimationFrame + 1) % 3; // 3帧动画
    } else {
        mAnimationFrame = 0; // 静止状态
    }
    updatePixmap();
}

void SugarOilPlayer::onInvincibilityTimeout()
{
    setInvincible(false);
}

void SugarOilPlayer::onAnimationTimeout()
{
    updateAnimation();
}

void SugarOilPlayer::updatePixmap()
{
    QString imagePath;
    
    // 根据动画帧和朝向选择图像
    if (mFaceRight) {
        imagePath = QString(":/img/roles/usagi%1.png").arg(mAnimationFrame + 1);
    } else {
        imagePath = QString(":/img/roles/usagi%1_left.png").arg(mAnimationFrame + 1);
    }
    
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    } else {
        // 如果找不到特定图像，使用默认图像
        setPixmap(QPixmap(":/img/roles/usagi1.png"));
    }
}

void SugarOilPlayer::checkLevelUp()
{
    while (mExp >= EXP_PER_LEVEL) {
        mExp -= EXP_PER_LEVEL;
        mLevel++;
        
        // 升级时提升属性
        mMaxHP += 10;
        mHP = mMaxHP; // 升级时回满血
        mAttackPoint += 2;
        mDefence += 1;
        
        emit levelUp(mLevel);
        emit healthChanged(mHP, mMaxHP);
        emit expChanged(mExp, EXP_PER_LEVEL);
    }
}