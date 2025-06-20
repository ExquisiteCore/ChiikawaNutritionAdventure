#include "playerclass.h"

extern const int DIR_VALUE[4][2];

PlayerClass::PlayerClass(GMap* map):BaseMoveObject(_PLAYER,map)
{
    //将四个方向的图片加载到图片缓存数组中，并且将动图切换帧的信号与改变玩家类显示图像的操作进行关联
    /*Buffer_Images[DIR_LEFT]=new QMovie(":/Images/Pacman_left.gif");
    Buffer_Images[DIR_LEFT]->setScaledSize(QSize(PLAYER_SIZE,PLAYER_SIZE));
    QObject::connect(Buffer_Images[DIR_LEFT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_LEFT]->currentPixmap());
    });*/

    Buffer_Images=new QMovie(":/Images/Pacman_right.gif");
    Buffer_Images->setScaledSize(QSize(PLAYER_SIZE,PLAYER_SIZE));
    QObject::connect(Buffer_Images,&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images->currentPixmap());
    });

    /*Buffer_Images[DIR_UP]=new QMovie(":/Images/Pacman_up.gif");
    Buffer_Images[DIR_UP]->setScaledSize(QSize(PLAYER_SIZE,PLAYER_SIZE));
    QObject::connect(Buffer_Images[DIR_UP],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_UP]->currentPixmap());
    });

    Buffer_Images[DIR_DOWN]=new QMovie(":/Images/Pacman_down.gif");
    Buffer_Images[DIR_DOWN]->setScaledSize(QSize(PLAYER_SIZE,PLAYER_SIZE));
    QObject::connect(Buffer_Images[DIR_DOWN],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_DOWN]->currentPixmap());
    });*/

    //死亡动画指针置空
    DieImage=nullptr;

    //初始时非超级豆时间
    IsSuperTime = false;

    //将玩家类设为可聚焦(focus)，这样才能接收键盘信号
    setFlags(QGraphicsItem::ItemIsFocusable);

    //将玩家类的绘制原点进行偏移，偏移量为半个图像的大小，这样玩家类的实际位置刚好处于图像中央
    setOffset(-PLAYER_SIZE/2,-PLAYER_SIZE/2);

    //设置玩家类的初始位置
    setPos(P_COL*BRICK_SIZE+HALF_BRICK_SIZE,P_ROW*BRICK_SIZE+HALF_BRICK_SIZE);

    //设置玩家类初始速度
    SetVelocity(INIT_PLAYER_VELOCITY);

    //设置玩家的初始逻辑位置
    X=P_ROW; Y=P_COL;

    //将玩家类的移动指令和缓存移动指令置零
    Dir_X=Dir_Y=Buffer_Dir_X=Buffer_Dir_Y=0;

    //初始方向为右边
    Dir=Buffer_Dir=DIR_RIGHT;

    //最开始在方格中心，将方格中心标志设为真
    Ctrl=true;

    //将四个键盘按下标记清零
    for(int i=0;i<4;++i) ButtenPressed[i]=false;

    //初始键盘按下个数为零
    PressedKeys=0;

    //加载右方向的图片
    Image=Buffer_Images;
    Image->start();
}

void PlayerClass::Move()
{
    //如果缓存移动指令与当前移动指令在同一条直线上，或者已经到达了方块中心
    if ( (Buffer_Dir_X==Dir_X || Buffer_Dir_Y==Dir_Y ) || Ctrl)
    {
        //将缓存指令写入当前移动指令中
        Dir_X=Buffer_Dir_X;
        Dir_Y=Buffer_Dir_Y;

        //如果缓存移动指令的方向与当前方向不同，说明产生了变向
        if (Dir!=Buffer_Dir)
        {
            //改变动图的方向
            //Image->stop();
            //Image=Buffer_Images[Buffer_Dir];
            //Image->start();
            setRotation((Buffer_Dir+2)%4*90);
            Dir=Buffer_Dir;
        }
    }
    //调用父类的移动函数
    BaseMoveObject::Move();
}

