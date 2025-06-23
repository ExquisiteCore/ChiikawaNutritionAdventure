#include "creature_system.h"
#include "sugar_oil_player.h"
#include <QPixmap>
#include <QUrl>
#include <QDebug>
#include <QtMath>
#include <QPainter>

// GameCreature 实现
GameCreature::GameCreature(CreatureType type, QObject *parent)
    : GameObjectBase(parent)
    , mCreatureType(type)
    , mAnimationFrame(0)
    , mSpeed(1.5)
    , mIsFollowingPlayer(true)
    , mActivateSoundPlayer(nullptr)
    , mActivateSoundAudioOutput(nullptr)
{
    // 初始化动画定时器
    mAnimationTimer = new QTimer(this);
    mAnimationTimer->setInterval(ANIMATION_INTERVAL);
    connect(mAnimationTimer, &QTimer::timeout, this, &GameCreature::onAnimationTimeout);
    mAnimationTimer->start();
    
    // 初始化移动定时器
    mMoveTimer = new QTimer(this);
    mMoveTimer->setInterval(MOVE_INTERVAL);
    connect(mMoveTimer, &QTimer::timeout, this, &GameCreature::onMoveTimeout);
    mMoveTimer->start();
    
    // 初始化音效
    mActivateSoundPlayer = new QMediaPlayer(this);
    mActivateSoundAudioOutput = new QAudioOutput(this);
    mActivateSoundPlayer->setAudioOutput(mActivateSoundAudioOutput);
    mActivateSoundPlayer->setSource(QUrl("qrc:/sounds/Bean_sound_short.wav"));
    mActivateSoundAudioOutput->setVolume(0.6f);
    
    setupEffect();
    updatePixmap();
    setScale(0.7);
    setZValue(2);
}

GameCreature::~GameCreature()
{
    if (mAnimationTimer) {
        mAnimationTimer->stop();
    }
    if (mMoveTimer) {
        mMoveTimer->stop();
    }
}

void GameCreature::setupEffect()
{
    switch (mCreatureType) {
    case CREATURE_HELPER: // 健身教练 - 提供攻击力加成
        mEffect.attackBonus = 0.3f; // 30%攻击力加成
        mEffect.duration = 15000;   // 15秒
        break;
        
    case CREATURE_GUARDIAN: // 营养师 - 提供生命恢复
        mEffect.healthRegen = 2;    // 每秒恢复2点生命
        mEffect.duration = 20000;   // 20秒
        break;
        
    case CREATURE_HEALER: // 私人教练 - 提供速度加成
        mEffect.speedBonus = 0.4f;  // 40%速度加成
        mEffect.duration = 12000;   // 12秒
        break;
        
    case CREATURE_SPEEDSTER: // 励志大师 - 提供经验加成
        mEffect.expBonus = 1.5f;    // 50%经验加成
        mEffect.duration = 25000;   // 25秒
        break;
        
    case CREATURE_WARRIOR: // 健康专家 - 提供防御加成
        mEffect.defenseBonus = 0.5f; // 50%防御加成
        mEffect.duration = 18000;    // 18秒
        break;
    }
}

void GameCreature::updatePixmap()
{
    QString imagePath;
    
    // 根据生物类型选择对应的角色图片
    switch (mCreatureType) {
    case CREATURE_HELPER:
        imagePath = ":/images/roles/usagi1.png"; // 使用兔子1作为健身教练
        break;
    case CREATURE_GUARDIAN:
        imagePath = ":/images/roles/usagi2.png"; // 使用兔子2作为营养师
        break;
    case CREATURE_HEALER:
        imagePath = ":/images/roles/usagi3.png"; // 使用兔子3作为私人教练
        break;
    case CREATURE_SPEEDSTER:
        imagePath = ":/images/roles/chimera1.png"; // 使用奇美拉1作为励志大师
        break;
    case CREATURE_WARRIOR:
        imagePath = ":/images/roles/chimera2.png"; // 使用奇美拉2作为健康专家
        break;
    }
    
    QPixmap pixmap(imagePath);
    
    if (pixmap.isNull()) {
        // 如果没有对应的图片，创建一个简单的彩色圆形
        pixmap = QPixmap(48, 48);
        pixmap.fill(Qt::transparent);
        
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        
        QColor colors[] = {Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta};
        painter.setBrush(colors[static_cast<int>(mCreatureType) % 5]);
        painter.setPen(Qt::black);
        painter.drawEllipse(4, 4, 40, 40);
    }
    
    setPixmap(pixmap);
}

