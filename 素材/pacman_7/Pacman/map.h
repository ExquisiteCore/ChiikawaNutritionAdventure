#ifndef MAP_H
#define MAP_H

#include "config.h"

//抽象类GMap
class GMap
{

public:
    bool WallData[MAPLENGTH][MAPLENGTH];
    int BeanData[MAPLENGTH][MAPLENGTH];
    int RoadData[MAPLENGTH][MAPLENGTH];
    int BeansNum;
    int PR_Bean;                              //超级豆概率

    //friend class GObject;                   // 允许物体类使用直线的起点和终点的信息做碰撞检测
    //friend class PacMan;                    // 允许"大嘴"访问豆子地图
};
//"第一关"
class Stage_1 : public GMap
{
public:
    static const int  initData[MAPLENGTH][MAPLENGTH];	// 地图数据
    Stage_1();
};
//第二关
class Stage_2 : public GMap
{
public:
    static const int initData[MAPLENGTH][MAPLENGTH]; 	// 地图数据

    Stage_2();
};
// 第三关
class Stage_3 : public GMap
{
public:
    static const int initData[MAPLENGTH][MAPLENGTH]; 	// 地图数据

    Stage_3();
};


#endif // MAP_H
