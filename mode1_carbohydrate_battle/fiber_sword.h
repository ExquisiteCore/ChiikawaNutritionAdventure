#ifndef FIBER_SWORD_H
#define FIBER_SWORD_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPixmap>
#include "carbohydrate_config.h"
#include "game_map.h"

class FiberSword : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit FiberSword(QPointF startPos, Direction direction, GameMap* gameMap, QObject *parent = nullptr);
    ~FiberSword();
    
    // 移动控制
    void startMovement();
    void stopMovement();
    
    // 属性访问
    int getDamage() const { return FIBER_SWORD_DAMAGE; }
    Direction getDirection() const { return moveDirection; }
    
    // 碰撞检测
    enum { Type = TYPE_FIBER_SWORD };
    int type() const override { return Type; }
    
signals:
    void swordDestroyed();
    void hitTarget(QGraphicsItem* target);
    
private slots:
    void updateMovement();
    void onMovementFinished();
    void checkCollisions();
    
private:
    void loadSprite();
    void calculateTargetPosition();
    bool isValidPosition(QPointF pos) const;
    
    GameMap* map;
    
    // 移动相关
    Direction moveDirection;
    QPointF startPosition;
    QPointF targetPosition;
    QPropertyAnimation* moveAnimation;
    QTimer* collisionTimer;
    
    // 移动参数
    qreal moveSpeed;
    qreal maxDistance;
    
    // 视觉效果
    QPixmap swordSprite;
};

#endif // FIBER_SWORD_H