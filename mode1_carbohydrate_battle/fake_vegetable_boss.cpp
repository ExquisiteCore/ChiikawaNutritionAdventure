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
    
    // 智能AI逻辑：根据与玩家的距离采用不同策略
    if (!targetCell.isNull()) {
        int distanceToPlayer = qAbs(targetCell.x() - currentCol) + qAbs(targetCell.y() - currentRow);
        
        Direction moveDir = DIR_NONE;
        
        // 根据距离采用不同的AI策略
        if (distanceToPlayer <= 2) {
            // 近距离：尝试包围玩家
            moveDir = getSurroundDirection(targetCell);
        } else if (distanceToPlayer <= 5) {
            // 中距离：直接追击
            moveDir = getDirectionTo(targetCell);
        } else {
            // 远距离：智能寻路追击
            moveDir = getSmartPathDirection(targetCell);
        }
        
        // 添加随机性，避免过于机械
        if (QRandomGenerator::global()->bounded(100) < 15) { // 15%概率随机移动
            QList<Direction> validDirections;
            for (Direction dir : {DIR_LEFT, DIR_UP, DIR_RIGHT, DIR_DOWN}) {
                int testRow = currentRow + DIR_OFFSET[dir][1];
                int testCol = currentCol + DIR_OFFSET[dir][0];
                if (canMoveTo(testRow, testCol)) {
                    validDirections.append(dir);
                }
            }
            if (!validDirections.isEmpty()) {
                moveDir = validDirections[QRandomGenerator::global()->bounded(validDirections.size())];
            }
        }
        
        if (moveDir != DIR_NONE) {
            int newRow = currentRow + DIR_OFFSET[moveDir][1];
            int newCol = currentCol + DIR_OFFSET[moveDir][0];
            
            if (canMoveTo(newRow, newCol)) {
                currentDirection = moveDir;
                moveToNextCell();
            } else {
                // 如果不能直接移动，使用备用路径
                Direction fallbackDir = getFallbackDirection(moveDir);
                if (fallbackDir != DIR_NONE) {
                    int testRow = currentRow + DIR_OFFSET[fallbackDir][1];
                    int testCol = currentCol + DIR_OFFSET[fallbackDir][0];
                    if (canMoveTo(testRow, testCol)) {
                        currentDirection = fallbackDir;
                        moveToNextCell();
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

// 包围策略：尝试移动到玩家周围的位置
Direction FakeVegetableBoss::getSurroundDirection(QPoint target) const
{
    // 计算到玩家的相对位置
    int deltaX = target.x() - currentCol;
    int deltaY = target.y() - currentRow;
    
    // 如果已经在玩家旁边，尝试绕到玩家的另一侧
    if (qAbs(deltaX) <= 1 && qAbs(deltaY) <= 1) {
        // 尝试绕到玩家的对面
        QList<Direction> surroundDirs;
        
        // 优先选择垂直于当前方向的移动
        if (qAbs(deltaX) > qAbs(deltaY)) {
            surroundDirs << DIR_UP << DIR_DOWN;
            if (deltaX > 0) surroundDirs << DIR_RIGHT;
            else surroundDirs << DIR_LEFT;
        } else {
            surroundDirs << DIR_LEFT << DIR_RIGHT;
            if (deltaY > 0) surroundDirs << DIR_DOWN;
            else surroundDirs << DIR_UP;
        }
        
        for (Direction dir : surroundDirs) {
            int testRow = currentRow + DIR_OFFSET[dir][1];
            int testCol = currentCol + DIR_OFFSET[dir][0];
            if (canMoveTo(testRow, testCol)) {
                return dir;
            }
        }
    }
    
    // 否则直接朝玩家移动
    return getDirectionTo(target);
}

// 智能寻路：考虑障碍物的路径规划
Direction FakeVegetableBoss::getSmartPathDirection(QPoint target) const
{
    Direction directDir = getDirectionTo(target);
    
    // 如果直接路径可行，使用直接路径
    if (directDir != DIR_NONE) {
        int testRow = currentRow + DIR_OFFSET[directDir][1];
        int testCol = currentCol + DIR_OFFSET[directDir][0];
        if (canMoveTo(testRow, testCol)) {
            return directDir;
        }
    }
    
    // 如果直接路径被阻挡，尝试绕路
    int deltaX = target.x() - currentCol;
    int deltaY = target.y() - currentRow;
    
    QList<Direction> alternatives;
    
    // 根据目标位置确定备选路径
    if (qAbs(deltaX) > qAbs(deltaY)) {
        // 水平距离更大，优先尝试垂直移动绕路
        if (deltaY != 0) {
            alternatives << ((deltaY > 0) ? DIR_DOWN : DIR_UP);
        }
        alternatives << ((deltaX > 0) ? DIR_RIGHT : DIR_LEFT);
        if (deltaY != 0) {
            alternatives << ((deltaY > 0) ? DIR_UP : DIR_DOWN);
        }
    } else {
        // 垂直距离更大，优先尝试水平移动绕路
        if (deltaX != 0) {
            alternatives << ((deltaX > 0) ? DIR_RIGHT : DIR_LEFT);
        }
        alternatives << ((deltaY > 0) ? DIR_DOWN : DIR_UP);
        if (deltaX != 0) {
            alternatives << ((deltaX > 0) ? DIR_LEFT : DIR_RIGHT);
        }
    }
    
    // 尝试备选路径
    for (Direction dir : alternatives) {
        int testRow = currentRow + DIR_OFFSET[dir][1];
        int testCol = currentCol + DIR_OFFSET[dir][0];
        if (canMoveTo(testRow, testCol)) {
            return dir;
        }
    }
    
    return DIR_NONE;
}

// 备用方向：当主要方向被阻挡时的替代选择
Direction FakeVegetableBoss::getFallbackDirection(Direction blockedDir) const
{
    QList<Direction> fallbacks;
    
    // 根据被阻挡的方向确定备用方向
    switch (blockedDir) {
        case DIR_UP:
        case DIR_DOWN:
            fallbacks << DIR_LEFT << DIR_RIGHT;
            break;
        case DIR_LEFT:
        case DIR_RIGHT:
            fallbacks << DIR_UP << DIR_DOWN;
            break;
        default:
            fallbacks << DIR_LEFT << DIR_UP << DIR_RIGHT << DIR_DOWN;
            break;
    }
    
    // 随机选择一个可行的备用方向
    QList<Direction> validFallbacks;
    for (Direction dir : fallbacks) {
        int testRow = currentRow + DIR_OFFSET[dir][1];
        int testCol = currentCol + DIR_OFFSET[dir][0];
        if (canMoveTo(testRow, testCol)) {
            validFallbacks.append(dir);
        }
    }
    
    if (!validFallbacks.isEmpty()) {
        return validFallbacks[QRandomGenerator::global()->bounded(validFallbacks.size())];
    }
    
    return DIR_NONE;
}