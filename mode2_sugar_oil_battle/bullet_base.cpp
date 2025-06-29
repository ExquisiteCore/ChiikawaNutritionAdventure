#include "bullet_base.h"
#include <QPixmap>
#include <QtMath>

// 静态成员变量定义
QList<BulletBase*> BulletBase::sPlayerBulletPool;
QList<BulletBase*> BulletBase::sEnemyBulletPool;

BulletBase::BulletBase(QObject *parent)
    : GameObjectBase(parent)
    , mOwner(nullptr)
    , mBulletType(PlayerBullet)
    , mSpeed(8.0)
    , mMoveDirection(1.0, 0.0)
    , mDamage(10)
    , mIsDestroyed(false)
    , mMoveTimer(nullptr)
{
    // 初始化移动定时器
    mMoveTimer = new QTimer(this);
    mMoveTimer->setInterval(MOVE_INTERVAL);
    connect(mMoveTimer, &QTimer::timeout, this, &BulletBase::onMoveTimeout);
    
    // 设置默认图像
    updateBulletPixmap();
    setScale(0.15);
    setZValue(5);
}

BulletBase::BulletBase(GameObjectBase* owner, BulletType type, QObject *parent)
    : GameObjectBase(parent)
    , mOwner(owner)
    , mBulletType(type)
    , mSpeed(8.0)
    , mMoveDirection(1.0, 0.0)
    , mDamage(10)
    , mIsDestroyed(false)
    , mMoveTimer(nullptr)
{
    // 初始化移动定时器
    mMoveTimer = new QTimer(this);
    mMoveTimer->setInterval(MOVE_INTERVAL);
    connect(mMoveTimer, &QTimer::timeout, this, &BulletBase::onMoveTimeout);
    
    // 设置图像
    updateBulletPixmap();
    setScale(0.5);
    setZValue(5);
}

BulletBase::~BulletBase()
{
    if (mMoveTimer) {
        mMoveTimer->stop();
    }
}

void BulletBase::setMoveDirection(const QPointF &direction)
{
    // 标准化方向向量
    qreal length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
    if (length > 0) {
        mMoveDirection = QPointF(direction.x() / length, direction.y() / length);
    } else {
        mMoveDirection = QPointF(1.0, 0.0); // 默认向右
    }
}

void BulletBase::setMoveDirection(BulletDirection direction)
{
    switch (direction) {
    case North:
        mMoveDirection = QPointF(0.0, -1.0);
        break;
    case South:
        mMoveDirection = QPointF(0.0, 1.0);
        break;
    case West:
        mMoveDirection = QPointF(-1.0, 0.0);
        break;
    case East:
        mMoveDirection = QPointF(1.0, 0.0);
        break;
    case NorthWest:
        mMoveDirection = QPointF(-1.0/qSqrt(2), -1.0/qSqrt(2));
        break;
    case NorthEast:
        mMoveDirection = QPointF(1.0/qSqrt(2), -1.0/qSqrt(2));
        break;
    case SouthWest:
        mMoveDirection = QPointF(-1.0/qSqrt(2), 1.0/qSqrt(2));
        break;
    case SouthEast:
        mMoveDirection = QPointF(1.0/qSqrt(2), 1.0/qSqrt(2));
        break;
    case Custom:
    default:
        // 保持当前方向
        break;
    }
}

void BulletBase::moveBullet()
{
    qreal dx = mSpeed * mMoveDirection.x();
    qreal dy = mSpeed * mMoveDirection.y();
    moveBy(dx, dy);
}

bool BulletBase::isOutOfBounds(const QRectF &sceneBounds) const
{
    QPointF currentPos = pos();
    QRectF bulletRect = boundingRect();
    
    return (currentPos.x() + bulletRect.width() < sceneBounds.left() ||
            currentPos.x() > sceneBounds.right() ||
            currentPos.y() + bulletRect.height() < sceneBounds.top() ||
            currentPos.y() > sceneBounds.bottom());
}

void BulletBase::startMoving()
{
    if (mMoveTimer && !mMoveTimer->isActive()) {
        mMoveTimer->start();
    }
}

void BulletBase::stopMoving()
{
    if (mMoveTimer) {
        mMoveTimer->stop();
    }
}

