#ifndef SUGAR_OIL_GAME_SCENE_H
#define SUGAR_OIL_GAME_SCENE_H

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
#include <QMediaPlayer>
#include <QSoundEffect>
#include <QRandomGenerator>
#include <QList>
#include <QElapsedTimer>
#include "sugar_oil_config.h"
#include "usagi_player.h"
#include "sugar_oil_enemy.h"
#include "game_item.h"
#include "creature.h"

class SugarOilGameScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit SugarOilGameScene(QObject *parent = nullptr);
    ~SugarOilGameScene();
    
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
    SugarOilGameState getCurrentState() const { return currentState; }
    bool isGameRunning() const { return currentState == SUGAR_OIL_RUNNING; }
    
    // 游戏数据访问
    int getRemainingTime() const { return remainingTime; }
    int getPlayerLives() const;
    int getPlayerScore() const;
    
    // 音频控制
    void playBackgroundMusic();
    void stopBackgroundMusic();
    void playSound(const QString& soundName);
    void setMusicVolume(float volume);
    void setSoundVolume(float volume);
    
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    
signals:
    void gameWon();
    void gameLost();
    void gameStateChanged(SugarOilGameState newState);
    void timeChanged(int remainingSeconds);
    void livesChanged(int lives);
    void scoreChanged(int score);
    void itemCollected(ItemType itemType);
    void creatureEncountered(CreatureType creatureType);
    
private slots:
    void updateGame();
    void updateTimer();
    void spawnEnemy();
    void spawnItem();
    void spawnCreature();
    void onPlayerDamaged();
    void onPlayerHealed();
    void onItemPickedUp(ItemType itemType);
    void onCreatureMet(CreatureType creatureType);
    void checkCollisions();
    void cleanupObjects();
    
private:
    void initializeGame();
    void setupTimers();
    void setupAudio();
    void spawnInitialObjects();
    void updateGameObjects();
    void checkWinCondition();
    void checkLoseCondition();
    void applyItemEffect(ItemType itemType);
    void applyCreatureEffect(CreatureType creatureType);
    void removeOffscreenObjects();
    QPointF getRandomSpawnPosition();
    bool isPositionSafe(const QPointF& position);
    
    // 游戏状态
    SugarOilGameState currentState;
    int remainingTime;
    bool gameInitialized;
    
    // 游戏对象
    UsagiPlayer* player;
    QList<SugarOilEnemy*> enemies;
    QList<GameItem*> items;
    QList<Creature*> creatures;
    
    // 定时器
    QTimer* gameTimer;          // 主游戏循环
    QTimer* countdownTimer;     // 倒计时
    QTimer* enemySpawnTimer;    // 敌人生成
    QTimer* itemSpawnTimer;     // 道具生成
    QTimer* creatureSpawnTimer; // 生物生成
    
    // 音频
    QMediaPlayer* backgroundMusicPlayer;
    QSoundEffect* soundEffect;
    
    // 游戏计数器
    int enemiesSpawned;
    int itemsSpawned;
    int creaturesSpawned;
    
    // 性能计时
    QElapsedTimer performanceTimer;
    
    // 背景图片
    QPixmap backgroundPixmap;
};

#endif // SUGAR_OIL_GAME_SCENE_H