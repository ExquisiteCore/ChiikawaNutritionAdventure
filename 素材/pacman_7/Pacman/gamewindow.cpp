#include "gamewindow.h"
#include "ui_gamewindow.h"
#include <QProgressBar>


GameWindow::GameWindow(int level, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameWindow)
{
    //初始化窗体界面
    ui->setupUi(this);

    //设置关闭窗口时直接销毁窗口对象
    setAttribute(Qt::WA_DeleteOnClose);

    //设置窗口图标和标题
    this->setWindowIcon(QIcon(":/Images/Pacman_icon.png"));
    this->setWindowTitle("Pacman");

    //为窗口添加事件过滤器,检测窗口失焦和窗口得焦
    installEventFilter(this);

    //调整窗体界面
    this->setFixedSize(GAME_WINDOW_WIDTH,GAME_WINDOW_HEIGHT);

    //调整游戏信息界面（右边部分)的颜色和焦点策略
    ui->InfoPanel->setFocusPolicy(Qt::NoFocus);     //设为不可聚焦，防止鼠标点击后角色失去键盘控制
    setStyleSheet("#InfoPanel{background:black}");  //将信息界面的背景设为黑色



    //随机种子初始化
    qsrand(time(NULL));

    //将暂停按钮和回退按钮和音量按钮设为不可聚焦，防止鼠标点击后角色失去键盘控制
    ui->PauseButton->setFocusPolicy(Qt::NoFocus);
    ui->BackButton->setFocusPolicy(Qt::NoFocus);
    ui->SoundButton->setFocusPolicy(Qt::NoFocus);

    //分数（Scores）标题文本框和数值文本框的文字颜色设为白色
    ui->ScoreTitle->setStyleSheet("color:white");
    ui->ScoreValue->setStyleSheet("color:white");

    //豆子统计标题文本框和豆子数值文本框的文字颜色为白色
    ui->BeansTitle->setStyleSheet("color:white");
    ui->BeansValue->setStyleSheet("color:white");

    //超级豆和减速标题文字设为白色,并且初始不可见
    ui->SuperBeanTitle->setStyleSheet("color:white");
    ui->SuperBeanTitle->setVisible(false);
    ui->SlowDownTitle->setStyleSheet("color:white");
    ui->SlowDownTitle->setVisible(false);

    //初始不显示进度条
    ui->slowProgressBar->setVisible(false);
    ui->superProgressBar->setVisible(false);

    //设置暂停按钮的大小
    ui->PauseButton->setFixedSize(36,36);
    //设置回退按钮的大小
    ui->BackButton->setFixedSize(80,36);
    //设置音量按钮的大小
    ui->SoundButton->setFixedSize(44,36);

    //暂停按钮加载按钮图片（普通、 鼠标悬停、鼠标按下）
    ui->PauseButton->setStyleSheet("QPushButton{border-image: url(:/Images/Pause_button.png);}"
                                   "QPushButton:hover{border-image: url(:/Images/Pause_button_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/Images/Pause_button_press.png);}");

    //回退按钮加载按钮图片（普通、 鼠标悬停、鼠标按下）
    ui->BackButton->setStyleSheet("QPushButton{border-image: url(:/Images/Back_button.png);}"
                                  "QPushButton:hover{border-image: url(:/Images/Back_button_hover.png);}"
                                  "QPushButton:pressed{border-image: url(:/Images/Back_button_press.png);}");

    //音量按钮加载按钮图片（普通、 鼠标悬停、鼠标按下）
    ui->SoundButton->setStyleSheet("QPushButton{border-image: url(:/Images/Sound_on.png);}"
                                   "QPushButton:hover{border-image: url(:/Images/Sound_on_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/Images/Sound_on_press.png);}");
    //加载音效
    SoundPlayWorker* GameSoundWorker=new SoundPlayWorker(":/Sound/Sounds/Game_sound.wav");
    GameSoundWorker->setInfinity();
    GameSoundWorker->setVolume(0.5);
    SoundPlayWorker* BeanSoundWorker=new SoundPlayWorker(":/Sound/Sounds/Bean_sound.wav");
    BeanSoundWorker->setVolume(0.6);
    SoundPlayWorker* CoughSoundWorker=new SoundPlayWorker(":/Sound/Sounds/Cough_sound.wav");
    SoundPlayWorker* LoseSoundWorker=new SoundPlayWorker(":/Sound/Sounds/Lose.wav");
    SoundPlayWorker* WinSoundWorker=new SoundPlayWorker(":/Sound/Sounds/Win.wav");
    SoundPlayWorker* DoubleKillWorker=new SoundPlayWorker(":/Sound/Sounds/Double_Kill.wav");
    SoundPlayWorker* TripleKillWorker=new SoundPlayWorker(":/Sound/Sounds/Triple_Kill.wav");
    SoundPlayWorker* DominatingWorker=new SoundPlayWorker(":/Sound/Sounds/Dominating.wav");

    //将音效移入其它线程中
    GameSoundWorker->moveToThread(&GameSoundThread);
    BeanSoundWorker->moveToThread(&SoundPlayThread);
    CoughSoundWorker->moveToThread(&SoundPlayThread);
    LoseSoundWorker->moveToThread(&SoundPlayThread);
    WinSoundWorker->moveToThread(&SoundPlayThread);
    DoubleKillWorker->moveToThread(&KillSoundThread);
    TripleKillWorker->moveToThread(&KillSoundThread);
    DominatingWorker->moveToThread(&KillSoundThread);
    //与其他线程建立联系
    connect(this,&GameWindow::GameSound,GameSoundWorker,&SoundPlayWorker::Play);
    connect(this,&GameWindow::GameSoundStop,GameSoundWorker,&SoundPlayWorker::Stop);
    connect(this,&GameWindow::GameSoundMute,GameSoundWorker,[=](){
        GameSoundWorker->setVolume(0);
    });
    connect(this,&GameWindow::GameSoundLoud,GameSoundWorker,[=](){
        GameSoundWorker->setVolume(0.5);
    });
    connect(&GameSoundThread,&QThread::finished,GameSoundWorker,&QObject::deleteLater);
    connect(this,&GameWindow::BeanSound,BeanSoundWorker,&SoundPlayWorker::Play);
    connect(&SoundPlayThread,&QThread::finished,BeanSoundWorker,&QObject::deleteLater);
    connect(this,&GameWindow::CoughSound,CoughSoundWorker,&SoundPlayWorker::Play);
    connect(&SoundPlayThread,&QThread::finished,CoughSoundWorker,&QObject::deleteLater);
    connect(this,&GameWindow::LoseSound,LoseSoundWorker,&SoundPlayWorker::Play);
    connect(&SoundPlayThread,&QThread::finished,LoseSoundWorker,&QObject::deleteLater);
    connect(this,&GameWindow::WinSound,WinSoundWorker,&SoundPlayWorker::Play);
    connect(&SoundPlayThread,&QThread::finished,WinSoundWorker,&QObject::deleteLater);
    connect(this,&GameWindow::DoublekillSound,DoubleKillWorker,&SoundPlayWorker::Play);
    connect(&KillSoundThread,&QThread::finished,DoubleKillWorker,&QObject::deleteLater);
    connect(this,&GameWindow::TriplekillSound,TripleKillWorker,&SoundPlayWorker::Play);
    connect(&KillSoundThread,&QThread::finished,TripleKillWorker,&QObject::deleteLater);
    connect(this,&GameWindow::DominatingSound,DominatingWorker,&SoundPlayWorker::Play);
    connect(&KillSoundThread,&QThread::finished,DominatingWorker,&QObject::deleteLater);
    SoundPlayThread.start();
    GameSoundThread.start();
    KillSoundThread.start();

    emit GameSound();

    //记录当前关卡
    Level=level;

    //根据构造函数传入的关卡信息决定加载哪一关地图
    switch(Level)
    {
    case 1:map=new Stage_1(); break;
    case 2:map=new Stage_2(); break;
    case 3:map=new Stage_3(); break;
    }


    //////////初始化标记//////////
    //将已吃豆子数和分数置零
    BeansCount=map->BeansNum;
    Scores=0;

    //准备时间标记和闪烁标记置零
    ReadyTimeoutflag=0;
    Blinktimes=0;

    //一开始没有执行任何运算
    CalculationFinished=true;

    //关闭自动关闭标志
    AutoClose=false;

    //游戏标志打开，暂停标志关闭,声音打开
    GameStatus=GAME_READY;
    Pause=false;
    Sound=true;

    KillGhost=0;

    /////////////////////////////


    //显示置零后的游戏数据
    QString BeansCountText;
    BeansCountText=QString::number(BeansCount)+"/"+QString::number(map->BeansNum);
    ui->BeansValue->setText(BeansCountText);
    QString ScoresText;
    ScoresText=QString::number(Scores);
    ui->ScoreValue->setText(ScoresText);

    //创建自定义Scene类
    GameScene=new MyGameScene(map,level,this);

    //创建View类
    GameView=new QGraphicsView(GameScene,ui->GameWidget);
    //取消View的滚动条
    GameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    GameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //取消View的边界
    GameView->setStyleSheet("border: 0px");
    //将View类设为聚焦控件（键盘事件才能通过View-Scene—Item的顺序传递到玩家类中，实现对玩家类的操控）
    GameView->setFocus();


    //////////////////时钟/////////////////
    //主控时钟初始化
    Timer=new QTimer(this);
    //添加道具时钟
    PropsTimer=new QTimer(this);
    //减速时钟
    SlowTimer=new QTimer(this);
    //超级豆时钟
    SuperTimer=new QTimer(this);
    //准备开始游戏时钟
    ReadyTimer=new QTimer(this);


    //////////////////////////////加入游戏对象///////////////////////////////////////
    //在Scene中加入豆子
    for(int i=0;i<MAPLENGTH;++i)
        for(int j=0;j<MAPLENGTH;++j)
            if(map->BeanData[i][j])
            {
                GameScene->addEllipse(0,0,BEAN_SIZE,BEAN_SIZE,QPen(Qt::white),QBrush(Qt::white))
                        ->setPos( j*BRICK_SIZE+HALF_BRICK_SIZE-BEAN_SIZE/2,i*BRICK_SIZE+HALF_BRICK_SIZE-BEAN_SIZE/2);
            }


    //新建玩家类
    Player=new PlayerClass(map);

    //玩家类加入Scene中
    GameScene->addItem(Player);
    //将玩家类设为焦点（这样才能接收View和Scene中传递来的键盘事件）
    Player->setFocus();

    //调整玩家类的层级
    Player->setZValue(2);
    //新建敌人类
    Enemy[0]=new Enemy1(map,Player);
    Enemy[1]=new Enemy2(map,Player);
    Enemy[2]=new Enemy3(map,Player);

    //将敌人类加入Scene中
    GameScene->addItem(Enemy[0]);
    GameScene->addItem(Enemy[1]);
    GameScene->addItem(Enemy[2]);
    //敌人在最上层
    for(int i = 0; i < ENEMY_NUMBERS; i++)
        Enemy[i]->setZValue(3);


    //设置准备开始的文本
    Readytext=new QGraphicsTextItem();
    Readytext->setHtml("<center>""Ready!""<center>");
    Readytext->setDefaultTextColor(Qt::white);
    Readytext->setFont(QFont("Comic Sans MS",18));
    Readytext->setTextWidth(3*BRICK_SIZE);
    Readytext->setPos(8*BRICK_SIZE,7*BRICK_SIZE);
    GameScene->addItem(Readytext);

    //////////////////////////////////【核心】通过主控时钟引导的游戏主进程/////////////////////////////////////////
    connect(Timer,&QTimer::timeout,this,&GameWindow::MainGameFunc);


    ////////////////////////////////每隔一段时间，随机加入道具到非墙的位置/////////////////////////////
    connect(PropsTimer, &QTimer::timeout,this,&GameWindow::AddPropsObject);

    /////////////////调整减速/////////////
    connect(SlowTimer, &QTimer::timeout, [=](){

        ui->slowProgressBar->setValue(--Player->degree_of_deceleration);

        if(Player->degree_of_deceleration == 60)
            Player->SetVelocity(INIT_PLAYER_VELOCITY-2>=0?INIT_PLAYER_VELOCITY-2:0);
        else if(Player->degree_of_deceleration == 30)
            Player->SetVelocity(INIT_PLAYER_VELOCITY-1>=0?INIT_PLAYER_VELOCITY-1:0);
        else if(Player->degree_of_deceleration == 0){
            if(!Player->IsSuperTime)Player->SetVelocity(INIT_PLAYER_VELOCITY);
            else Player->SetVelocity(INIT_PLAYER_VELOCITY+2);
            SlowTimer->stop();
            ui->SlowDownTitle->setVisible(false);
            ui->slowProgressBar->setVisible(false);
        }

    });
    ///////////////调整超级时间////////////
    connect(SuperTimer, &QTimer::timeout, [=](){

        ui->superProgressBar->setValue(--Enemy[0]->degree_of_deceleration);
        if (Enemy[0]->degree_of_deceleration == 100-SUPER_BEAN_FREEZE/SUPER_BEAN_INTERVAL)
        {
            for(int i = 0; i < 3; i++)
                if (!Enemy[i]->HasEaten) Enemy[i]->SetVelocity(INIT_ENEMY_VELOCITY-3>=0?INIT_ENEMY_VELOCITY-3:0);
        }

        else if(Enemy[0]->degree_of_deceleration == 50)
        {
            for(int i = 0; i < 3; i++)
                if (!Enemy[i]->HasEaten) Enemy[i]->SetVelocity(INIT_ENEMY_VELOCITY-2>=0?INIT_ENEMY_VELOCITY-2:0);
        }

        else if(Enemy[0]->degree_of_deceleration == 0){
            for(int i = 0; i < 3; i++)
                if (!Enemy[i]->HasEaten)
                {
                    Enemy[i]->SetVelocity(INIT_ENEMY_VELOCITY);
                    Enemy[i]->FrightenShift();
                }
            SuperTimer->stop();
            ui->SuperBeanTitle->setVisible(false);
            ui->superProgressBar->setVisible(false);
            Player->IsSuperTime = false;
            Player->SetVelocity(INIT_PLAYER_VELOCITY);
            KillGhost=0;
        }
    });

    connect(ui->PauseButton,&QPushButton::released,[=]()
    {
        //按下暂停按钮调用暂停-继续切换函数
        PauseShift();
    });

    connect(ui->SoundButton,&QPushButton::released,[=]()
    {
        //按下音量按钮调用静音切换函数
        SoundShift();
    });

    connect(ui->BackButton,&QPushButton::released,[=]()
    {
        close();
    });


    ///////////////////////////游戏准备阶段//////////////////////////////////

    //倒计时结束后开始游戏，改变准备开始的文本并打开主控时钟
    connect(ReadyTimer,&QTimer::timeout,[=](){

        //设置开始计算标记
        CalculationFinished=false;

        switch (ReadyTimeoutflag)
        {
        case 0:
            ReadyTimer->stop();
            ReadyTimer->setInterval(1000);
            ReadyTimer->start();
            Readytext->setHtml("<center>""3""<center>");
            break;
        case 1:
            Readytext->setHtml("<center>""2""<center>");
            break;
        case 2:
            Readytext->setHtml("<center>""1""<center>");
            break;
        case 3:
            //开始游戏，启动主控时钟，改变游戏状态
            Readytext->setHtml("<center>""Go!""<center>");
            Timer->start(GAME_TIMER_SPEED);
            PropsTimer->start(PROPS_ADD_INTERVAL);
            GameStatus=GAME_RUN;
            break;
        case 4:
            GameScene->removeItem(Readytext);
            ReadyTimer->stop();
            break;
        }

        ++ReadyTimeoutflag;

        //设置结束计算标记
        CalculationFinished=true;
    });

    ReadyTimer->start(2000);

    //connect(this,&QWidget::show,this,&GameWindow::InitializeGraphic);
    //show();
    //qDebug()<<ui->GameWidget->size();
    //GameView->resize(ui->GameWidget->size());
}

