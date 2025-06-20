#include "enemyclass.h"

extern const int DIR_VALUE[4][2];

EnemyClass::EnemyClass(GMap *map,const PlayerClass* player,int x, int y, int objecttype):BaseMoveObject(objecttype,map),Player(player),initX(x), initY(y)
{
    //设为不可聚焦

    //初始化敌人速度
    SetVelocity(INIT_ENEMY_VELOCITY);

    //初始时在方块中央
    Ctrl=true;

    //随机种子初始化
    qsrand(time(NULL));

    //初始化警戒范围
    AlertDist=ALERT_DISTANCE;

    //初始化随机步长
    LastSteps=0;

    //加载惊吓图片
    Frighten_Image=new QMovie(":/Images/Ghost_frightened.gif");
    Frighten_Image->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));

    QObject::connect(Frighten_Image,&QMovie::frameChanged,[=]()
    {
        setPixmap(Frighten_Image->currentPixmap());
    });

    //清空惊吓标记
    Frightened=false;

    //清空回退标记
    BackHome=false;

    //清空被吃标记
    HasEaten=false;
}
EnemyClass::~EnemyClass()
{
    Frighten_Image->stop();
    delete Frighten_Image;
}


void EnemyClass::Move()
{
    //标记是否开启追踪
    bool tracing=false;

    //一个表示改变方向的量
    int chg=0;

    if (BackHome && X== initX && Y==initY)
    {
        //结束回退
        BackHome=false;

        //速度变为正常
        SetVelocity(INIT_ENEMY_VELOCITY);

        FrightenShift();
    }

    //如果当前在方格中心，才能考虑变向
    if(Ctrl)
    {

        int X_Dist=qAbs(Player->getX()-X);
        int Y_Dist=qAbs(Player->getY()-Y);

        if (
                (((
                    flag_trace_mode ==_WEAK_TRACING //弱追踪表示只在同一行或同一列且处于警戒范围内时才追踪
                    &&
                    (
                        ((X_Dist<=AlertDist || MAPLENGTH-X_Dist<=AlertDist) && Player->getY()==Y)
                        ||
                        ((Y_Dist<=AlertDist || MAPLENGTH-Y_Dist<=AlertDist) && Player->getX()==X)
                        )
                    )
                ||
                (
                    flag_trace_mode == _STRONG_TRACING //强追踪表示只要行与列均处于警戒范围内时就可以追踪
                    &&
                    (X_Dist<=AlertDist || MAPLENGTH-X_Dist <=AlertDist)
                    &&
                    (Y_Dist<=AlertDist || MAPLENGTH-Y_Dist<=AlertDist)
                    )
                 )
                &&
                Map->RoadData[X][Y]!=-1)
                ||
                Frightened || BackHome
                )
            tracing=trace();    //通过追踪函数计算最短距离，判断是否开启追踪


        //如果不追踪则对方向进行随机
        if(!tracing || CollideWall())
        {
            //每移动一步都要减少随机步长
            --LastSteps;

            //如果撞墙则随机步长清零
            if (CollideWall()) LastSteps=0;

            //判断当前方向的垂直方向是否可以移动，如果可以则表示到达了分叉口，随机步长清零
            int tempDir_X=Dir_X,tempDir_Y=Dir_Y;
            Dir_X=DIR_VALUE[(Dir+1)%4][0],Dir_Y=DIR_VALUE[(Dir+1)%4][1];
            if (!CollideWall()) LastSteps=0;
            Dir_X=DIR_VALUE[(Dir+3)%4][0],Dir_Y=DIR_VALUE[(Dir+3)%4][1];
            if (!CollideWall()) LastSteps=0;
            Dir_X=tempDir_X; Dir_Y=tempDir_Y;

            //如果随机步长为零，表明需要转向了
            if(LastSteps<=0)
                do
            {
                //随机选择一个方向直到该方向可以移动，并通过数学方法将后退的概率降低
                chg=((qrand()%16+4)/5+2)%4;
                Dir_X=DIR_VALUE[(Dir+chg)%4][0];
                Dir_Y=DIR_VALUE[(Dir+chg)%4][1];
            }while(CollideWall());

            //将当前方向变为随机出来的新方向
            Dir=(Dir+chg)%4;

            if(Map->RoadData[X][Y]==-1)
            {
                Dir=DIR_UP;
                Dir_X=DIR_VALUE[Dir][0];
                Dir_Y=DIR_VALUE[Dir][1];
                if(CollideWall()) Dir_X=Dir_Y=0;

            }
        }   
    }
    BaseMoveObject::Move();
    if (LastSteps<=0) LastSteps=qrand()%10+10;

    //如果方向改变了，并且不在惊吓状态，则改变动态图
    if (LastDir!=Dir && !Frightened)
    {
        Image->stop();
        Image=Buffer_Images[Dir];
        Image->start();
        LastDir=Dir;
    }

}

