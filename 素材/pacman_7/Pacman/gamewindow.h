#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QVector>
#include <QGraphicsView>

#include <QSoundEffect>
#include <QThread>

#include "mygamescene.h"
#include "config.h"
#include "map.h"
#include "playerclass.h"
#include "enemyclass.h"
#include "propsobject.h"


class SoundPlayWorker:public QObject
{
    Q_OBJECT

private:
    QSoundEffect *Sound;
public:
    SoundPlayWorker(const QString &file)
    {
        Sound=new QSoundEffect(this);
        Sound->setSource(QUrl::fromLocalFile(file));
        Sound->setLoopCount(0);
    }

    ~SoundPlayWorker()
    {
        Sound->stop();
    }
public slots:
    void Play(void)
    {
        Sound->play();
    }
    void setInfinity(void)
    {
        Sound->setLoopCount(QSoundEffect::Infinite);
    }

    void Stop(void)
    {
        Sound->stop();

    }

    void setVolume(qreal volume)
    {
        Sound->setVolume(volume);
    }
};

namespace Ui {
class GameWindow;
}


//GameWindow是游戏的游玩界面
class GameWindow : public QWidget
{
    Q_OBJECT
public:
    explicit GameWindow(int level, QWidget *parent = nullptr);//level为关卡参数
    ~GameWindow();

    //实现暂停和继续的切换功能的函数
    void PauseShift(void);

    //实现静音切换功能的函数
    void SoundShift(void);

signals:
    void GameSound(void);
    void GameSoundStop(void);
    void GameSoundMute(void);
    void GameSoundLoud(void);
    void closeWindow(void);
    void BeanSound(void);
    void CoughSound(void);
    void LoseSound(void);
    void WinSound(void);
    void DoublekillSound(void);
    void TriplekillSound(void);
    void DominatingSound(void);
    void PassLevel(void);

public slots:
    void MainGameFunc();
    void AddPropsObject();
    void InitializeGraphic();

protected:

    //重写键盘按下事件，实现用空格进行暂停
    void keyPressEvent(QKeyEvent*event) override;

    //重写关闭窗口事件
    void closeEvent(QCloseEvent *event) override;

    //重写事件过滤器
    bool eventFilter(QObject *watcher, QEvent *event) override;


private:
    Ui::GameWindow *ui;         //程序界面指针
    QGraphicsView *GameView;    //图形视角View类（用来对场景进行显示）
    MyGameScene *GameScene;     //自定义图形场景Scene类（需要依赖图形视角显示）

    QTimer *Timer;              //游戏主控时钟
    QTimer *PropsTimer;         //定时加入道具时钟
    QTimer *SlowTimer;          //减速时钟
    QTimer *SuperTimer;         //超级豆时间
    QTimer *ReadyTimer;             //准备开始游戏的时钟
    QTimer *LoseTimer;          //失败时使用的时钟

    GMap *map;                  //游戏地图
    EnemyClass *Enemy[3];       //敌人指针
    PlayerClass *Player;        //玩家指针

    QGraphicsTextItem* Readytext;   //准备开始的文本图形
    QThread SoundPlayThread;
    QThread GameSoundThread;
    QThread KillSoundThread;


    int Level;                      //当前关卡
    int BeansCount;                 //已经吃掉的豆子数量
    int Scores;                     //当前分数
    int KillGhost;                  //连续击杀幽灵数量
    bool Pause;                     //标记，表明是否暂停
    bool Sound;                     //标记，表明是否打开声音
    int GameStatus;                 //标记，表明游戏当前状态
    int ReadyTimeoutflag;           //标记，表明准备阶段的读秒
    int Blinktimes;                 //标记，表明失败时玩家闪烁次数
    bool CalculationFinished;       //标记，表明是否正在执行时钟信号引发的函数
    bool AutoClose;                 //标记，用于关闭窗口


};


#endif // GAMEWINDOW_H
