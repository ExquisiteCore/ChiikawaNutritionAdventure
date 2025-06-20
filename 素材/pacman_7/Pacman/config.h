#ifndef CONFIG_H
#define CONFIG_H

///////////////////窗口大小///////////////////
#define MAIN_WINDOW_WIDTH 800
#define MAIN_WINDOW_HEIGHT 800

#define LEVEL_WINDOW_WIDTH 800
#define LEVEL_WINDOW_HEIGHT 800

#define RULE_WINDOW_WIDTH 800
#define RULE_WINDOW_HEIGHT 800

#define GAME_WINDOW_WIDTH 1300
#define GAME_WINDOW_HEIGHT 1000
#define GAMESCENE_WIDTH 1000    //游戏Scene宽度
#define GAMESCENE_HEIGHT 1000   //游戏Scene长度


/////////////////////////////////////GMap地图类常数////////////////////////////////////
#define MAPLENGTH 19			// 逻辑地图大小
#define ENEMY_NUMBERS 3         // 敌人数量
#define P_ROW 10				// 我方的位置坐标
#define P_COL 9					// 我方的位置坐标
#define ENEMY1_ROW 8            // 敌方1的位置坐标
#define ENEMY1_COL 8            // 敌方的位置坐标
#define ENEMY2_ROW 8            // 敌方2的位置坐标
#define ENEMY2_COL 9            // 敌方2的位置坐标
#define ENEMY3_ROW 8			// 敌方3的位置坐标
#define ENEMY3_COL 10           // 敌方3的位置坐标



#define OBJECT_SIZE 50          //物品大小
#define BRICK_SIZE 50           //砖块大小
#define HALF_BRICK_SIZE 25      //半个砖块大小
#define BEAN_SIZE 6             //豆子大小
#define PROP_SIZE 20

#define PLAYER_SIZE 30          //玩家大小
#define ENEMY_SIZE 40           //敌人大小
#define CRASH_DISTANCE 15       //玩家和敌人碰撞的判定距离

//////////////////////////////////////游戏进行常数////////////////////////////////////////
enum OBJECT_FLAG{ _PLAYER=65536, _ENEMY=65537, _SLOWVIRUS=65538, _SUPERBEAN=65539, _BEAN=4,};       //物品的类别标记
enum DIRECTION{DIR_LEFT=0,DIR_UP=1,DIR_RIGHT=2,DIR_DOWN=3};     //方向标记
const int DIR_VALUE[4][2]={{0,-1},{-1,0},{0,1},{1,0}};  //方向对应的移动指令
enum GAME_STATUS{GAME_READY=0,GAME_RUN=1,GAME_END=2};   //游戏当前状态标记

#define INIT_PLAYER_VELOCITY 5          //初始玩家速度
#define INIT_ENEMY_VELOCITY 4           //初始敌人速度

#define GAME_TIMER_SPEED 20   //游戏主控时钟的间隔，单位为毫秒
#define PROPS_ADD_INTERVAL 5000    //加入道具的间隔时间
#define SLOW_DOWN_TIME 3000        //总共减速时间
#define SLOW_DOWN_INTERVAL (SLOW_DOWN_TIME/100)     //减速的时间间隔
#define SUPER_BEAN_TIME 5000       //超级豆总共时间
#define SUPER_BEAN_INTERVAL (SUPER_BEAN_TIME/100)    //超级豆时钟的时间间隔
#define SUPER_BEAN_FREEZE 1000     //敌人冰冻的时间

#define SUPER_BEAN_SCORE 50     //超级豆分数
#define BEAN_SCORE 20           //普通豆分数
#define ENEMY_SCORE 200

#define ALERT_DISTANCE 6 //敌人警戒范围，单位为砖块长度

#define _NO_TRACING 0       //不追踪标记
#define _WEAK_TRACING 1     //弱追踪标记
#define _STRONG_TRACING 2   //强追踪标记

//////////关卡锁////////////

extern bool passedLevelNum[4];  //对每个类引用config.cpp里的关卡锁变量


#include <QDebug>

#endif // CONFIG_H
