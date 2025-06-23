#include "item_system.h"
#include "sugar_oil_player.h"
#include <QPixmap>
#include <QUrl>
#include <QDebug>

// GameItem 实现
GameItem::GameItem(ItemType type, QObject *parent)
    : GameObjectBase(parent)
    , mItemType(type)
    , mAnimationFrame(0)
    , mPickupSoundPlayer(nullptr)
    , mPickupSoundAudioOutput(nullptr)
{
    // 初始化动画定时器
    mAnimationTimer = new QTimer(this);
    mAnimationTimer->setInterval(ANIMATION_INTERVAL);
    connect(mAnimationTimer, &QTimer::timeout, this, &GameItem::onAnimationTimeout);
    mAnimationTimer->start();
    
    // 初始化音效
    mPickupSoundPlayer = new QMediaPlayer(this);
    mPickupSoundAudioOutput = new QAudioOutput(this);
    mPickupSoundPlayer->setAudioOutput(mPickupSoundAudioOutput);
    mPickupSoundPlayer->setSource(QUrl("qrc:/sounds/TapButton.wav"));
    mPickupSoundAudioOutput->setVolume(0.5f);
    
    setupEffect();
    updatePixmap();
    setScale(0.2);
    setZValue(2);
}

GameItem::~GameItem()
{
    if (mAnimationTimer) {
        mAnimationTimer->stop();
    }
}

void GameItem::setupEffect()
{
    switch (mItemType) {
    // 基础营养素 (8种)
    case ITEM_SPEED_BOOST:
        mEffect.speedMultiplier = 1.5f;
        mEffect.duration = 5000; // 5秒
        break;
    case ITEM_DAMAGE_BOOST:
        mEffect.attackMultiplier = 1.5f;
        mEffect.duration = 8000; // 8秒
        break;
    case ITEM_SHIELD:
        mEffect.defenseMultiplier = 2.0f;
        mEffect.duration = 10000; // 10秒
        break;
    case ITEM_HEALTH:
        mEffect.healthRestore = 30;
        break;
    case ITEM_SCORE_BONUS:
        mEffect.scoreBonus = 100;
        break;
    case ITEM_TIME_SLOW:
        // 这个效果需要在游戏场景中实现
        mEffect.duration = 3000; // 3秒
        break;
    case ITEM_INVINCIBLE:
        mEffect.invincible = true;
        mEffect.duration = 3000; // 3秒
        break;
    case ITEM_DOUBLE_SCORE:
        mEffect.duration = 15000; // 15秒
        break;
        
    // 健身装备 (8种)
    case ITEM_EXTRA_LIFE:
        mEffect.healthRestore = 50;
        break;
    case ITEM_FREEZE_ENEMIES:
        // 冰冻敌人效果需要在游戏场景中实现
        mEffect.duration = 2000; // 2秒
        break;
    case ITEM_MAGNET:
        mEffect.magnetPower = true;
        mEffect.duration = 10000; // 10秒
        break;
    case ITEM_BOMB:
        // 炸弹效果需要在游戏场景中实现
        break;
    case ITEM_RAPID_FIRE:
        mEffect.rapidFire = true;
        mEffect.duration = 8000; // 8秒
        break;
    case ITEM_ENERGY_DRINK:
        mEffect.speedMultiplier = 1.3f;
        mEffect.attackMultiplier = 1.2f;
        mEffect.duration = 6000; // 6秒
        break;
    case ITEM_PROTEIN_BAR:
        mEffect.healthRestore = 20;
        mEffect.attackMultiplier = 1.1f;
        mEffect.duration = 5000; // 5秒
        break;
    case ITEM_VITAMIN:
        mEffect.healthRestore = 15;
        mEffect.speedMultiplier = 1.1f;
        mEffect.duration = 4000; // 4秒
        break;
        
    // 特殊道具 (8种)
    case ITEM_MINERAL:
        mEffect.defenseMultiplier = 1.3f;
        mEffect.duration = 7000; // 7秒
        break;
    case ITEM_FIBER:
        mEffect.healthRestore = 25;
        break;
    case ITEM_ANTIOXIDANT:
        mEffect.invincible = true;
        mEffect.duration = 2000; // 2秒
        break;
    case ITEM_OMEGA3:
        mEffect.speedMultiplier = 1.2f;
        mEffect.defenseMultiplier = 1.2f;
        mEffect.duration = 6000; // 6秒
        break;
    case ITEM_CALCIUM:
        mEffect.defenseMultiplier = 1.4f;
        mEffect.duration = 8000; // 8秒
        break;
    case ITEM_IRON:
        mEffect.attackMultiplier = 1.3f;
        mEffect.duration = 7000; // 7秒
        break;
    case ITEM_ZINC:
        mEffect.speedMultiplier = 1.4f;
        mEffect.duration = 6000; // 6秒
        break;
    case ITEM_MULTIVITAMIN:
        mEffect.speedMultiplier = 1.2f;
        mEffect.attackMultiplier = 1.2f;
        mEffect.defenseMultiplier = 1.2f;
        mEffect.healthRestore = 20;
        mEffect.duration = 10000; // 10秒
        break;
    }
}