//运用广度优先搜索的追踪函数
bool EnemyClass::trace()
{
    bool mapflag[MAPLENGTH][MAPLENGTH];
    int queue[MAPLENGTH*MAPLENGTH][2];
    int dir[MAPLENGTH*MAPLENGTH],pos[MAPLENGTH*MAPLENGTH];
    int p=0,pe=0,steps=0;
    int targetX,targetY;

    if (!BackHome){
        targetX=Player->getX();
        targetY=Player->getY();
    }
    else
    {
        targetX=initX;
        targetY=initY;
    }

    for(int i=0;i<MAPLENGTH;++i)
        for(int j=0;j<MAPLENGTH;++j)
            mapflag[i][j]=Map->RoadData[i][j];

    queue[p][0]=X; queue[p][1]=Y; mapflag[X][Y]=0; pos[p]=-1;

    while(p<=pe)
    {
        for(int i=0;i<4;++i)
            if (mapflag[(queue[p][0]+DIR_VALUE[i][0]+MAPLENGTH)%MAPLENGTH][(queue[p][1]+DIR_VALUE[i][1]+MAPLENGTH)%MAPLENGTH])
            {
                ++pe;
                queue[pe][0]=(queue[p][0]+DIR_VALUE[i][0]+MAPLENGTH)%MAPLENGTH;
                queue[pe][1]=(queue[p][1]+DIR_VALUE[i][1]+MAPLENGTH)%MAPLENGTH;
                mapflag[(queue[p][0]+DIR_VALUE[i][0]+MAPLENGTH)%MAPLENGTH][(queue[p][1]+DIR_VALUE[i][1]+MAPLENGTH)%MAPLENGTH]=false;
                dir[pe]=i;
                pos[pe]=p;
                if (queue[pe][0]==targetX && queue[pe][1]==targetY) break;
            }
        if (queue[pe][0]==targetX && queue[pe][1]==targetY) break;

        ++p;
    }

    while(pos[pe]!=0)
    {
        pe=pos[pe]; ++steps;
    }

    if(!Frightened)
    {
        //如果敌人到玩家的距离大于警戒距离的平方除以一半，则路线过于曲折，放弃追踪，否则开启追踪
        if (steps<=AlertDist*AlertDist/2)
        {
            Dir=dir[pe];
            Dir_X=DIR_VALUE[Dir][0];
            Dir_Y=DIR_VALUE[Dir][1];
            LastSteps=0;
            return true;
        }
        else return false;
    }
    else if (!BackHome)
    {
        LastSteps=0;

        //如果是惊吓状态，则尽量从最短路反方向逃离
        Dir=(dir[pe]+2)%4;
        Dir_X=DIR_VALUE[Dir][0];
        Dir_Y=DIR_VALUE[Dir][1];

        if (CollideWall())
            for(int i=1;i<=4;++i)
            {
                if (i==2) continue;
                Dir=(dir[pe]+i)%4;
                Dir_X=DIR_VALUE[Dir][0];
                Dir_Y=DIR_VALUE[Dir][1];
                if (!CollideWall()) break;
            }
    }
    else
    {
        Dir=dir[pe];
        Dir_X=DIR_VALUE[Dir][0];
        Dir_Y=DIR_VALUE[Dir][1];
        LastSteps=0;
    }

    return true;
}

