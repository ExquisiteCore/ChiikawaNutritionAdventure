#ifndef PROPSOBJECT_H
#define PROPSOBJECT_H

#include <QPixmap>

#include "baseobject.h"
#include "config.h"

class PropsObject : public BaseObject
{
public:
    PropsObject(short objectType);


};

//减速道具
class SlowDownPacman : public PropsObject
{
public:
    SlowDownPacman(int x, int y);

    //标记对象类型
    enum { Type = _SLOWVIRUS };
    int type() const override { return Type; }
};

//超级豆道具
class SuperBean : public PropsObject
{
public:
    SuperBean(int x, int y);

    //标记对象类型
    enum { Type = _SUPERBEAN };
    int type() const override { return Type; }
};

#endif // PROPSOBJECT_H