void GameItem::updatePixmap()
{
    QString imagePath = QString(":/images/items/itemicon%1.png").arg(static_cast<int>(mItemType));
    QPixmap pixmap(imagePath);
    
    if (pixmap.isNull()) {
        // 如果没有对应的图片，创建一个简单的彩色方块
        pixmap = QPixmap(32, 32);
        QColor colors[] = {Qt::red, Qt::green, Qt::blue, Qt::yellow, Qt::cyan, Qt::magenta};
        pixmap.fill(colors[static_cast<int>(mItemType) % 6]);
    }
    
    setPixmap(pixmap);
}

void GameItem::applyEffect(SugarOilPlayer* player)
{
    if (!player) return;
    
    // 播放拾取音效
    if (mPickupSoundPlayer) {
        mPickupSoundPlayer->play();
    }
    
    // 应用瞬间效果
    if (mEffect.healthRestore > 0) {
        player->heal(mEffect.healthRestore);
    }
    
    if (mEffect.scoreBonus > 0) {
        player->gainScore(mEffect.scoreBonus);
    }
    
    // 持续效果需要在玩家类中实现
    // 这里只是触发效果的应用
    qDebug() << "Applied item effect:" << ItemManager::getItemName(mItemType);
}

void GameItem::updateAnimation()
{
    // 简单的上下浮动动画
    static qreal offset = 0;
    offset += 0.1;
    setPos(pos().x(), pos().y() + qSin(offset) * 2);
}

void GameItem::onAnimationTimeout()
{
    updateAnimation();
}

// ItemManager 实现
ItemManager::ItemManager(QObject *parent)
    : QObject(parent)
    , mRandomGenerator(QRandomGenerator::global())
{
}

ItemManager::~ItemManager()
{
}

GameItem* ItemManager::spawnRandomItem(const QPointF& position)
{
    // 随机选择一个道具类型
    int itemTypeIndex = mRandomGenerator->bounded(24); // 0-23
    ItemType type = static_cast<ItemType>(itemTypeIndex);
    
    GameItem* item = new GameItem(type);
    item->setPos(position);
    
    return item;
}

QString ItemManager::getItemDescription(ItemType type)
{
    switch (type) {
    case ITEM_SPEED_BOOST: return "提升移动速度50%，持续5秒";
    case ITEM_DAMAGE_BOOST: return "提升攻击力50%，持续8秒";
    case ITEM_SHIELD: return "提升防御力100%，持续10秒";
    case ITEM_HEALTH: return "立即恢复30点生命值";
    case ITEM_SCORE_BONUS: return "获得100分奖励";
    case ITEM_TIME_SLOW: return "减缓时间流逝，持续3秒";
    case ITEM_INVINCIBLE: return "获得无敌状态，持续3秒";
    case ITEM_DOUBLE_SCORE: return "双倍分数，持续15秒";
    case ITEM_EXTRA_LIFE: return "恢复50点生命值";
    case ITEM_FREEZE_ENEMIES: return "冰冻所有敌人2秒";
    case ITEM_MAGNET: return "磁力吸引道具，持续10秒";
    case ITEM_BOMB: return "清除屏幕上所有敌人";
    case ITEM_RAPID_FIRE: return "快速射击，持续8秒";
    case ITEM_ENERGY_DRINK: return "速度+30%，攻击+20%，持续6秒";
    case ITEM_PROTEIN_BAR: return "恢复20生命，攻击+10%，持续5秒";
    case ITEM_VITAMIN: return "恢复15生命，速度+10%，持续4秒";
    case ITEM_MINERAL: return "防御+30%，持续7秒";
    case ITEM_FIBER: return "恢复25点生命值";
    case ITEM_ANTIOXIDANT: return "无敌状态2秒";
    case ITEM_OMEGA3: return "速度+20%，防御+20%，持续6秒";
    case ITEM_CALCIUM: return "防御+40%，持续8秒";
    case ITEM_IRON: return "攻击+30%，持续7秒";
    case ITEM_ZINC: return "速度+40%，持续6秒";
    case ITEM_MULTIVITAMIN: return "全属性+20%，恢复20生命，持续10秒";
    default: return "未知道具";
    }
}

QString ItemManager::getItemName(ItemType type)
{
    switch (type) {
    case ITEM_SPEED_BOOST: return "速度提升";
    case ITEM_DAMAGE_BOOST: return "攻击提升";
    case ITEM_SHIELD: return "护盾";
    case ITEM_HEALTH: return "生命药水";
    case ITEM_SCORE_BONUS: return "分数奖励";
    case ITEM_TIME_SLOW: return "时间减缓";
    case ITEM_INVINCIBLE: return "无敌星星";
    case ITEM_DOUBLE_SCORE: return "双倍分数";
    case ITEM_EXTRA_LIFE: return "额外生命";
    case ITEM_FREEZE_ENEMIES: return "冰冻敌人";
    case ITEM_MAGNET: return "磁力吸引";
    case ITEM_BOMB: return "炸弹";
    case ITEM_RAPID_FIRE: return "快速射击";
    case ITEM_ENERGY_DRINK: return "能量饮料";
    case ITEM_PROTEIN_BAR: return "蛋白棒";
    case ITEM_VITAMIN: return "维生素";
    case ITEM_MINERAL: return "矿物质";
    case ITEM_FIBER: return "纤维";
    case ITEM_ANTIOXIDANT: return "抗氧化剂";
    case ITEM_OMEGA3: return "欧米伽3";
    case ITEM_CALCIUM: return "钙片";
    case ITEM_IRON: return "铁剂";
    case ITEM_ZINC: return "锌片";
    case ITEM_MULTIVITAMIN: return "复合维生素";
    default: return "未知道具";
    }
}