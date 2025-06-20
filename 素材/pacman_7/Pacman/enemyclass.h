#ifndef ENEMYCLASS_H
#define ENEMYCLASS_H

#include <QMovie>

#include "config.h"
#include "playerclass.h"

class EnemyClass : public BaseMoveObject
{
public:
    EnemyClass(GMap *map, const PlayerClass* player, int x, int y, int objecttype=_ENEMY);   //map为地图，player为玩家指针,对象类型默认为敌人
    virtual ~EnemyClass();

    //重写移动函数
    void Move() override;

    //使用更简单的撞墙函数
    bool CollideWall(void);

    //静止动态图
    void Pause(void);

    //恢复动态图
    void Resume(void);

    //切换惊吓模式
    void FrightenShift(void);

    //是否惊吓标记
    bool Frightened;

    //标记，是否在受惊吓后被玩家吃掉，处于回退状态
    bool BackHome;

    //标记，表明是否在触发超级豆后被吃
    bool HasEaten;

    //标记对象类型
    enum { Type = _ENEMY };
    int type() const override { return Type; }

    //初始坐标,当被超级豆时间玩家撞到时候，回到该点
    const int initX; const int initY;

protected:
    //追踪函数
    bool trace();

    //四个方向动态图片的缓存
    QMovie* Buffer_Images[4];

    //受惊吓时的动态图
    QMovie* Frighten_Image;

    //玩家类的指针
    const PlayerClass* Player;

    //警戒范围
    int AlertDist;

    //随机步长
    int LastSteps;

    //上一个移动方向，用来判定是否转向
    int LastDir;

    //追踪模式
    int flag_trace_mode;




};

class Enemy1:public EnemyClass
{
public:
    Enemy1(GMap *map,const PlayerClass* player);
    ~Enemy1();
};

class Enemy2:public EnemyClass
{
public:
    Enemy2(GMap *map,const PlayerClass* player);
    ~Enemy2();
};
class Enemy3:public EnemyClass
{
public:
    Enemy3(GMap *map,const PlayerClass* player);
    ~Enemy3();
};

#endif // ENEMYCLASS_H
