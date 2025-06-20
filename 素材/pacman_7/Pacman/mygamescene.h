#ifndef MYGAMESCENE_H
#define MYGAMESCENE_H

#include <QGraphicsScene>
#include <QPainter>
#include <QBrush>
#include <QPixmap>
#include <QRadialGradient>
#include <QColor>
#include <QRectF>
#include <QGraphicsLineItem>

#include "config.h"
#include "map.h"

class MyGameScene : public QGraphicsScene
{
public:
    MyGameScene(GMap* map,int level,QObject *parent=nullptr);
    ~MyGameScene();
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
private:
    GMap* map;
    int level;
    QPixmap* BackGround;
    QGraphicsLineItem *GateLine;
};

#endif // MYGAMESCENE_H
