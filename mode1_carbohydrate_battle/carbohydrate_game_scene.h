#ifndef CARBOHYDRATE_GAME_SCENE_H
#define CARBOHYDRATE_GAME_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include "carbohydrate_config.h"
#include "game_map.h"
#include "player.h"
#include "fake_vegetable_boss.h"
#include "fiber_sword.h"

class CarbohydrateGameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit CarbohydrateGameScene(QObject *parent = nullptr);
    ~CarbohydrateGameScene();
    
    // 游戏控制
    void startGame();
    void pauseGame();
    void resumeGame();
    void endGame(bool won);
    void resetGame();
    
    // 键盘事件处理
    void handleKeyPress(QKeyEvent *event);
    void handleKeyRelease(QKeyEvent *event);
    
    // 游戏状态
    GameState getCurrentState() const { return currentState; }
    bool isGameRunning() const { return currentState == GAME_RUNNING; }
    
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    
signals:
    void gameWon();
    void gameLost();
    void gameStateChanged(GameState newState);
    
private slots:
    void updateGame();
    void onFiberSwordUsed(QPointF position, Direction direction);
    void onFiberSwordHit(QGraphicsItem* target);
    void onFiberSwordDestroyed();
    void onBossDefeated();
    void onPlayerCaught();
    void onFiberValueChanged(int newValue);
    void onBossHealthChanged(int newHealth);
    void onPauseButtonClicked();
    void onResumeButtonClicked();
    
private:
    void initializeGame();
    void createUI();
    void updateUI();
    void drawMap();
    void drawFakeVegetables();
    void checkCollisions();
    void cleanupFiberSwords();
    
    // 游戏对象
    GameMap* gameMap;
    Player* player;
    FakeVegetableBoss* boss;
    QList<FiberSword*> fiberSwords;
    
    // 游戏状态
    GameState currentState;
    QTimer* gameTimer;
    
    // UI元素
    QLabel* fiberValueLabel;
    QProgressBar* bossHealthBar;
    QLabel* gameStatusLabel;
    QPushButton* pauseButton;
    QPushButton* resumeButton;
    QGraphicsProxyWidget* uiWidget;
    
    // 视觉元素
    QList<QGraphicsPixmapItem*> wallItems;
    QList<QGraphicsPixmapItem*> fakeVegetableItems;
    
    // 背景
    QPixmap backgroundPixmap;
};

class CarbohydrateGameView : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit CarbohydrateGameView(CarbohydrateGameScene* scene, QWidget *parent = nullptr);
    
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    
private:
    CarbohydrateGameScene* gameScene;
};

#endif // CARBOHYDRATE_GAME_SCENE_H