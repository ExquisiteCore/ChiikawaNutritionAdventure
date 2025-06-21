#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "carbohydrate_config.h"
#include <QObject>

class GameMap : public QObject
{
    Q_OBJECT

public:
    explicit GameMap(QObject *parent = nullptr);
    ~GameMap();
    
    // 地图数据访问
    bool isWall(int row, int col) const;
    bool hasFakeVegetable(int row, int col) const;
    void removeFakeVegetable(int row, int col);
    
    // 获取地图尺寸
    int getRows() const { return MAP_ROWS; }
    int getCols() const { return MAP_COLS; }
    
    // 获取剩余假蔬菜数量
    int getRemainingFakeVegetables() const { return remainingFakeVegetables; }
    
    // 坐标转换
    QPointF cellToPixel(int row, int col) const;
    QPoint pixelToCell(const QPointF& pixel) const;
    
    // 检查位置是否有效（不是墙）
    bool isValidPosition(int row, int col) const;
    
private:
    void initializeMap();
    
    // 地图数据
    bool wallData[MAP_ROWS][MAP_COLS];
    bool fakeVegetableData[MAP_ROWS][MAP_COLS];
    int remainingFakeVegetables;
    
    // 地图布局模板（1=墙，0=通道，2=假蔬菜）
    static const int mapTemplate[MAP_ROWS][MAP_COLS];
};

#endif // GAME_MAP_H