#include "game_object_base.h"

GameObjectBase::GameObjectBase(QObject *parent)
    : QObject(parent)
{
    // 基础设置
}

GameObjectBase::~GameObjectBase()
{
    // 清理资源
}

QPointF GameObjectBase::getCenterPos() const
{
    return QPointF(pos().x() + (pixmap().width() * scale() / 2),
                   pos().y() + (pixmap().height() * scale() / 2));
}