#include "chooselevelscene.h"
#include "ui_chooselevelscene.h"

ChooseLevelScene::ChooseLevelScene(QSound* mainsound,QWidget *parent) :
    QWidget(parent),MainSound(mainsound),
    ui(new Ui::ChooseLevelScene)
{
    ui->setupUi(this);

    //setFixedSize(LEVEL_WINDOW_WIDTH,LEVEL_WINDOW_HEIGHT);

    //setWindowIcon(QIcon(":/Images/Pacman_icon.png"));

    //setWindowTitle("Pacman");


    /*Background=new QLabel(this);
    Background->setGeometry(0,0,1250,1000);
    StarBackground=new QMovie(":/Images/StarBackground.gif",QByteArray(),this);
    Background->setMovie(StarBackground);
    StarBackground->start();
    Background->lower();*/

    ui->Line_1->setPixmap(QPixmap(":/Images/Line_1.png"));
    ui->Line_2->setPixmap(QPixmap(":/Images/Line_2.png"));
    ui->Line_3->setPixmap(QPixmap(":/Images/Line_3.png"));
    // 设置回退按钮及音效
    ui->BackButton->setStyleSheet("QPushButton{border-image: url(:/Images/Back_button.png);}"
                                  "QPushButton:hover{border-image: url(:/Images/Back_button_hover.png);}"
                                  "QPushButton:pressed{border-image: url(:/Images/Back_button_press.png);}");
    QSoundEffect *ButtonSound = new QSoundEffect(this);
    ButtonSound->setSource(QUrl::fromLocalFile(":/Sound/Sounds/TapButton.wav"));
    connect(ui->BackButton,&QPushButton::released,[=]()
    {
        //播放返回按钮音效
        ButtonSound->play();
        emit closeWindow();
    });

    LevelBtn[0]=ui->LevelButton_1;
    LevelBtn[1]=ui->LevelButton_2;
    LevelBtn[2]=ui->LevelButton_3;
    LevelBtn[3]=ui->LevelButton_4;

    for(int i = 0; i<4; i++)
    {

        connect(LevelBtn[i],&QPushButton::released,[=]()
        {
            ButtonSound->play();
            if (passedLevelNum[i]){
                Game_w=new GameWindow(i+1);     //注意关卡数要加1
                //Game_w->move(x()+(LEVEL_WINDOW_WIDTH-GAME_WINDOW_WIDTH)/2,y()+(LEVEL_WINDOW_HEIGHT-GAME_WINDOW_HEIGHT)/2);
                connect(this,&ChooseLevelScene::GameStart,Game_w,&GameWindow::InitializeGraphic);
                Game_w->show();
                emit GameStart();
                MainSound->stop();
                connect(Game_w,&GameWindow::PassLevel,this,&ChooseLevelScene::ChangeButton);
                connect(Game_w,&GameWindow::closeWindow,[=](){
                    ButtonSound->play();
                    emit GameEnd();
                    delete Game_w;
                    MainSound->play();
                });
            }
        });
    }

    ChangeButton();
}

ChooseLevelScene::~ChooseLevelScene()
{
    delete ui;
}

void ChooseLevelScene::ChangeButton(void)
{
    for(int i=0;i<4;++i)
    {
        LevelBtn[i]->setEnabled(passedLevelNum[i]);
        if(passedLevelNum[i])
            LevelBtn[i]->setStyleSheet("QPushButton{border-image: url(:/Images/Level_"+QString::number(i+1)+"_button.png);}"
                                       "QPushButton:hover{border-image: url(:/Images/Level_"+QString::number(i+1)+"_button_hovered.png);}"
                                       "QPushButton:pressed{border-image: url(:/Images/Level_"+QString::number(i+1)+"_button_pressed.png);}"
                                       "QPushButton{border:0px;}");
        else
            LevelBtn[i]->setStyleSheet("QPushButton{border-image: url(:/Images/Level_"+QString::number(i+1)+"_button_locked.png);}");
    }
}

void ChooseLevelScene::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit closeWindow();
}
