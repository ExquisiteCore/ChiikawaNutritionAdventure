#ifndef SUGAR_OIL_CONFIG_H
#define SUGAR_OIL_CONFIG_H

// 游戏窗口配置
#define SUGAR_OIL_WINDOW_WIDTH 1020
#define SUGAR_OIL_WINDOW_HEIGHT 640
#define SUGAR_OIL_SCENE_WIDTH 800
#define SUGAR_OIL_SCENE_HEIGHT 600

// 游戏时间配置
#define GAME_DURATION_SECONDS 300  // 5分钟 = 300秒 - 坚持300秒即可取得胜利
#define TIMER_UPDATE_INTERVAL 16   // 约60FPS

// 角色配置
#define USAGI_SIZE 4  // 再次缩小角色尺寸
#define USAGI_SPEED 4  // 从5减小到4，与模式1保持一致
#define USAGI_INITIAL_LIVES 3

// 敌人配置
#define SUGAR_OIL_ENEMY_SIZE 4  // 再次缩小敌人尺寸
#define SUGAR_OIL_ENEMY_SPEED 2  // 从3减小到2，降低敌人速度
#define SUGAR_OIL_ENEMY_COUNT 5
#define SUGAR_OIL_ENEMY_SPAWN_INTERVAL 3000  // 3秒生成一个敌人

// 道具配置
#define ITEM_SIZE 16
#define ITEM_COUNT 24
#define ITEM_SPAWN_INTERVAL 2000  // 2秒生成一个道具

// 生物配置
#define CREATURE_SIZE 20
#define CREATURE_COUNT 5
#define CREATURE_SPAWN_INTERVAL 5000  // 5秒生成一个生物

// 属性加成配置
#define BUFF_DURATION 10000  // 10秒
#define SPEED_BUFF_MULTIPLIER 1.5
#define DAMAGE_BUFF_MULTIPLIER 2.0
#define SHIELD_BUFF_DURATION 5000  // 5秒无敌

// 碰撞检测
#define SUGAR_OIL_COLLISION_DISTANCE 20

// 游戏对象类型
enum SugarOilGameObjectType {
    TYPE_USAGI = 2001,
    TYPE_SUGAR_OIL_ENEMY = 2002,
    TYPE_GAME_ITEM = 2003,
    TYPE_CREATURE = 2004,
    TYPE_BUFF = 2005
};

// 游戏状态
enum SugarOilGameState {
    SUGAR_OIL_MENU = 0,
    SUGAR_OIL_RUNNING = 1,
    SUGAR_OIL_PAUSED = 2,
    SUGAR_OIL_WON = 3,
    SUGAR_OIL_LOST = 4
};

// 移动方向
enum SugarOilDirection {
    SUGAR_OIL_DIR_NONE = -1,
    SUGAR_OIL_DIR_LEFT = 0,
    SUGAR_OIL_DIR_UP = 1,
    SUGAR_OIL_DIR_RIGHT = 2,
    SUGAR_OIL_DIR_DOWN = 3
};

// 方向向量
const int SUGAR_OIL_DIR_OFFSET[4][2] = {
    {-1, 0},  // LEFT
    {0, -1},  // UP
    {1, 0},   // RIGHT
    {0, 1}    // DOWN
};

// 道具类型 - 24种物品
enum ItemType {
    // 基础营养素 (8种)
    ITEM_SPEED_BOOST = 0,     // 速度提升
    ITEM_DAMAGE_BOOST = 1,    // 攻击力提升
    ITEM_SHIELD = 2,          // 护盾
    ITEM_HEALTH = 3,          // 生命恢复
    ITEM_SCORE_BONUS = 4,     // 分数加成
    ITEM_TIME_SLOW = 5,       // 时间减缓
    ITEM_INVINCIBLE = 6,      // 无敌
    ITEM_DOUBLE_SCORE = 7,    // 双倍分数
    
    // 健身装备 (8种)
    ITEM_EXTRA_LIFE = 8,      // 额外生命
    ITEM_FREEZE_ENEMIES = 9,  // 冰冻敌人
    ITEM_MAGNET = 10,         // 磁力吸引
    ITEM_BOMB = 11,           // 炸弹
    ITEM_RAPID_FIRE = 12,     // 快速射击
    ITEM_ENERGY_DRINK = 13,   // 能量饮料
    ITEM_PROTEIN_BAR = 14,    // 蛋白棒
    ITEM_VITAMIN = 15,        // 维生素
    
    // 特殊道具 (8种)
    ITEM_MINERAL = 16,        // 矿物质
    ITEM_FIBER = 17,          // 纤维
    ITEM_ANTIOXIDANT = 18,    // 抗氧化剂
    ITEM_OMEGA3 = 19,         // 欧米伽3
    ITEM_CALCIUM = 20,        // 钙
    ITEM_IRON = 21,           // 铁
    ITEM_ZINC = 22,           // 锌
    ITEM_MULTIVITAMIN = 23    // 复合维生素
};

// 生物类型 - 5种奇异生物
enum CreatureType {
    CREATURE_HELPER = 0,      // 健身教练 - 提供攻击力加成
    CREATURE_GUARDIAN = 1,    // 营养师 - 提供生命恢复
    CREATURE_HEALER = 2,      // 私人教练 - 提供速度加成
    CREATURE_SPEEDSTER = 3,   // 励志大师 - 提供经验加成
    CREATURE_WARRIOR = 4      // 健康专家 - 提供防御加成
};

// 敌人类型 - 5种糖油混合物
enum SugarOilEnemyType {
    ENEMY_FRIED_CHICKEN = 0,  // 炸鸡
    ENEMY_BARBECUE = 1,       // 烧烤
    ENEMY_MILK_TEA = 2,       // 奶茶
    ENEMY_SNAIL_NOODLES = 3,  // 螺蛳粉
    ENEMY_CAKE = 4            // 小蛋糕
};

#endif // SUGAR_OIL_CONFIG_H