GameWindow::~GameWindow()
{
    GameSoundThread.quit();
    GameSoundThread.wait();
    SoundPlayThread.quit();
    SoundPlayThread.wait();
    KillSoundThread.quit();
    KillSoundThread.wait();

    delete ui;

    delete Readytext;
    Readytext=nullptr;

    delete Player;
    Player=nullptr;
    for(int i=0;i<3;++i)
    {
        delete Enemy[i];
        Enemy[i]=nullptr;
    }

    delete map;

}


void GameWindow::InitializeGraphic(void)
{
    //将Scene和View的大小进行设置，不能在构造函数中进行，否则无法读取正确的GameWidget大小
    GameView->resize(ui->GameWidget->size());

}

void GameWindow::keyPressEvent(QKeyEvent*event)
{
    //如果按下空格，则调用暂停-继续切换函数
    if (event->key()==Qt::Key_Space)
    {
        PauseShift();
    }
    else return QWidget::keyPressEvent(event);
}


//暂停-继续切换函数
void GameWindow::PauseShift(void)
{
    GameView->setFocus();

    //如果之前暂停则继续，之前继续则暂停
    if (!Pause)
    {
        //游戏还在准备，则停止准备时钟
        if(GameStatus==GAME_READY) ReadyTimer->stop();

        //游戏还没结束，则停止主控时钟
        if(GameStatus==GAME_RUN) Timer->stop();

        PropsTimer->stop();
        if(SlowTimer->isActive()) SlowTimer->stop();
        if(SuperTimer->isActive()) SuperTimer->stop();

        //将玩家和敌人静止
        Player->Pause();
        for(int i=0;i<3;++i)Enemy[i]->Pause();

        //将暂停按钮变为继续按钮
        ui->PauseButton->setStyleSheet("QPushButton{border-image: url(:/Images/Resume_button.png);}"
                                       "QPushButton:hover{border-image: url(:/Images/Resume_button_hover.png);}"
                                       "QPushButton:pressed{border-image: url(:/Images/Resume_button_press.png);}");
        //设为暂停
        Pause=true;
    }
    else
    {
        //游戏还在准备，则打开准备时钟
        if(GameStatus==GAME_READY) ReadyTimer->start();

        //游戏还没结束，则打开主控时钟
        if(GameStatus==GAME_RUN) Timer->start(GAME_TIMER_SPEED);

        if(GameStatus!=GAME_END)
        {
            PropsTimer->start(PROPS_ADD_INTERVAL);
            if(Player->degree_of_deceleration) SlowTimer->start(SLOW_DOWN_INTERVAL);
            if(Enemy[0]->degree_of_deceleration) SuperTimer->start(SLOW_DOWN_INTERVAL);

            //将玩家和敌人重新变为动态
            Player->Resume();
            for(int i=0;i<3;++i)Enemy[i]->Resume();
        }

        //将继续按钮变为暂停按钮
        ui->PauseButton->setStyleSheet("QPushButton{border-image: url(:/Images/Pause_button.png);}"
                                       "QPushButton:hover{border-image: url(:/Images/Pause_button_hover.png);}"
                                       "QPushButton:pressed{border-image: url(:/Images/Pause_button_press.png);}");

        //让View重新获得焦点，理由同构造函数中所述
        GameView->setFocus();


        //设为继续
        Pause=false;
    }
}