void BulletBase::onMoveTimeout()
{
    moveBullet();
    
    // 检查是否超出边界（需要场景边界信息）
    // 这里使用一个大概的边界检查
    QRectF sceneBounds(0, 0, 1000, 800);
    if (isOutOfBounds(sceneBounds)) {
        emit bulletOutOfBounds(this);
    }
}

void BulletBase::updateBulletPixmap()
{
    // 使用静态变量缓存预加载的图像，避免重复加载导致卡顿
    static QPixmap playerBulletPixmap;
    static QPixmap enemyBulletPixmap;
    static bool pixmapsLoaded = false;
    
    // 首次加载时初始化图像缓存
    if (!pixmapsLoaded) {
        playerBulletPixmap = QPixmap(":/img/bulletsample.png");
        enemyBulletPixmap = QPixmap(":/img/enemybulletsample.png");
        
        // 如果加载失败，创建默认图像
        if (playerBulletPixmap.isNull()) {
            playerBulletPixmap = QPixmap(10, 10);
            playerBulletPixmap.fill(Qt::blue);
        }
        if (enemyBulletPixmap.isNull()) {
            enemyBulletPixmap = QPixmap(10, 10);
            enemyBulletPixmap.fill(Qt::red);
        }
        
        pixmapsLoaded = true;
    }
    
    // 直接使用缓存的图像
    if (mBulletType == PlayerBullet) {
        setPixmap(playerBulletPixmap);
    } else {
        setPixmap(enemyBulletPixmap);
    }
}

// 对象池实现 - 性能优化
BulletBase* BulletBase::getBulletFromPool(GameObjectBase* owner, BulletType type)
{
    // 添加空指针检查
    if (!owner) {
        qWarning("BulletBase::getBulletFromPool: owner is null!");
        return nullptr;
    }
    
    QList<BulletBase*>& pool = (type == PlayerBullet) ? sPlayerBulletPool : sEnemyBulletPool;
    
    BulletBase* bullet = nullptr;
    if (!pool.isEmpty()) {
        // 从池中获取现有对象
        bullet = pool.takeLast();
        
        // 验证从池中获取的对象是否有效
        if (!bullet) {
            qWarning("BulletBase::getBulletFromPool: Retrieved null bullet from pool!");
            // 创建新对象作为备用
            bullet = new BulletBase(owner, type);
        } else {
            // 尝试安全地重置子弹
            try {
                bullet->resetBullet(owner, type);
            } catch (...) {
                qWarning("BulletBase::getBulletFromPool: Exception during resetBullet, creating new bullet");
                delete bullet; // 删除损坏的对象
                bullet = new BulletBase(owner, type);
            }
        }
    } else {
        // 池为空时创建新对象
        bullet = new BulletBase(owner, type);
    }
    
    return bullet;
}

void BulletBase::returnBulletToPool(BulletBase* bullet)
{
    if (!bullet) return;
    
    // 验证子弹对象的有效性
    try {
        // 尝试访问子弹的基本属性来验证对象完整性
        BulletType type = bullet->mBulletType;
        
        // 停止移动和重置状态
        bullet->stopMoving();
        bullet->setVisible(false);
        bullet->markForDestruction(); // 标记为已销毁
        
        QList<BulletBase*>& pool = (type == PlayerBullet) ? sPlayerBulletPool : sEnemyBulletPool;
        
        // 检查是否已经在池中（避免重复添加）
        if (pool.contains(bullet)) {
            qWarning("BulletBase::returnBulletToPool: Bullet already in pool!");
            return;
        }
        
        // 如果池未满，将对象返回池中
        if (pool.size() < MAX_POOL_SIZE) {
            pool.append(bullet);
        } else {
            // 池已满，直接删除对象
            delete bullet;
        }
    } catch (...) {
        qWarning("BulletBase::returnBulletToPool: Exception accessing bullet, deleting it");
        delete bullet;
    }
}

void BulletBase::resetBullet(GameObjectBase* owner, BulletType type)
{
    // 添加空指针检查，提高安全性
    if (!owner) {
        qWarning("BulletBase::resetBullet: owner is null!");
        return;
    }
    
    mOwner = owner;
    mBulletType = type;
    mSpeed = 8.0;
    mMoveDirection = QPointF(1.0, 0.0);
    mDamage = 10;
    mIsDestroyed = false; // 重置销毁状态
    
    // 重新设置图像
    updateBulletPixmap();
    setVisible(true);
    
    // 重置定时器
    if (mMoveTimer) {
        mMoveTimer->stop();
    }
}