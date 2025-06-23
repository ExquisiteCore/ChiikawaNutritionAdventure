#ifndef CARBOHYDRATE_CONFIG_H
#define CARBOHYDRATE_CONFIG_H

// 游戏窗口配置
#define GAME_WINDOW_WIDTH 1200
#define GAME_WINDOW_HEIGHT 900
#define GAME_SCENE_WIDTH 1000
#define GAME_SCENE_HEIGHT 800

// 地图配置
#define MAP_ROWS 21
#define MAP_COLS 25
#define CELL_SIZE 32

// 角色配置
#define PLAYER_SIZE 28
#define ENEMY_SIZE 28
#define FIBER_SWORD_SIZE 20
#define FAKE_VEGETABLE_SIZE 8

// 游戏机制配置
#define INITIAL_FIBER_VALUE 100
#define FIBER_SWORD_COST 10
#define FIBER_SWORD_DAMAGE 25
#define BOSS_HEALTH 100
#define MOVEMENT_SPEED 4
#define CARBOHYDRATE_COLLISION_DISTANCE 16

// 游戏对象类型
enum GameObjectType {
    TYPE_PLAYER = 1001,
    TYPE_BOSS = 1002,
    TYPE_FIBER_SWORD = 1003,
    TYPE_FAKE_VEGETABLE = 1004,
    TYPE_WALL = 1005
};

// 移动方向
enum Direction {
    DIR_NONE = -1,
    DIR_LEFT = 0,
    DIR_UP = 1,
    DIR_RIGHT = 2,
    DIR_DOWN = 3
};

// 方向向量
const int DIR_OFFSET[4][2] = {
    {-1, 0},  // LEFT
    {0, -1},  // UP
    {1, 0},   // RIGHT
    {0, 1}    // DOWN
};

// 游戏状态
enum GameState {
    GAME_READY = 0,
    GAME_RUNNING = 1,
    GAME_PAUSED = 2,
    GAME_WIN = 3,
    GAME_LOSE = 4
};

#endif // CARBOHYDRATE_CONFIG_H