void GameWindow::SoundShift(void)
{
    if (Sound) {
        emit GameSoundMute();
        Sound=false;
        ui->SoundButton->setStyleSheet("QPushButton{border-image: url(:/Images/Sound_off.png);}"
                                       "QPushButton:hover{border-image: url(:/Images/Sound_off_hover.png);}"
                                       "QPushButton:pressed{border-image: url(:/Images/Sound_off_press.png);}");

    }
    else{
        emit GameSoundLoud();
        Sound=true;
        ui->SoundButton->setStyleSheet("QPushButton{border-image: url(:/Images/Sound_on.png);}"
                                       "QPushButton:hover{border-image: url(:/Images/Sound_on_hover.png);}"
                                       "QPushButton:pressed{border-image: url(:/Images/Sound_on_press.png);}");

    }
}

void GameWindow::closeEvent(QCloseEvent *event)
{
    if(ReadyTimer->isActive()) ReadyTimer->stop();
    if(Timer->isActive())Timer->stop();
    if (PropsTimer->isActive()) PropsTimer->stop();
    if(SlowTimer->isActive()) SlowTimer->stop();
    if(SuperTimer->isActive()) SuperTimer->stop();

    if(!CalculationFinished)
    {
        //如果还没结束运算

        //不响应关闭事件
        event->ignore();

        //打开自动关闭标志
        AutoClose=true;
    }
    else
    {
        event->accept();
        emit closeWindow();
    }
}


