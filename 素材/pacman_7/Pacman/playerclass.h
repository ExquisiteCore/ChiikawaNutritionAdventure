#ifndef PLAYERCLASS_H
#define PLAYERCLASS_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMovie>
#include <QTimer>

#include "config.h"
#include "basemoveobject.h"

class PlayerClass:public BaseMoveObject
{
public:
    PlayerClass(GMap* map);     //map为地图
    ~PlayerClass();

    //重写移动函数
    void Move() override;

    //暂停函数，让图像静止
    void Pause(void);

     //继续函数，让图像恢复动态
    void Resume(void);

    //窗口失去焦点时清空键盘指令
    void CleanKeyPress(void);

    //失败时显示死亡动画
    void Die(void);

    int Buffer_Dir_X,Buffer_Dir_Y,Buffer_Dir;  //玩家类特有的指令缓存

    //标记对象类型
    enum { Type = _PLAYER };
    int type() const override { return Type; }

    bool IsSuperTime;                           //是否是超级豆时间

protected:
    void keyPressEvent(QKeyEvent* event) override;      //重写键盘按下事件
    void keyReleaseEvent(QKeyEvent*event) override;     //重写键盘释放事件
    void focusOutEvent(QFocusEvent* event)override;



private:
    int PressedKeys;               //记录键盘按下的个数
    bool ButtenPressed[4];         //标记哪些键盘被按下
    QMovie* Buffer_Images;      //缓存四个方向的动态图
    QMovie* DieImage;               //死亡动画


};

#endif // PLAYERCLASS_H
