#include "basemoveobject.h"

BaseMoveObject::BaseMoveObject(int objectType,GMap* map)
    :BaseObject(objectType),Map(map)
{
}

void BaseMoveObject::SetVelocity(int setVelocity)
{
    //速度计算方法为
    //先将方格大小整除以速度，得到一个StepDevide，表明通过几次移动能到达下一格方格
    //再将方格大小除以stepDevide（实数除法），得到步幅
    //所以速度约等于步幅
    //由于Step为实数，所以移动过程中会积累误差，要定时消除

    Velocity=setVelocity;


    if(Velocity == 0){
        //如果速度为零，那么步幅为0，对象静止
        Step = 0;
    }
    else{
        //如果速度不为零，计算步幅
        int StepDevide=BRICK_SIZE/Velocity;
        Step=(qreal)BRICK_SIZE/(qreal)StepDevide;

        //保留两位小数
        Step=(qreal)((int)(Step*100))/(qreal)100;
    }
}

int BaseMoveObject::GetVelocity()
{
    return Velocity;
}

void BaseMoveObject::Move()
{
    bool PosChanged=false;  //判断位置是否移动的标记

    //如果没撞墙，则移动，改变实际位置
    if (!CollideWall())
    {
        setPos(QPointF(pos().x()+Dir_Y*Step,pos().y()+Dir_X*Step));
        PosChanged=true;

        //假设移动以后不在方块中央
        Ctrl=false;
    }
    //如果撞墙则方向指令置零
    else
        Dir_X=Dir_Y=0;

    //如果移动了位置
    if (PosChanged)
    {
        //通过实际位置重新计算逻辑位置
        X=((int)pos().y()) / BRICK_SIZE;
        Y=((int)pos().x()) / BRICK_SIZE;

        //如果走到了穿梭通道，则进行穿梭
        if (pos().y()<0)
        {
            X=MAPLENGTH-1;
            setPos(pos().x(),pos().y()+BRICK_SIZE * MAPLENGTH);
        }
        else if (pos().y()>=BRICK_SIZE*MAPLENGTH)
        {
            X=0;
            setPos(pos().x(),pos().y()-BRICK_SIZE * MAPLENGTH);
        }

        //如果走到了穿梭通道，则进行穿梭
        if (pos().x()<0)
        {
            Y=MAPLENGTH-1;
            setPos(pos().x()+BRICK_SIZE * MAPLENGTH,pos().y());
        }
        else if (pos().x()>=BRICK_SIZE*MAPLENGTH)
        {
            Y=0;
            setPos(pos().x()-BRICK_SIZE * MAPLENGTH,pos().y());
        }

        //如果实际位置离当前方块的中心小于半个步幅，则该物体实际已经在方块中央,需要消除误差
        if (
                qAbs(pos().y() - (BRICK_SIZE*X + HALF_BRICK_SIZE)) < Step - 0.05
                &&
                qAbs(pos().x() - (BRICK_SIZE*Y + HALF_BRICK_SIZE)) < Step - 0.05

                )
        {
            //将位置直接设在方块中央
            setPos( BRICK_SIZE * Y + HALF_BRICK_SIZE, BRICK_SIZE * X + HALF_BRICK_SIZE);

            //当前在方块中央
            Ctrl=true;
        }
    }
}

bool BaseMoveObject::CollideWall()
{
    //比较复杂的碰撞判定

    //当前移动方向为垂直方向
    if (Dir_X)
    {
        //计算碰撞点（物体前进方向最前端的点
        qreal XBoundaryNext = pos().y() + (OBJECT_SIZE/2 + Step) * Dir_X;
        //计算前方砖块的边界
        int XBrickNext=(int)(XBoundaryNext) / BRICK_SIZE;

        //进行撞墙判定(考虑Step的误差）
        if (
                XBrickNext>=0 && XBrickNext<MAPLENGTH
                &&
                (Map->WallData[XBrickNext][Y] || (ObjectType==_PLAYER && Map->RoadData[XBrickNext][Y] == -1))
                &&
                qAbs(XBoundaryNext - (XBrickNext*BRICK_SIZE + HALF_BRICK_SIZE - Dir_X*HALF_BRICK_SIZE)) > Step/2

                )
            return true;

        //如果当前类为Player，则对Player类启用自动转向功能
        if (ObjectType==_PLAYER && (int)pos().x()!= BRICK_SIZE * Y + HALF_BRICK_SIZE)
        {
            if((int)pos().x() - (BRICK_SIZE * Y + HALF_BRICK_SIZE) < 0) Dir_Y=1;
            else Dir_Y=-1;
            Dir_X=0;
        }
        return false;
    }

    //当前移动方向为水平方向
    if (Dir_Y)
    {
        //计算碰撞点（物体前进方向最前端的点
        qreal YBoundaryNext=pos().x()+(OBJECT_SIZE/2+Step)*Dir_Y;
        //计算前方砖块的边界
        int YBrickNext=(int)(YBoundaryNext)/BRICK_SIZE;

        //进行撞墙判定(考虑Step的误差）
        if(
                YBrickNext>=0 && YBrickNext<MAPLENGTH
                &&
                (Map->WallData[X][YBrickNext] || (ObjectType == _PLAYER && Map->RoadData[X][YBrickNext] == -1))
                &&
                qAbs(YBoundaryNext - (YBrickNext*BRICK_SIZE + HALF_BRICK_SIZE - Dir_Y*HALF_BRICK_SIZE)) > Step/2
                )
            return true;

        //如果当前类为Player，则对Player类启用自动转向功能
        if (ObjectType==_PLAYER && (int)pos().y()!= BRICK_SIZE * X + HALF_BRICK_SIZE)
        {
            if((int)pos().y() - (BRICK_SIZE * X + HALF_BRICK_SIZE) < 0) Dir_X=1;
            else Dir_X=-1;
            Dir_Y=0;
        }
        return false;
    }

    return false;
}
