#include "fiber_sword.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>
#include <QtMath>

FiberSword::FiberSword(QPointF startPos, Direction direction, GameMap* gameMap, QObject *parent)
    : QObject(parent), QGraphicsPixmapItem()
    , map(gameMap)
    , moveDirection(direction)
    , startPosition(startPos)
    , moveSpeed(8.0) // 移动速度
    , maxDistance(300.0) // 最大飞行距离
{
    // 加载精灵图片
    loadSprite();
    
    // 设置初始位置
    setPos(startPosition);
    
    // 创建移动动画
    moveAnimation = new QPropertyAnimation(this, "pos");
    moveAnimation->setEasingCurve(QEasingCurve::Linear);
    
    // 创建碰撞检测定时器
    collisionTimer = new QTimer(this);
    
    connect(moveAnimation, &QPropertyAnimation::finished, this, &FiberSword::onMovementFinished);
    connect(collisionTimer, &QTimer::timeout, this, &FiberSword::checkCollisions);
    
    // 计算目标位置
    calculateTargetPosition();
    
    // 设置Z值，确保在其他对象之上
    setZValue(10);
}

FiberSword::~FiberSword()
{
}

void FiberSword::loadSprite()
{
    // 尝试加载项目中的道具图片
    QPixmap itemSprite(":/img/items/itemicon0.png");
    
    if (itemSprite.isNull()) {
        // 如果资源加载失败，创建简单的图形作为替代
        QPixmap defaultSprite(FIBER_SWORD_SIZE, FIBER_SWORD_SIZE);
        defaultSprite.fill(Qt::green);
        swordSprite = defaultSprite;
    } else {
        // 缩放图片到合适大小
        swordSprite = itemSprite.scaled(FIBER_SWORD_SIZE, FIBER_SWORD_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    
    setPixmap(swordSprite);
}

void FiberSword::calculateTargetPosition()
{
    // 根据方向计算目标位置
    QPointF direction;
    
    switch (moveDirection) {
    case DIR_LEFT:
        direction = QPointF(-1, 0);
        break;
    case DIR_UP:
        direction = QPointF(0, -1);
        break;
    case DIR_RIGHT:
        direction = QPointF(1, 0);
        break;
    case DIR_DOWN:
        direction = QPointF(0, 1);
        break;
    default:
        direction = QPointF(0, 1); // 默认向下
        break;
    }
    
    targetPosition = startPosition + direction * maxDistance;
    
    // 确保目标位置在场景范围内
    if (targetPosition.x() < 0) targetPosition.setX(0);
    if (targetPosition.y() < 0) targetPosition.setY(0);
    if (targetPosition.x() > GAME_SCENE_WIDTH) targetPosition.setX(GAME_SCENE_WIDTH);
    if (targetPosition.y() > GAME_SCENE_HEIGHT) targetPosition.setY(GAME_SCENE_HEIGHT);
}

void FiberSword::startMovement()
{
    // 计算移动时间
    qreal distance = QLineF(startPosition, targetPosition).length();
    int duration = static_cast<int>(distance / moveSpeed * 16.67); // 约60FPS
    
    moveAnimation->setDuration(duration);
    moveAnimation->setStartValue(startPosition);
    moveAnimation->setEndValue(targetPosition);
    moveAnimation->start();
    
    // 启动碰撞检测
    collisionTimer->start(16); // 约60FPS检测频率
}

void FiberSword::stopMovement()
{
    moveAnimation->stop();
    collisionTimer->stop();
    emit swordDestroyed();
}

void FiberSword::updateMovement()
{
    // 这个方法由动画系统自动调用
    checkCollisions();
}

void FiberSword::onMovementFinished()
{
    // 移动完成，销毁剑
    collisionTimer->stop();
    emit swordDestroyed();
}

void FiberSword::checkCollisions()
{
    if (!scene()) {
        return;
    }
    
    // 获取当前位置的碰撞项目
    QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);
    
    for (QGraphicsItem* item : collidingItems) {
        if (item->type() == TYPE_BOSS) {
            // 击中BOSS
            emit hitTarget(item);
            stopMovement();
            return;
        }
    }
    
    // 检查是否撞墙
    QPoint currentCell = map->pixelToCell(pos() + QPointF(FIBER_SWORD_SIZE/2, FIBER_SWORD_SIZE/2));
    if (map->isWall(currentCell.y(), currentCell.x())) {
        stopMovement();
        return;
    }
    
    // 检查是否超出边界
    QPointF currentPos = pos();
    if (currentPos.x() < -FIBER_SWORD_SIZE || currentPos.x() > GAME_SCENE_WIDTH ||
        currentPos.y() < -FIBER_SWORD_SIZE || currentPos.y() > GAME_SCENE_HEIGHT) {
        stopMovement();
        return;
    }
}

bool FiberSword::isValidPosition(QPointF pos) const
{
    QPoint cell = map->pixelToCell(pos);
    return !map->isWall(cell.y(), cell.x());
}