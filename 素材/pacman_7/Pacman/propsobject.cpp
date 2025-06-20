#include "propsobject.h"


PropsObject::PropsObject(short objectType): BaseObject(objectType)
{

}

SlowDownPacman::SlowDownPacman(int x, int y) : PropsObject(_SLOWVIRUS)
{
    //将道具类的绘制原点进行偏移，偏移量为半个图像的大小，这样道具类的实际位置刚好处于图像中央
    setOffset(-PROP_SIZE/2,-PROP_SIZE/2);

    //绘制道具
    setPixmap(QPixmap(":/Images/Virus.png").scaled(PROP_SIZE, PROP_SIZE));

    X = x; Y = y;
    //设置道具类的初始位置
    setPos(Y*BRICK_SIZE+HALF_BRICK_SIZE,X*BRICK_SIZE+HALF_BRICK_SIZE);
}

SuperBean::SuperBean(int x, int y) : PropsObject(_SUPERBEAN)
{


    //将道具类的绘制原点进行偏移，偏移量为半个图像的大小，这样道具类的实际位置刚好处于图像中央
    setOffset(-PROP_SIZE/2,-PROP_SIZE/2);

    //绘制道具
    setPixmap(QPixmap(":/Images/Super_bean.png").scaled(PROP_SIZE+6, PROP_SIZE+6));

    X = x; Y = y;
    //设置道具类的初始位置
    setPos(Y*BRICK_SIZE+HALF_BRICK_SIZE,X*BRICK_SIZE+HALF_BRICK_SIZE);
}
