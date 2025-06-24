#ifndef BULLET_BASE_H
#define BULLET_BASE_H

#include "game_object_base.h"
#include <QPointF>
#include <QTimer>

class BulletBase : public GameObjectBase
{
    Q_OBJECT

public:
    enum BulletType {
        PlayerBullet,
        EnemyBullet
    };
    
    enum BulletDirection {
        North,
        South,
        West,
        East,
        NorthWest,
        NorthEast,
        SouthWest,
        SouthEast,
        Custom
    };

    explicit BulletBase(QObject *parent = nullptr);
    BulletBase(GameObjectBase* owner, BulletType type, QObject *parent = nullptr);
    virtual ~BulletBase();

    // 基础属性
    qreal getSpeed() const { return mSpeed; }
    void setSpeed(qreal speed) { mSpeed = speed; }
    
    QPointF getMoveDirection() const { return mMoveDirection; }
    void setMoveDirection(const QPointF &direction);
    void setMoveDirection(BulletDirection direction);
    
    int getDamage() const { return mDamage; }
    void setDamage(int damage) { mDamage = damage; }
    
    BulletType getBulletType() const { return mBulletType; }
    
    GameObjectBase* getOwner() const { return mOwner; }
    
    // 移动
    virtual void moveBullet();
    
    // 检查是否超出边界
    bool isOutOfBounds(const QRectF &sceneBounds) const;
    
    // 启动/停止移动
    void startMoving();
    void stopMoving();
    
    // 对象池功能 - 性能优化
    static BulletBase* getBulletFromPool(GameObjectBase* owner, BulletType type);
    static void returnBulletToPool(BulletBase* bullet);
    void resetBullet(GameObjectBase* owner, BulletType type);
    
signals:
    void bulletOutOfBounds(BulletBase* bullet);
    void bulletHit(BulletBase* bullet, GameObjectBase* target);

public slots:
    void onMoveTimeout();

protected:
    virtual void updateBulletPixmap();
    
private:
    GameObjectBase* mOwner;
    BulletType mBulletType;
    qreal mSpeed;
    QPointF mMoveDirection;
    int mDamage;
    
    QTimer* mMoveTimer;
    
    // 对象池相关
    static QList<BulletBase*> sPlayerBulletPool;
    static QList<BulletBase*> sEnemyBulletPool;
    static const int MAX_POOL_SIZE = 50; // 最大池大小
    
    static const int MOVE_INTERVAL = 25; // ~40 FPS，降低子弹更新频率
};

#endif // BULLET_BASE_H