void GameWindow::MainGameFunc()
{


    //设置开始运算标记
    CalculationFinished=false;

    //如果玩家类失去了焦点，则重新聚焦，防止失去键盘控制
    //if(!Player->hasFocus()) Player->setFocus();

    //如果玩家类存在移动指令，或缓存移动指令，则进行移动
    if(Player->Dir_X+Player->Dir_Y!=0 || Player->Buffer_Dir_X+Player->Buffer_Dir_Y!=0) Player->Move();

    //分别移动三个敌人类
    for(int i=0;i<ENEMY_NUMBERS;++i) Enemy[i]->Move();

    //将玩家类碰撞到的图形存入collideList中
    QList<QGraphicsItem *> collideList=Player->collidingItems();

    //如果List非空，表明玩家类碰到了图形，对图形进行判断
    if(!collideList.isEmpty())
    {
        //创建List的迭代器
        QList<QGraphicsItem *>::iterator Object;

        //对List迭代，取出所有碰撞图形进行判定
        for(Object=collideList.begin();Object!=collideList.end();++Object)
        {
            ///////////////////是敌人////////////////////
            if((*Object)->type() == _ENEMY)
            {
                //强制类型转换，以控制自己创建的类
                EnemyClass * tmp = qgraphicsitem_cast<EnemyClass *> (*Object);

                if(tmp->Frightened)
                {
                    //防止重复碰撞正在回退的怪物
                    if (!tmp->BackHome)
                    {
                        //加分
                        ui->ScoreValue->setText(QString::number(Scores+=ENEMY_SCORE));

                        //回到出生点
                        tmp->BackHome=true;

                        //标记被吃过
                        tmp->HasEaten=true;

                        //加快回退速度
                        tmp->SetVelocity(10);

                        //连续击杀数加一
                        ++KillGhost;

                        if(Sound)
                        switch (KillGhost) {
                        case 1:break;
                        case 2:emit DoublekillSound(); break;
                        case 3:emit TriplekillSound(); break;
                        }
                    }

                    /*
                    //创建用于对怪物进行闪烁的时钟
                    QTimer* BeatTimer=new QTimer(this);
                    //设置时钟闪烁间隔
                    BeatTimer->setInterval(500);
                    //对怪物闪烁6次
                    connect(BeatTimer,&QTimer::timeout,[=]()
                    {
                        static int Blinktimes_=0;
                        ++Blinktimes_;
                        if (Blinktimes_%2) tmp->setVisible(false);
                        else tmp->setVisible(true);
                        if (Blinktimes_==6) {
                            BeatTimer->stop();
                            tmp->setVisible(true);
                            Blinktimes_=0;
                        }
                    });
                    BeatTimer->start();
                    */
                }
                else
                {   //如果玩家和敌人的距离小于判定碰撞距离（可以比实际碰撞距离更小，使碰撞判定更加自然)
                    //游戏失败
                    if(qAbs(Player->pos().x()-(*Object)->pos().x())+qAbs(Player->pos().y()-(*Object)->y())<= CRASH_DISTANCE)
                    {

                        //主控时钟停止
                        Timer->stop();
                        PropsTimer->stop();
                        if(SlowTimer->isActive()) SlowTimer->stop();
                        if(SuperTimer->isActive()) SuperTimer->stop();

                        //游戏音乐结束
                        emit GameSoundStop();

                        //创建用于对玩家进行闪烁的时钟
                        LoseTimer=new QTimer(this);

                        //设置时钟闪烁间隔
                        LoseTimer->setInterval(300);

                        //对玩家闪烁六次
                        connect(LoseTimer,&QTimer::timeout,[=]()
                        {
                            ++Blinktimes;
                            if (Blinktimes%2) Player->setVisible(false);
                            else Player->setVisible(true);
                            if (Blinktimes==6) {

                                LoseTimer->stop();

                                for(int i=0;i<3;++i) GameScene->removeItem(Enemy[i]);

                                //播放失败音效
                                if (Sound) emit LoseSound();

                                ui->BeansTitle->setVisible(false);
                                //显示游戏失败
                                ui->BeansValue->setText("YOU LOSE!");

                                //显示玩家死亡动画
                                Player->Die();


                            }
                        });
                        LoseTimer->start();
                        //游戏结束
                        GameStatus=GAME_END;
                    }

                }

            }
            ///////////////////是病毒////////////////////
            else if((*Object)->type() == _SLOWVIRUS)
            {
                SlowDownPacman * tmp = qgraphicsitem_cast<SlowDownPacman *>(*Object);
                //已经处于减速
                if(SlowTimer->isActive())
                    SlowTimer->stop();
                SlowTimer->start(SLOW_DOWN_INTERVAL);
                //减速
                Player->SetVelocity(INIT_PLAYER_VELOCITY-3>=0?INIT_PLAYER_VELOCITY-3:0);
                Player->degree_of_deceleration = 100;

                //显示进度条
                ui->SlowDownTitle->setVisible(true);
                ui->slowProgressBar->setVisible(true);
                ui->slowProgressBar->setValue(Player->degree_of_deceleration);

                //移除
                GameScene->removeItem(*Object);
                delete tmp;

                map->BeanData[tmp->X][tmp->Y] = -1;

                //播放咳嗽声
                if (Sound) emit CoughSound();
            }
            ///////////////////是超级豆////////////////////
            else if((*Object)->type() == _SUPERBEAN)
            {
                //发出音效
                if(Sound && !Player->IsSuperTime) emit DominatingSound();

                //强制类型转换，以控制自己创建的类
                SuperBean * tmp = qgraphicsitem_cast<SuperBean *>(*Object);

                //玩家加速
                Player->SetVelocity(INIT_PLAYER_VELOCITY+2);

                Player->IsSuperTime = true;
                if(SuperTimer->isActive())
                    SuperTimer->stop();
                SuperTimer->start(SUPER_BEAN_INTERVAL);
                //减速
                for(int i = 0; i < ENEMY_NUMBERS; ++i)
                {
                    if (!Enemy[i]->BackHome)
                    {
                        Enemy[i]->SetVelocity(0);
                        Enemy[i]->HasEaten=false;
                    }
                    if (!Enemy[i]->Frightened) Enemy[i]->FrightenShift();
                }
                Enemy[0]->degree_of_deceleration = 100; //用一个敌人做代表，标记减速进行的程度

                //显示进度条
                ui->SuperBeanTitle->setVisible(true);
                ui->superProgressBar->setVisible(true);
                ui->superProgressBar->setValue(Enemy[0]->degree_of_deceleration);

                //加分移除
                ui->ScoreValue->setText(QString::number(Scores+=SUPER_BEAN_SCORE));

                //增加已吃豆子数并显示
                QString BeansCountText;
                BeansCountText=QString::number(--BeansCount)+"/"+QString::number(map->BeansNum);
                ui->BeansValue->setText(BeansCountText);

                //更改BeanDate标记
                map->BeanData[tmp->X][tmp->Y] = 0;

                GameScene->removeItem(*Object);
                delete tmp;
            }
            //////////////////是普通豆子////////////////////
            else if((*Object)->type() == _BEAN)
            {
                //发出音效
                if (Sound) emit BeanSound();

                //增加分数并显示
                ui->ScoreValue->setText(QString::number(Scores+=BEAN_SCORE));

                //增加已吃豆子数并显示
                QString BeansCountText;
                BeansCountText=QString::number(--BeansCount)+"/"+QString::number(map->BeansNum);
                ui->BeansValue->setText(BeansCountText);

                //更改BeanDate标记
                map->BeanData[(int)(*Object)->scenePos().y() / BRICK_SIZE][(int)(*Object)->scenePos().x() / BRICK_SIZE] = 0;

                //移除豆子
                GameScene->removeItem(*Object);


            }
        }
    }

    if (BeansCount==0 && Timer->isActive())
    {
        //如果吃完了豆子，且没有在吃最后一颗豆子的时候先碰到敌人而提前结束了主控时钟
        //游戏胜利

        //停止主控时钟
        Timer->stop();
        PropsTimer->stop();
        if(SlowTimer->isActive()) SlowTimer->stop();
        if(SuperTimer->isActive()) SuperTimer->stop();

        //游戏音乐结束
        emit GameSoundStop();

        //显示游戏胜利
        ui->BeansTitle->setVisible(false);
        ui->BeansValue->setText("YOU WIN!");
        GameStatus=GAME_END;

        if (Sound) emit WinSound();

        //解锁关卡
        passedLevelNum[Level%4]=true;

        emit PassLevel();
    }

    //运算结束
    CalculationFinished=true;

    //如果开启了自动关闭，那么就自动关闭窗口
    if (AutoClose) close();
}

