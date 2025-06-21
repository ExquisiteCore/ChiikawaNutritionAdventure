#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QKeyEvent>
#include <QPixmap>
#include "carbohydrate_config.h"
#include "game_map.h"

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit Player(GameMap* gameMap, QObject *parent = nullptr);
    ~Player();
    
    // 移动控制
    void setDirection(Direction dir);
    void move();
    void stopMovement();
    
    // 技能系统
    bool canUseFiberSword() const;
    void useFiberSword();
    
    // 属性访问
    int getFiberValue() const { return fiberValue; }
    void setFiberValue(int value) { fiberValue = value; }
    
    // 位置管理
    QPoint getCurrentCell() const;
    void setPosition(int row, int col);
    
    // 碰撞检测
    enum { Type = TYPE_PLAYER };
    int type() const override { return Type; }
    
    // 动画控制
    void updateAnimation();
    void pauseAnimation();
    void resumeAnimation();
    
signals:
    void fiberSwordUsed(QPointF position, Direction direction);
    void fiberValueChanged(int newValue);
    void positionChanged(QPoint newCell);
    
public slots:
    void handleKeyPress(int key);
    void handleKeyRelease(int key);
    
private slots:
    void updateMovement();
    void updateAnimationFrame();
    
private:
    void loadSprites();
    void updatePixmap();
    bool canMoveTo(int row, int col) const;
    
    GameMap* map;
    
    // 移动相关
    Direction currentDirection;
    Direction nextDirection;
    QTimer* movementTimer;
    QTimer* animationTimer;
    
    // 位置信息
    QPointF targetPosition;
    int currentRow, currentCol;
    
    // 游戏属性
    int fiberValue;
    
    // 动画相关
    QPixmap sprites[4][3]; // 4个方向，每个方向3帧动画
    int currentFrame;
    
    // 输入状态
    bool keyPressed[4]; // 对应四个方向键
};

#endif // PLAYER_H