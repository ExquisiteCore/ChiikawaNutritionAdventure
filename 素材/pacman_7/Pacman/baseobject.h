#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QGraphicsPixmapItem>

#include "config.h"

//基本物体类，作为所有游戏物体的基类
class BaseObject:public QGraphicsPixmapItem
{
public:
    BaseObject(int objectType);
    const int ObjectType;     //物体对象的类型

    int X,Y;        //逻辑位置
};

#endif // BASEOBJECT_H