//生成道具
void GameWindow::AddPropsObject()
{
    int whichOne = qrand() % 10;
    int x = qrand() % MAPLENGTH;
    int y = qrand() % MAPLENGTH;

    if(whichOne > map->PR_Bean){ //概率出现病毒 出现在空地
        int Count=0;
        while(map->BeanData[x][y] || map->RoadData[x][y]<=0 || (x==Player->X && y==Player->Y)){
            x=(x+(++y)/MAPLENGTH)%MAPLENGTH;
            y%=MAPLENGTH;
            if (++Count>MAPLENGTH*MAPLENGTH) break;
        }
        if (Count<=MAPLENGTH*MAPLENGTH)
        {
            QList<QGraphicsItem *> item_list_p = GameScene->items(QRectF(y*BRICK_SIZE,x*BRICK_SIZE,BRICK_SIZE,BRICK_SIZE), Qt::IntersectsItemShape);
            SlowDownPacman * tmp = new SlowDownPacman(x, y);
            GameScene->addItem(tmp);
            //道具在倒数第二下层
            map->BeanData[x][y]=-1;
            tmp->setZValue(1);
        }
    }
    else{ //概率出现超级豆 出现在豆子上
        int Count=0;
        while(map->BeanData[x][y]<=0 || (x==Player->X && y==Player->Y)){
            x=(x+(++y)/MAPLENGTH)%MAPLENGTH;
            y%=MAPLENGTH;
            if (++Count>MAPLENGTH*MAPLENGTH) break;
        }
        if (Count<=MAPLENGTH*MAPLENGTH)
        {
            QList<QGraphicsItem *> item_list_p = GameScene->items(QRectF(y*BRICK_SIZE,x*BRICK_SIZE,BRICK_SIZE,BRICK_SIZE), Qt::IntersectsItemShape);

            if(!item_list_p.empty()){
                //超级豆，会把出现处的除了玩家和幽灵都清除
                if(item_list_p[0]->type() != _ENEMY && item_list_p[0]->type() != _PLAYER)
                    GameScene->removeItem(item_list_p[0]);
            }

            SuperBean * tmp = new SuperBean(x, y);
            GameScene->addItem(tmp);
            //道具在倒数第二下层
            tmp->setZValue(1);
        }
    }

}

bool GameWindow::eventFilter(QObject *watcher, QEvent *event)
{
    if (watcher==this)
    {
        if (event->type()==QEvent::WindowDeactivate)
        {
            Player->CleanKeyPress();
            if (!Pause) PauseShift();
            return false;
        }
        else if (event->type()==QEvent::WindowActivate)
        {
            return false;
        }
        else if (event->type()==QEvent::Move)
        {
            Player->CleanKeyPress();
            return false;
        }
        else
            return false;
    }
    else return QWidget::eventFilter(watcher,event);
}