void PlayerClass::keyPressEvent(QKeyEvent *event)
{
    //如果按下指令是由第一次按下产生的（而非长按导致的重复按压与释放指令）
    if (!event->isAutoRepeat())
    {
        //将方向指令输入缓存移动指令中，按压键盘数加一，并记录按压的键盘
        switch(event->key())
        {
        case Qt::Key_A:
        {
            Buffer_Dir_X=DIR_VALUE[DIR_LEFT][0]; Buffer_Dir_Y=DIR_VALUE[DIR_LEFT][1]; Buffer_Dir=DIR_LEFT; ButtenPressed[DIR_LEFT]=true; ++PressedKeys;
            break;
        }
        case Qt::Key_D:
        {
            Buffer_Dir_X=DIR_VALUE[DIR_RIGHT][0]; Buffer_Dir_Y=DIR_VALUE[DIR_RIGHT][1]; Buffer_Dir=DIR_RIGHT; ButtenPressed[DIR_RIGHT]=true; ++PressedKeys;
            break;
        }
        case Qt::Key_W:
        {
            Buffer_Dir_X=DIR_VALUE[DIR_UP][0]; Buffer_Dir_Y=DIR_VALUE[DIR_UP][1]; Buffer_Dir=DIR_UP; ButtenPressed[DIR_UP]=true; ++PressedKeys;
            break;
        }
        case Qt::Key_S:
        {
            Buffer_Dir_X=DIR_VALUE[DIR_DOWN][0]; Buffer_Dir_Y=DIR_VALUE[DIR_DOWN][1]; Buffer_Dir=DIR_DOWN; ButtenPressed[DIR_DOWN]=true; ++PressedKeys;
            break;
        }
        default:
            QGraphicsPixmapItem::keyPressEvent(event);
            break;
        };
    }

    //调用父类的键盘按压事件函数


}

void PlayerClass::keyReleaseEvent(QKeyEvent*event)
{
    //如果释放指令是由第一次释放产生的（而非长按导致的重复按压与释放指令）
    //减少键盘按压个数并将对应键盘按压标记清零
    if(!event->isAutoRepeat())
        switch(event->key())
        {
        case Qt::Key_A:
            ButtenPressed[DIR_LEFT]=false; --PressedKeys;
            break;
        case Qt::Key_D:
            ButtenPressed[DIR_RIGHT]=false; --PressedKeys;
            break;
        case Qt::Key_W:
            ButtenPressed[DIR_UP]=false; --PressedKeys;
            break;
        case Qt::Key_S:
            ButtenPressed[DIR_DOWN]=false; --PressedKeys;
            break;
        default:
            //调用父类的键盘释放事件函数
            QGraphicsPixmapItem::keyReleaseEvent(event);
        }

    if (PressedKeys<0) PressedKeys=0;
    //如果按压键盘数为零则缓存移动指令清零
    if(!PressedKeys) Buffer_Dir_X=Buffer_Dir_Y=0;
    //否则说明当前还有键盘在按压
    else
    {
        //将搜索到的第一个仍在按压的键盘对应的指令输入缓存移动指令中
        for(int i=0;i<4;++i)
            if (ButtenPressed[i])
            {
                Buffer_Dir_X=DIR_VALUE[i][0];
                Buffer_Dir_Y=DIR_VALUE[i][1];
                Buffer_Dir=i;
                break;
            }
    }


}

PlayerClass::~PlayerClass()
{
    Image->stop();
    Image=nullptr;
    delete Buffer_Images;
    Buffer_Images=nullptr;
    if(DieImage!=nullptr) delete DieImage;
}

//暂停动态图
void PlayerClass::Pause(void)
{
   Image->setPaused(true);
}

//恢复动态图
void PlayerClass::Resume(void)
{
    Image->setPaused(false);
}

void PlayerClass::Die(void)
{
    Image->stop();
    setRotation(0);

    setOffset(-PLAYER_SIZE,-PLAYER_SIZE);
    DieImage=new QMovie(":/Images/Pacman_die.gif");
    DieImage->setSpeed(130);
    QObject::connect(DieImage,&QMovie::frameChanged,[=]()
    {
        setPixmap(DieImage->currentPixmap());
    });

    QObject::connect(DieImage,&QMovie::finished,[=]()
    {
        DieImage->stop();
    });

    DieImage->start();

}

void PlayerClass::CleanKeyPress(void)
{
    Dir_X=Dir_Y=Buffer_Dir_X=Buffer_Dir_Y=0;
    for(int i=0;i<4;++i) ButtenPressed[i]=false;
    PressedKeys=0;
}

void PlayerClass::focusOutEvent(QFocusEvent* event)
{
    setFocus();
}
