#include "fake_vegetable_boss.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QDebug>
#include <QtMath>

FakeVegetableBoss::FakeVegetableBoss(GameMap* gameMap, QObject *parent)
    : QObject(parent), QGraphicsPixmapItem()
    , map(gameMap)
    , health(BOSS_HEALTH), maxHealth(BOSS_HEALTH)
    , currentRow(10), currentCol(6) // BOSS起始位置
    , isMoving(false)
    , currentDirection(DIR_RIGHT)
    , currentFrame(0)
    , pathIndex(0)
{
    // 加载精灵图片
    loadSprites();
    
    // 设置初始位置
    setPosition(currentRow, currentCol);
    
    // 创建定时器
    aiTimer = new QTimer(this);
    animationTimer = new QTimer(this);
    moveAnimation = new QPropertyAnimation(this, "pos");
    
    connect(aiTimer, &QTimer::timeout, this, &FakeVegetableBoss::updateAI);
    connect(animationTimer, &QTimer::timeout, this, &FakeVegetableBoss::updateAnimation);
    connect(moveAnimation, &QPropertyAnimation::finished, this, &FakeVegetableBoss::onMovementFinished);
    
    // 设置动画属性
    moveAnimation->setDuration(200); // 移动动画持续时间
    moveAnimation->setEasingCurve(QEasingCurve::Linear);
    
    // 启动定时器
    animationTimer->start(300); // 动画帧切换间隔
}

FakeVegetableBoss::~FakeVegetableBoss()
{
}

void FakeVegetableBoss::loadSprites()
{
    // 使用项目中的敌人图片资源
    QPixmap chimeraSprite(":/img/roles/chimera1.png");
    QPixmap chimeraMirSprite(":/img/roles/chimera1-mir.png");
    
    if (chimeraSprite.isNull() || chimeraMirSprite.isNull()) {
        // 如果资源加载失败，创建简单的彩色方块作为替代
        QPixmap defaultSprite(ENEMY_SIZE, ENEMY_SIZE);
        defaultSprite.fill(Qt::red);
        
        for (int dir = 0; dir < 4; ++dir) {
            for (int frame = 0; frame < 2; ++frame) {
                sprites[dir][frame] = defaultSprite;
            }
        }
    } else {
        // 缩放图片到合适大小
        chimeraSprite = chimeraSprite.scaled(ENEMY_SIZE, ENEMY_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        chimeraMirSprite = chimeraMirSprite.scaled(ENEMY_SIZE, ENEMY_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 为不同方向设置精灵
        for (int frame = 0; frame < 2; ++frame) {
            sprites[DIR_LEFT][frame] = chimeraMirSprite;
            sprites[DIR_RIGHT][frame] = chimeraSprite;
            sprites[DIR_UP][frame] = chimeraSprite;
            sprites[DIR_DOWN][frame] = chimeraSprite;
        }
    }
    
    updatePixmap();
}

void FakeVegetableBoss::updatePixmap()
{
    setPixmap(sprites[currentDirection][currentFrame]);
}

void FakeVegetableBoss::takeDamage(int damage)
{
    health -= damage;
    if (health < 0) {
        health = 0;
    }
    
    emit healthChanged(health);
    
    if (health <= 0) {
        stopMovement();
        emit bossDefeated();
    }
}

void FakeVegetableBoss::startMovement()
{
    aiTimer->start(500); // AI更新间隔
}

void FakeVegetableBoss::stopMovement()
{
    aiTimer->stop();
    moveAnimation->stop();
    isMoving = false;
}

void FakeVegetableBoss::pauseMovement()
{
    aiTimer->stop();
    animationTimer->stop();
    moveAnimation->pause();
}

void FakeVegetableBoss::resumeMovement()
{
    if (isAlive()) {
        aiTimer->start();
        animationTimer->start();
        moveAnimation->resume();
    }
}

QPoint FakeVegetableBoss::getCurrentCell() const
{
    return QPoint(currentCol, currentRow);
}

void FakeVegetableBoss::setPosition(int row, int col)
{
    if (row >= 0 && row < map->getRows() && col >= 0 && col < map->getCols()) {
        currentRow = row;
        currentCol = col;
        
        QPointF pixelPos = map->cellToPixel(row, col);
        setPos(pixelPos.x() - ENEMY_SIZE/2, pixelPos.y() - ENEMY_SIZE/2);
    }
}

void FakeVegetableBoss::setTargetPosition(QPoint playerCell)
{
    targetCell = playerCell;
}

void FakeVegetableBoss::updateAI()
{
    if (!isAlive() || isMoving) {
        return;
    }
    
    // 简单的追踪AI：朝玩家方向移动
    if (!targetCell.isNull()) {
        Direction moveDir = getDirectionTo(targetCell);
        
        if (moveDir != DIR_NONE) {
            int newRow = currentRow + DIR_OFFSET[moveDir][1];
            int newCol = currentCol + DIR_OFFSET[moveDir][0];
            
            if (canMoveTo(newRow, newCol)) {
                currentDirection = moveDir;
                moveToNextCell();
            } else {
                // 如果不能直接移动，尝试其他方向
                QList<Direction> directions = {DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN};
                directions.removeOne(moveDir);
                
                for (Direction dir : directions) {
                    int testRow = currentRow + DIR_OFFSET[dir][1];
                    int testCol = currentCol + DIR_OFFSET[dir][0];
                    
                    if (canMoveTo(testRow, testCol)) {
                        currentDirection = dir;
                        moveToNextCell();
                        break;
                    }
                }
            }
        }
    }
}

void FakeVegetableBoss::updateAnimation()
{
    currentFrame = (currentFrame + 1) % 2;
    updatePixmap();
}

void FakeVegetableBoss::onMovementFinished()
{
    isMoving = false;
}

bool FakeVegetableBoss::canMoveTo(int row, int col) const
{
    return map->isValidPosition(row, col);
}

Direction FakeVegetableBoss::getDirectionTo(QPoint target) const
{
    int deltaX = target.x() - currentCol;
    int deltaY = target.y() - currentRow;
    
    // 选择距离更大的轴作为移动方向
    if (qAbs(deltaX) > qAbs(deltaY)) {
        return (deltaX > 0) ? DIR_RIGHT : DIR_LEFT;
    } else if (deltaY != 0) {
        return (deltaY > 0) ? DIR_DOWN : DIR_UP;
    }
    
    return DIR_NONE;
}

void FakeVegetableBoss::moveToNextCell()
{
    if (isMoving) {
        return;
    }
    
    int newRow = currentRow + DIR_OFFSET[currentDirection][1];
    int newCol = currentCol + DIR_OFFSET[currentDirection][0];
    
    if (canMoveTo(newRow, newCol)) {
        isMoving = true;
        
        QPointF currentPos = pos();
        QPointF targetPos = map->cellToPixel(newRow, newCol);
        targetPos.setX(targetPos.x() - ENEMY_SIZE/2);
        targetPos.setY(targetPos.y() - ENEMY_SIZE/2);
        
        moveAnimation->setStartValue(currentPos);
        moveAnimation->setEndValue(targetPos);
        moveAnimation->start();
        
        // 更新逻辑位置
        currentRow = newRow;
        currentCol = newCol;
    }
}