bool EnemyClass::CollideWall(void)
{
    return (Map->WallData[(X+Dir_X+MAPLENGTH)%MAPLENGTH][(Y+Dir_Y+MAPLENGTH)%MAPLENGTH]
            ||
            (!BackHome && Map->RoadData[X][Y] == 1 && Map->RoadData[(X+Dir_X+MAPLENGTH)%MAPLENGTH][(Y+Dir_Y+MAPLENGTH)%MAPLENGTH]==-1)
            ||
            (Player->IsSuperTime && Map->RoadData[X][Y] == -1 && Map->RoadData[(X+Dir_X+MAPLENGTH)%MAPLENGTH][(Y+Dir_Y+MAPLENGTH)%MAPLENGTH]==1));
}

void EnemyClass::Pause(void)
{
    Image->setPaused(true);
}
void EnemyClass::Resume(void)
{
    Image->setPaused(false);
}

void EnemyClass::FrightenShift(void)
{
    if (!Frightened)
    {
        Image->stop();
        Image=Frighten_Image;
        Image->start();
        Frightened=true;
    }
    else{
        Image->stop();
        Image=Buffer_Images[Dir];
        Image->start();
        Frightened=false;
    }
}

//-------------------------Enemy1------------------------------
Enemy1::Enemy1(GMap *map,const PlayerClass* player):EnemyClass(map,player,ENEMY1_ROW,ENEMY1_COL)
{

    //将四个方向的图片加载到图片缓存数组中，并且将动图切换帧的信号与改变玩家类显示图像的操作进行关联
    Buffer_Images[DIR_LEFT]=new QMovie(":/Images/Ghost1_left.gif");
    Buffer_Images[DIR_LEFT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_LEFT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_LEFT]->currentPixmap());
    });

    Buffer_Images[DIR_UP]=new QMovie(":/Images/Ghost1_up.gif");
    Buffer_Images[DIR_UP]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_UP],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_UP]->currentPixmap());
    });

    Buffer_Images[DIR_RIGHT]=new QMovie(":/Images/Ghost1_right.gif");
    Buffer_Images[DIR_RIGHT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_RIGHT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_RIGHT]->currentPixmap());
    });

    Buffer_Images[DIR_DOWN]=new QMovie(":/Images/Ghost1_down.gif");
    Buffer_Images[DIR_DOWN]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_DOWN],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_DOWN]->currentPixmap());
    });


    //敌人1选择左方向的图片为出示图片
    Image=Buffer_Images[DIR_LEFT];
    Image->start();

    //将敌人类的绘制原点进行偏移，偏移量为半个图像的大小，这样敌人类的实际位置刚好处于图像中央
    setOffset(-ENEMY_SIZE/2,-ENEMY_SIZE/2);
    QObject::connect(Buffer_Images[DIR_LEFT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_LEFT]->currentPixmap());
    });

    //设置敌人类的初始逻辑位置
    X=ENEMY1_ROW; Y=ENEMY1_COL;
    //将敌人类移动到初始实际位置
    setPos(ENEMY1_COL*BRICK_SIZE+HALF_BRICK_SIZE,ENEMY1_ROW*BRICK_SIZE+HALF_BRICK_SIZE);

    //初始移动方向设为0
    Dir_X=Dir_Y=0; Dir=LastDir=0;

    //不开启追踪
    flag_trace_mode=_NO_TRACING;
}
Enemy1::~Enemy1()
{
    Image->stop();
    Image=nullptr;
    for(int i=0;i<4;++i)
    {
        delete Buffer_Images[i];
        Buffer_Images[i]=nullptr;
    }
}

