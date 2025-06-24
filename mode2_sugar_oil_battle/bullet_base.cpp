#include "bullet_base.h"
#include <QPixmap>
#include <QtMath>

BulletBase::BulletBase(QObject *parent)
    : GameObjectBase(parent)
    , mOwner(nullptr)
    , mBulletType(PlayerBullet)
    , mSpeed(8.0)
    , mMoveDirection(1.0, 0.0)
    , mDamage(10)
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
    QString imagePath;
    
    if (mBulletType == PlayerBullet) {
        imagePath = ":/img/bulletsample.png";
    } else {
        imagePath = ":/img/enemybulletsample.png";
    }
    
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        setPixmap(pixmap);
    } else {
        // 如果找不到图像，创建一个简单的彩色矩形
        QPixmap defaultPixmap(10, 10);
        if (mBulletType == PlayerBullet) {
            defaultPixmap.fill(Qt::blue);
        } else {
            defaultPixmap.fill(Qt::red);
        }
        setPixmap(defaultPixmap);
    }
}