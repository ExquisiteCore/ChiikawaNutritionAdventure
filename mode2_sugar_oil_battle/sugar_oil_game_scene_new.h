#ifndef SUGAR_OIL_GAME_SCENE_NEW_H
#define SUGAR_OIL_GAME_SCENE_NEW_H

#include <QGraphicsScene>
#include <QTimer>
#include <QList>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>

#include "sugar_oil_config.h"
#include "../audio_manager.h"
#include "sugar_oil_player.h"
#include "enemy_base.h"
#include "bullet_base.h"
#include "item_system.h"
#include "creature_system.h"

class SugarOilGameSceneNew : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit SugarOilGameSceneNew(QObject *parent = nullptr);
    virtual ~SugarOilGameSceneNew();

    // 游戏控制
    void startGame();
    void pauseGame();
    void resumeGame();
    void stopGame();
    void resetGame();
    
    // 游戏状态
    bool isGameRunning() const { return mGameRunning; }
    bool isGamePaused() const { return mGamePaused; }
    
    // 获取游戏数据
    int getGameTime() const { return mGameTime; }
    int getScore() const;
    int getPlayerLevel() const;
    
    // 键盘输入处理
    void handleKeyPress(int key);
    void handleKeyRelease(int key);
    
    // 鼠标输入处理
    void handleMousePress(const QPointF &scenePos);
    void handleMouseMove(const QPointF &scenePos);
    void handleMouseRelease(const QPointF &scenePos);
    
    // 获取玩家引用
    SugarOilPlayer* getPlayer() const { return mPlayer; }

signals:
    void gameStarted();
    void gamePaused();
    void gameResumed();
    void gameOver(int finalScore, int finalLevel);
    void gameWon(int finalScore, int finalLevel);
    void gameStateChanged(SugarOilGameState newState);
    void scoreChanged(int score);
    void timeChanged(int timeLeft);
    void playerStatsChanged(int hp, int maxHp, int level, int exp);

public slots:
    void onGameTimerTimeout();
    void onPlayerShoot(QPointF position, QPointF direction, int damage);
    void onEnemyAttack(QPointF position, QPointF direction, int damage);
    void onEnemyDied(EnemyBase* enemy);
    void onPlayerDied();
    void onPlayerLevelUp(int newLevel);
    void onBulletOutOfBounds(BulletBase* bullet);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    
private slots:
    void updateGame();
    void spawnEnemies();
    void updateCollisions();
    void updatePlayerMovement();
    void cleanupObjects();
    void updateItems();
    void updateCreatures();
    
private:
    // 初始化方法
    void initializeScene();
    void initializePlayer();
    void initializeTimers();
    void initializeAudio();
    void initializeManagers();
    void loadBackground();
    void drawMapBoundaries();
    
    // 游戏逻辑
    void spawnEnemy(EnemyBase::EnemyType type, const QPointF &position);
    void createPlayerBullet(const QPointF &position, const QPointF &direction, int damage);
    void createEnemyBullet(const QPointF &position, const QPointF &direction, int damage);
    
    // 碰撞检测
    void checkPlayerEnemyCollisions();
    void checkPlayerBulletEnemyCollisions();
    void checkEnemyBulletPlayerCollisions();
    void checkPlayerItemCollisions();
    void checkPlayerCreatureCollisions();
    
    // 清理方法
    void removeDeadEnemies();
    void removeOutOfBoundsBullets();
    void removeCollectedItems();
    void removeActivatedCreatures();
    
    // 敌人生成逻辑
    void updateEnemySpawning();
    QPointF getRandomSpawnPosition();
    
    // 道具和生物生成逻辑
    void spawnRandomItem();
    void spawnRandomCreature();
    void updateItemSpawning();
    void updateCreatureSpawning();
    
    // 游戏对象
    SugarOilPlayer* mPlayer;
    QList<EnemyBase*> mEnemies;
    QList<BulletBase*> mPlayerBullets;
    QList<BulletBase*> mEnemyBullets;
    QList<GameItem*> mItems;
    QList<GameCreature*> mCreatures;
    
    // 背景
    QGraphicsPixmapItem* mBackground;
    
    // 定时器
    QTimer* mGameTimer;
    QTimer* mUpdateTimer;
    QTimer* mSpawnTimer;
    
    // 游戏状态
    bool mGameRunning;
    bool mGamePaused;
    int mGameTime; // 游戏时间（秒）
    int mSpawnCounter;
    
    // 输入状态
    QSet<int> mPressedKeys;
    QPointF mLastMousePos;
    bool mMousePressed;
    
    // 音频现在由AudioManager统一管理
    
    // 系统管理器
    ItemManager* mItemManager;
    CreatureManager* mCreatureManager;
    
    // 生成计时器
    QTimer* mItemSpawnTimer;
    QTimer* mCreatureSpawnTimer;
    int mItemSpawnCounter;
    int mCreatureSpawnCounter;
    
    // 游戏配置
    static const int GAME_DURATION = 300; // 5分钟
    static const int UPDATE_INTERVAL = 25; // ~40 FPS，降低更新频率减少卡顿
    static const int SPAWN_INTERVAL = 1500; // 1.5秒，降低生成频率
    static const int SCENE_WIDTH = SUGAR_OIL_SCENE_WIDTH;
    static const int SCENE_HEIGHT = SUGAR_OIL_SCENE_HEIGHT;
};

#endif // SUGAR_OIL_GAME_SCENE_NEW_H