//-------------------------Enemy2------------------------------
Enemy2::Enemy2(GMap *map,const PlayerClass* player):EnemyClass(map,player,ENEMY2_ROW,ENEMY2_COL)
{
    Buffer_Images[DIR_LEFT]=new QMovie(":/Images/Ghost2_left.gif");
    Buffer_Images[DIR_LEFT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_LEFT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_LEFT]->currentPixmap());
    });

    Buffer_Images[DIR_UP]=new QMovie(":/Images/Ghost2_up.gif");
    Buffer_Images[DIR_UP]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_UP],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_UP]->currentPixmap());
    });

    Buffer_Images[DIR_RIGHT]=new QMovie(":/Images/Ghost2_right.gif");
    Buffer_Images[DIR_RIGHT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_RIGHT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_RIGHT]->currentPixmap());
    });

    Buffer_Images[DIR_DOWN]=new QMovie(":/Images/Ghost2_down.gif");
    Buffer_Images[DIR_DOWN]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_DOWN],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_DOWN]->currentPixmap());
    });

    Image=Buffer_Images[DIR_UP];
    Image->start();
    setOffset(-ENEMY_SIZE/2,-ENEMY_SIZE/2);
    QObject::connect(Image,&QMovie::frameChanged,[=](){
        setPixmap(Image->currentPixmap());
    });

    X=ENEMY2_ROW; Y=ENEMY2_COL;
    setPos(ENEMY2_COL*BRICK_SIZE+HALF_BRICK_SIZE,ENEMY2_ROW*BRICK_SIZE+HALF_BRICK_SIZE);
    Dir_X=Dir_Y=0; Dir=LastDir=0;

    //开启弱追踪
    flag_trace_mode=_WEAK_TRACING;
}
Enemy2::~Enemy2()
{
    Image->stop();
    Image=nullptr;
    for(int i=0;i<4;++i)
    {
        delete Buffer_Images[i];
        Buffer_Images[i]=nullptr;
    }
}

//-------------------------Enemy3------------------------------
Enemy3::Enemy3(GMap *map,const PlayerClass* player):EnemyClass(map,player,ENEMY3_ROW,ENEMY3_COL)
{
    Buffer_Images[DIR_LEFT]=new QMovie(":/Images/Ghost3_left.gif");
    Buffer_Images[DIR_LEFT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_LEFT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_LEFT]->currentPixmap());
    });

    Buffer_Images[DIR_UP]=new QMovie(":/Images/Ghost3_up.gif");
    Buffer_Images[DIR_UP]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_UP],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_UP]->currentPixmap());
    });

    Buffer_Images[DIR_RIGHT]=new QMovie(":/Images/Ghost3_right.gif");
    Buffer_Images[DIR_RIGHT]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_RIGHT],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_RIGHT]->currentPixmap());
    });

    Buffer_Images[DIR_DOWN]=new QMovie(":/Images/Ghost3_down.gif");
    Buffer_Images[DIR_DOWN]->setScaledSize(QSize(ENEMY_SIZE,ENEMY_SIZE));
    QObject::connect(Buffer_Images[DIR_DOWN],&QMovie::frameChanged,[=](){
        setPixmap(Buffer_Images[DIR_DOWN]->currentPixmap());
    });

    Image=Buffer_Images[DIR_RIGHT];
    Image->start();
    setOffset(-ENEMY_SIZE/2,-ENEMY_SIZE/2);
    QObject::connect(Image,&QMovie::frameChanged,[=](){
        setPixmap(Image->currentPixmap());
    });


    X=ENEMY3_ROW; Y=ENEMY3_COL;
    setPos(ENEMY3_COL*BRICK_SIZE+HALF_BRICK_SIZE,ENEMY3_ROW*BRICK_SIZE+HALF_BRICK_SIZE);
    Dir_X=Dir_Y=0; Dir=LastDir=0;

    //开启强追踪
    flag_trace_mode=_STRONG_TRACING;
}

Enemy3::~Enemy3()
{
    Image->stop();
    Image=nullptr;
    for(int i=0;i<4;++i)
    {
        delete Buffer_Images[i];
        Buffer_Images[i]=nullptr;
    }
}
