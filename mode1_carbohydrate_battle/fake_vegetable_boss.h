#ifndef FAKE_VEGETABLE_BOSS_H
#define FAKE_VEGETABLE_BOSS_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QPropertyAnimation>
#include "carbohydrate_config.h"
#include "game_map.h"

class FakeVegetableBoss : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit FakeVegetableBoss(GameMap* gameMap, QObject *parent = nullptr);
    ~FakeVegetableBoss();
    
    // 生命值管理
    int getHealth() const { return health; }
    void takeDamage(int damage);
    bool isAlive() const { return health > 0; }
    
    // 移动控制
    void startMovement();
    void stopMovement();
    void pauseMovement();
    void resumeMovement();
    
    // 位置管理
    QPoint getCurrentCell() const;
    void setPosition(int row, int col);
    
    // 碰撞检测
    enum { Type = TYPE_BOSS };
    int type() const override { return Type; }
    
    // AI行为
    void setTargetPosition(QPoint playerCell);
    
signals:
    void healthChanged(int newHealth);
    void bossDefeated();
    void playerCaught();
    
private slots:
    void updateAI();
    void updateAnimation();
    void onMovementFinished();
    
private:
    void loadSprites();
    void updatePixmap();
    void findPathToPlayer();
    bool canMoveTo(int row, int col) const;
    Direction getDirectionTo(QPoint target) const;
    Direction getSurroundDirection(QPoint target) const;
    Direction getSmartPathDirection(QPoint target) const;
    Direction getFallbackDirection(Direction blockedDir) const;
    void moveToNextCell();
    
    GameMap* map;
    
    // 生命值
    int health;
    int maxHealth;
    
    // 位置信息
    int currentRow, currentCol;
    QPoint targetCell;
    
    // AI相关
    QTimer* aiTimer;
    QTimer* animationTimer;
    QPropertyAnimation* moveAnimation;
    
    // 移动状态
    bool isMoving;
    Direction currentDirection;
    
    // 动画相关
    QPixmap sprites[4][2]; // 4个方向，每个方向2帧动画
    int currentFrame;
    
    // 路径查找
    QList<QPoint> pathToPlayer;
    int pathIndex;
};

#endif // FAKE_VEGETABLE_BOSS_H