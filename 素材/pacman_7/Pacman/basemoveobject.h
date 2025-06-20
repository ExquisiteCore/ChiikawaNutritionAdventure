#ifndef BASEMOVEOBJECT_H
#define BASEMOVEOBJECT_H

#include <QGraphicsPixmapItem>
#include <QMovie>

#include "config.h"
#include "baseobject.h"
#include "map.h"


//基本移动物体类，是移动物体的基类，继承自基本物体类
class BaseMoveObject:public BaseObject
{
public:
    BaseMoveObject(int objectType,GMap* map);

    //设置速度
    void SetVelocity(int setVelocity);

    //获得速度
    int GetVelocity();

    //判断是否撞墙
    bool CollideWall();

    //移动物体
    virtual void Move();

    //获得当前的逻辑位置X（从上到下的行数）
    int getX(void) const{return X;}

    //获得当前的逻辑位置Y(从左到右的列数
    int getY(void) const{return Y;}

    //减速的程度
    int degree_of_deceleration = 0;

    int Dir_X,Dir_Y;    //与移动指令相关的逻辑位置变化量
    int Dir;            //移动指令

protected:
    QMovie *Image;  //动态图
    GMap* Map;      //地图指针
    int Velocity;   //速度
    qreal Step;     //步幅（通过速度计算得到的每一步大小)

    bool Ctrl;      //标记，判断是否到方格中心

};

#endif // BASEMOVEOBJECT_H
