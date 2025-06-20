#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    this->setFixedSize(MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT);

    this->setWindowIcon(QIcon(":/Images/Pacman_icon.png"));

    this->setWindowTitle("Pacman");

    MainSound=new QSound(":/Sound/Sounds/Main_sound.wav",this);
    MainSound->setLoops(QSound::Infinite);
    MainSound->play();

    Background=new QLabel(this);
    Background->setGeometry(0,0,MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT);
    StarBackground=new QMovie(":/Images/StarBackground.gif",QByteArray(),this);
    Background->setMovie(StarBackground);
    StarBackground->start();
    Background->lower();

    ui->GameTitle->setPixmap(QPixmap(":/Images/Game_title.png").scaled(540,310,Qt::KeepAspectRatio));


    QMovie *Gif_1=new QMovie(this);
    Gif_1->setFileName(":/Images/Ghost1_left.gif");
    Gif_1->setScaledSize(QSize(80,80));
    ui->MainWindowGif1->setFixedSize(120,120);
    ui->MainWindowGif1->setMovie(Gif_1);
    Gif_1->start();

    QMovie *Gif_2=new QMovie(this);
    Gif_2->setFileName(":/Images/Ghost2_up.gif");
    Gif_2->setScaledSize(QSize(80,80));
    ui->MainWindowGif2->setFixedSize(120,120);
    ui->MainWindowGif2->setMovie(Gif_2);
    Gif_2->start();

    QMovie *Gif_3=new QMovie(this);
    Gif_3->setFileName(":/Images/Ghost3_right.gif");
    Gif_3->setScaledSize(QSize(80,80));
    ui->MainWindowGif3->setFixedSize(120,120);
    ui->MainWindowGif3->setMovie(Gif_3);
    Gif_3->start();

    ui->Begin_button->setFixedSize(200,50);
    ui->Begin_button->setStyleSheet("QPushButton{border-image: url(:/Images/Begin_text.png);}"
                                    "QPushButton:hover{border-image: url(:/Images/Begin_text_hover.png);}"
                                    "QPushButton:pressed{border-image: url(:/Images/Begin_text_press.png);}");

    ui->Rule_button->setFixedSize(200,50);
    ui->Rule_button->setStyleSheet("QPushButton{border-image: url(:/Images/Rules_text.png);}"
                                   "QPushButton:hover{border-image: url(:/Images/Rules_text_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/Images/Rules_text_press.png);}");

    ui->Exit_button->setFixedSize(200,50);
    ui->Exit_button->setStyleSheet("QPushButton{border-image: url(:/Images/Exit_text.png);}"
                                   "QPushButton:hover{border-image: url(:/Images/Exit_text_hover.png);}"
                                   "QPushButton:pressed{border-image: url(:/Images/Exit_text_press.png);}");


    Level_Page=new ChooseLevelScene(MainSound);
    ui->StackedWidget->addWidget(Level_Page);
    Rule_Page=new RuleWindow;
    ui->StackedWidget->addWidget(Rule_Page);    //插入新的widget    //Rule的所有权转移给StackedWidget
    //Rule->setFixedSize(MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT);    页面大小默认为StackedWidget大小，不需要设置



    QSound *ClickSound=new QSound(":/Sound/Sounds/TapButton.wav",this);

    connect(ui->Begin_button,&QPushButton::released,[=](){
        ClickSound->play();
        ui->StackedWidget->setCurrentWidget(Level_Page);
    });
    connect(Level_Page,&ChooseLevelScene::GameStart,this,&QWidget::hide);
    connect(Level_Page,&ChooseLevelScene::GameEnd,this,[=]()
    {
       this->show();
    });
    connect(Level_Page,&ChooseLevelScene::closeWindow,[=]()
    {
        ui->StackedWidget->setCurrentWidget(ui->Main_Page);
    });


    connect(ui->Rule_button,&QPushButton::released,[=](){
        ClickSound->play();
        ui->StackedWidget->setCurrentWidget(Rule_Page);
    });
    connect(Rule_Page,&RuleWindow::closeWindow,[=]()
    {
       ui->StackedWidget->setCurrentWidget(ui->Main_Page);
    });

    connect(ui->Exit_button,&QPushButton::released,[=]()
    {
        ClickSound->play();
        QTimer::singleShot(200,this,&QWidget::close);
    });    
}

MainWindow::~MainWindow()
{
    MainSound->stop();
    delete ui;
    delete Level_Page;
}

