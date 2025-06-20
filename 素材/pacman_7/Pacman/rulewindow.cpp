#include "rulewindow.h"
#include "ui_rulewindow.h"

RuleWindow::RuleWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RuleWindow)
{
    ui->setupUi(this);

    //this->setFixedSize(RULE_WINDOW_WIDTH,RULE_WINDOW_HEIGHT);

    //this->setWindowIcon(QIcon(":/Images/Pacman_icon.png"));

    //this->setWindowTitle("Pacman");

    // 设置回退按钮及音效
    ui->Back_Button->setStyleSheet("QPushButton{border-image: url(:/Images/Back_button.png);}"
                                  "QPushButton:hover{border-image: url(:/Images/Back_button_hover.png);}"
                                  "QPushButton:pressed{border-image: url(:/Images/Back_button_press.png);}");
    QSoundEffect *ButtonSound = new QSoundEffect(this);
    ButtonSound->setSource(QUrl::fromLocalFile(":/Sound/Sounds/TapButton.wav"));
    connect(ui->Back_Button,&QPushButton::released,[=]()
    {
        //播放返回按钮音效
        ButtonSound->play();
        emit closeWindow();
    });
}

RuleWindow::~RuleWindow()
{
    delete ui;
}

void RuleWindow::paintEvent(QPaintEvent *event)
{
    QPainter Painter(this);
    Painter.setBrush(QColor(0,0,0,200));
    Painter.drawRect(0,0,MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT);
    Painter.drawPixmap(0,0,MAIN_WINDOW_WIDTH,MAIN_WINDOW_HEIGHT,QPixmap(":/Images/Rule.png"));
}