void GameCreature::activateEffect(SugarOilPlayer* player)
{
    if (!player) return;
    
    // 播放激活音效
    if (mActivateSoundPlayer) {
        mActivateSoundPlayer->play();
    }
    
    mEffect.isActive = true;
    
    qDebug() << "Activated creature effect:" << CreatureManager::getCreatureName(mCreatureType);
    
    // 这里需要在玩家类中实现具体的效果应用逻辑
    // 目前只是标记效果为激活状态
}

void GameCreature::updateCreature()
{
    // 简单的浮动动画
    static qreal offset = 0;
    offset += 0.05;
    
    QPointF currentPos = pos();
    setPos(currentPos.x(), currentPos.y() + qSin(offset) * 1.5);
}

void GameCreature::moveTowardsPlayer(const QPointF& playerPos)
{
    if (!mIsFollowingPlayer) return;
    
    QPointF currentPos = pos();
    QPointF direction = playerPos - currentPos;
    qreal distance = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    
    if (distance > 5.0) { // 如果距离玩家较远，则移动
        direction /= distance; // 标准化方向向量
        QPointF newPos = currentPos + direction * mSpeed;
        setPos(newPos);
    }
}

bool GameCreature::isNearPlayer(const QPointF& playerPos, qreal distance) const
{
    QPointF currentPos = pos();
    QPointF diff = playerPos - currentPos;
    qreal actualDistance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
    return actualDistance <= distance;
}

void GameCreature::onAnimationTimeout()
{
    updateCreature();
    mAnimationFrame = (mAnimationFrame + 1) % 4; // 4帧动画循环
}

void GameCreature::onMoveTimeout()
{
    // 移动逻辑在游戏场景中调用 moveTowardsPlayer
}

// CreatureManager 实现
CreatureManager::CreatureManager(QObject *parent)
    : QObject(parent)
    , mRandomGenerator(QRandomGenerator::global())
{
}

CreatureManager::~CreatureManager()
{
}

GameCreature* CreatureManager::spawnRandomCreature(const QPointF& position)
{
    // 随机选择一个生物类型
    int creatureTypeIndex = mRandomGenerator->bounded(5); // 0-4
    CreatureType type = static_cast<CreatureType>(creatureTypeIndex);
    
    GameCreature* creature = new GameCreature(type);
    creature->setPos(position);
    
    return creature;
}

QString CreatureManager::getCreatureDescription(CreatureType type)
{
    switch (type) {
    case CREATURE_HELPER:
        return "健身教练：提供30%攻击力加成，持续15秒";
    case CREATURE_GUARDIAN:
        return "营养师：每秒恢复2点生命值，持续20秒";
    case CREATURE_HEALER:
        return "私人教练：提供40%速度加成，持续12秒";
    case CREATURE_SPEEDSTER:
        return "励志大师：提供50%经验加成，持续25秒";
    case CREATURE_WARRIOR:
        return "健康专家：提供50%防御加成，持续18秒";
    default:
        return "未知生物";
    }
}

QString CreatureManager::getCreatureName(CreatureType type)
{
    switch (type) {
    case CREATURE_HELPER:
        return "健身教练";
    case CREATURE_GUARDIAN:
        return "营养师";
    case CREATURE_HEALER:
        return "私人教练";
    case CREATURE_SPEEDSTER:
        return "励志大师";
    case CREATURE_WARRIOR:
        return "健康专家";
    default:
        return "未知生物";
    }
}