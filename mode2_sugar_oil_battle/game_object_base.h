#ifndef GAME_OBJECT_BASE_H
#define GAME_OBJECT_BASE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QDebug>
#include <QPointF>

class GameObjectBase : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    explicit GameObjectBase(QObject *parent = nullptr);
    virtual ~GameObjectBase();

    // 获取中心位置
    QPointF getCenterPos() const;
    
    // 获取当前位置
    QPointF getPos() { return pos(); }
    
    // 设置位置
    void setPosition(const QPointF &position) { setPos(position); }
    void setPosition(qreal x, qreal y) { setPos(x, y); }
    
    // 移动
    void moveBy(qreal dx, qreal dy) { QGraphicsPixmapItem::moveBy(dx, dy); }
    
    // 显示/隐藏
    void showObject() { show(); }
    void hideObject() { hide(); }
    
    // 获取边界矩形
    QRectF getBoundingRect() const { return boundingRect(); }
    
protected:
    // 子类可以重写的更新方法
    virtual void updateObject() {}
};

#endif // GAME_OBJECT_BASE_H