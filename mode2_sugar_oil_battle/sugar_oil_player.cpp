#include "sugar_oil_player.h"
#include <QPixmap>
#include <QUrl>
#include <QRandomGenerator>

SugarOilPlayer::SugarOilPlayer(QObject *parent)
    : GameObjectBase(parent)
    , mHP(100)
    , mMaxHP(100)
    , mSpeed(5.0)
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
    , mSpeedEffectTimer(nullptr)
    , mAttackEffectTimer(nullptr)
    , mDefenseEffectTimer(nullptr)
    , mFastShootingTimer(nullptr)
    , mMagnetismTimer(nullptr)
    , mExperienceEffectTimer(nullptr)
    , mSpeedMultiplier(1.0)
    , mAttackMultiplier(1.0)
    , mDefenseMultiplier(1.0)
    , mExperienceMultiplier(1.0)
    , mFastShootingEnabled(false)
    , mMagnetismEnabled(false)
    , mShootSoundPlayer(nullptr)
    , mHurtSoundPlayer(nullptr)
    , mShootSoundAudioOutput(nullptr)
    , mHurtSoundAudioOutput(nullptr)
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
    
    // 初始化效果定时器
    mSpeedEffectTimer = new QTimer(this);
    mSpeedEffectTimer->setSingleShot(true);
    connect(mSpeedEffectTimer, &QTimer::timeout, [this]() { mSpeedMultiplier = 1.0; });
    
    mAttackEffectTimer = new QTimer(this);
    mAttackEffectTimer->setSingleShot(true);
    connect(mAttackEffectTimer, &QTimer::timeout, [this]() { mAttackMultiplier = 1.0; });
    
    mDefenseEffectTimer = new QTimer(this);
    mDefenseEffectTimer->setSingleShot(true);
    connect(mDefenseEffectTimer, &QTimer::timeout, [this]() { mDefenseMultiplier = 1.0; });
    
    mFastShootingTimer = new QTimer(this);
    mFastShootingTimer->setSingleShot(true);
    connect(mFastShootingTimer, &QTimer::timeout, [this]() { mFastShootingEnabled = false; });
    
    mMagnetismTimer = new QTimer(this);
    mMagnetismTimer->setSingleShot(true);
    connect(mMagnetismTimer, &QTimer::timeout, [this]() { mMagnetismEnabled = false; });
    
    mExperienceEffectTimer = new QTimer(this);
    mExperienceEffectTimer->setSingleShot(true);
    connect(mExperienceEffectTimer, &QTimer::timeout, [this]() { mExperienceMultiplier = 1.0; });
    
    // 初始化音效播放器 (Qt5兼容)
    mShootSoundPlayer = new QMediaPlayer(this);
    mShootSoundAudioOutput = new QAudioOutput(this);
    mShootSoundPlayer->setAudioOutput(mShootSoundAudioOutput);
    mShootSoundPlayer->setSource(QUrl("qrc:/sounds/shoot.wav"));

    mHurtSoundPlayer = new QMediaPlayer(this);
    mHurtSoundAudioOutput = new QAudioOutput(this);
    mHurtSoundPlayer->setAudioOutput(mHurtSoundAudioOutput);
    mHurtSoundPlayer->setSource(QUrl("qrc:/sounds/hurt.wav"));
    
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
    
    int effectiveDefense = static_cast<int>(mDefence * mDefenseMultiplier);
    int actualDamage = qMax(1, damage - effectiveDefense);
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
    int effectiveExp = static_cast<int>(exp * mExperienceMultiplier);
    mExp += effectiveExp;
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
    
    // 计算有效攻击力
    int effectiveAttack = static_cast<int>(mAttackPoint * mAttackMultiplier);
    
    // 发射子弹信号
    emit playerShoot(getCenterPos(), direction, effectiveAttack);
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

// 道具效果方法
void SugarOilPlayer::applySpeedMultiplier(double multiplier, int duration)
{
    mSpeedMultiplier = multiplier;
    mSpeedEffectTimer->start(duration);
}

void SugarOilPlayer::applyAttackMultiplier(double multiplier, int duration)
{
    mAttackMultiplier = multiplier;
    mAttackEffectTimer->start(duration);
}

void SugarOilPlayer::applyDefenseMultiplier(double multiplier, int duration)
{
    mDefenseMultiplier = multiplier;
    mDefenseEffectTimer->start(duration);
}

void SugarOilPlayer::enableFastShooting(int duration)
{
    mFastShootingEnabled = true;
    mFastShootingTimer->start(duration);
}

void SugarOilPlayer::enableMagnetism(int duration)
{
    mMagnetismEnabled = true;
    mMagnetismTimer->start(duration);
}

void SugarOilPlayer::applyExperienceMultiplier(double multiplier, int duration)
{
    mExperienceMultiplier = multiplier;
    mExperienceEffectTimer->start(duration);
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
    
    // 重置所有效果
    mSpeedMultiplier = 1.0;
    mAttackMultiplier = 1.0;
    mDefenseMultiplier = 1.0;
    mExperienceMultiplier = 1.0;
    mFastShootingEnabled = false;
    mMagnetismEnabled = false;
    
    setOpacity(1.0);
    setPosition(400, 300); // 重置到中心位置
    
    if (mInvincibilityTimer) {
        mInvincibilityTimer->stop();
    }
    if (mBlinkAnimation) {
        mBlinkAnimation->stop();
    }
    
    // 停止所有效果定时器
    if (mSpeedEffectTimer && mSpeedEffectTimer->isActive()) {
        mSpeedEffectTimer->stop();
    }
    if (mAttackEffectTimer && mAttackEffectTimer->isActive()) {
        mAttackEffectTimer->stop();
    }
    if (mDefenseEffectTimer && mDefenseEffectTimer->isActive()) {
        mDefenseEffectTimer->stop();
    }
    if (mFastShootingTimer && mFastShootingTimer->isActive()) {
        mFastShootingTimer->stop();
    }
    if (mMagnetismTimer && mMagnetismTimer->isActive()) {
        mMagnetismTimer->stop();
    }
    if (mExperienceEffectTimer && mExperienceEffectTimer->isActive()) {
        mExperienceEffectTimer->stop();
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