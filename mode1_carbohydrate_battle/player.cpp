#include "player.h"
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QDebug>

Player::Player(GameMap* gameMap, QObject *parent)
    : QObject(parent), QGraphicsPixmapItem()
    , map(gameMap)
    , currentDirection(DIR_NONE)
    , nextDirection(DIR_NONE)
    , currentRow(10), currentCol(12) // 起始位置在地图中央
    , fiberValue(INITIAL_FIBER_VALUE)
    , currentFrame(0)
{
    // 初始化按键状态
    for (int i = 0; i < 4; ++i) {
        keyPressed[i] = false;
    }
    
    // 加载精灵图片
    loadSprites();
    
    // 设置初始位置
    setPosition(currentRow, currentCol);
    
    // 创建定时器
    movementTimer = new QTimer(this);
    animationTimer = new QTimer(this);
    
    connect(movementTimer, &QTimer::timeout, this, &Player::updateMovement);
    connect(animationTimer, &QTimer::timeout, this, &Player::updateAnimationFrame);
    
    // 启动定时器
    movementTimer->start(1000 / 60); // 60 FPS
    animationTimer->start(150); // 动画帧切换间隔
    
    // 设置碰撞检测
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFocus();
}

Player::~Player()
{
}

void Player::loadSprites()
{
    // 使用项目中的角色图片资源
    QPixmap usagiSprite(":/img/roles/usagi1.png");
    QPixmap usagiMirSprite(":/img/roles/usagi1-mir.png");
    
    if (usagiSprite.isNull() || usagiMirSprite.isNull()) {
        // 如果资源加载失败，创建简单的彩色方块作为替代
        QPixmap defaultSprite(PLAYER_SIZE, PLAYER_SIZE);
        defaultSprite.fill(Qt::blue);
        
        for (int dir = 0; dir < 4; ++dir) {
            for (int frame = 0; frame < 3; ++frame) {
                sprites[dir][frame] = defaultSprite;
            }
        }
    } else {
        // 缩放图片到合适大小
        usagiSprite = usagiSprite.scaled(PLAYER_SIZE, PLAYER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        usagiMirSprite = usagiMirSprite.scaled(PLAYER_SIZE, PLAYER_SIZE, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 为不同方向设置精灵
        for (int frame = 0; frame < 3; ++frame) {
            sprites[DIR_LEFT][frame] = usagiMirSprite;
            sprites[DIR_RIGHT][frame] = usagiSprite;
            sprites[DIR_UP][frame] = usagiSprite;
            sprites[DIR_DOWN][frame] = usagiSprite;
        }
    }
    
    updatePixmap();
}

void Player::updatePixmap()
{
    if (currentDirection != DIR_NONE) {
        setPixmap(sprites[currentDirection][currentFrame]);
    } else {
        setPixmap(sprites[DIR_DOWN][0]); // 默认朝下
    }
}

void Player::setDirection(Direction dir)
{
    nextDirection = dir;
}

void Player::move()
{
    if (nextDirection != DIR_NONE) {
        int newRow = currentRow + DIR_OFFSET[nextDirection][1];
        int newCol = currentCol + DIR_OFFSET[nextDirection][0];
        
        if (canMoveTo(newRow, newCol)) {
            currentDirection = nextDirection;
            setPosition(newRow, newCol);
        }
        nextDirection = DIR_NONE;
    }
}

void Player::stopMovement()
{
    currentDirection = DIR_NONE;
    nextDirection = DIR_NONE;
}

bool Player::canUseFiberSword() const
{
    return fiberValue >= FIBER_SWORD_COST;
}

void Player::useFiberSword()
{
    if (canUseFiberSword()) {
        fiberValue -= FIBER_SWORD_COST;
        emit fiberValueChanged(fiberValue);
        
        // 发射膳食纤维剑
        Direction swordDirection = (currentDirection != DIR_NONE) ? currentDirection : DIR_DOWN;
        QPointF swordPosition = pos();
        emit fiberSwordUsed(swordPosition, swordDirection);
    }
}

QPoint Player::getCurrentCell() const
{
    return QPoint(currentCol, currentRow);
}

void Player::setPosition(int row, int col)
{
    if (row >= 0 && row < map->getRows() && col >= 0 && col < map->getCols()) {
        currentRow = row;
        currentCol = col;
        
        QPointF pixelPos = map->cellToPixel(row, col);
        setPos(pixelPos.x() - PLAYER_SIZE/2, pixelPos.y() - PLAYER_SIZE/2);
        
        emit positionChanged(QPoint(col, row));
        
        // 检查是否吃到假蔬菜
        if (map->hasFakeVegetable(row, col)) {
            map->removeFakeVegetable(row, col);
            // 可以在这里添加得分或其他效果
        }
    }
}

bool Player::canMoveTo(int row, int col) const
{
    return map->isValidPosition(row, col);
}

void Player::handleKeyPress(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_A:
        keyPressed[DIR_LEFT] = true;
        setDirection(DIR_LEFT);
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        keyPressed[DIR_UP] = true;
        setDirection(DIR_UP);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        keyPressed[DIR_RIGHT] = true;
        setDirection(DIR_RIGHT);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        keyPressed[DIR_DOWN] = true;
        setDirection(DIR_DOWN);
        break;
    case Qt::Key_Space:
        useFiberSword();
        break;
    }
}

void Player::handleKeyRelease(int key)
{
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_A:
        keyPressed[DIR_LEFT] = false;
        break;
    case Qt::Key_Up:
    case Qt::Key_W:
        keyPressed[DIR_UP] = false;
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        keyPressed[DIR_RIGHT] = false;
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        keyPressed[DIR_DOWN] = false;
        break;
    }
    
    // 检查是否还有按键被按下
    bool anyKeyPressed = false;
    for (int i = 0; i < 4; ++i) {
        if (keyPressed[i]) {
            anyKeyPressed = true;
            setDirection(static_cast<Direction>(i));
            break;
        }
    }
    
    if (!anyKeyPressed) {
        stopMovement();
    }
}

void Player::updateMovement()
{
    move();
}

void Player::updateAnimationFrame()
{
    if (currentDirection != DIR_NONE) {
        currentFrame = (currentFrame + 1) % 3;
        updatePixmap();
    }
}

void Player::updateAnimation()
{
    updatePixmap();
}

void Player::pauseAnimation()
{
    movementTimer->stop();
    animationTimer->stop();
}

void Player::resumeAnimation()
{
    movementTimer->start();
    animationTimer->start();
}