#include "game_map.h"
#include <QPointF>
#include <QPoint>

// 地图布局模板（1=墙，0=通道，2=假蔬菜）
const int GameMap::mapTemplate[MAP_ROWS][MAP_COLS] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,2,1,2,1,1,1,2,1},
    {1,2,2,2,2,2,1,2,2,2,2,1,1,1,2,2,2,2,1,2,2,2,2,2,1},
    {1,1,1,1,1,2,1,1,1,1,0,1,1,1,0,1,1,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,1,1,0,1,1,1,0,1,1,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,0,1,1,0,0,0,1,1,0,1,1,2,1,1,1,1,1},
    {0,0,0,0,0,2,0,0,0,1,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0},
    {1,1,1,1,1,2,1,1,0,1,1,1,1,1,1,1,0,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,0,0,0,0,0,0,0,0,0,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,1,1,0,1,1,1,0,1,1,1,1,2,1,1,1,1,1},
    {1,1,1,1,1,2,1,1,1,1,0,1,1,1,0,1,1,1,1,2,1,1,1,1,1},
    {1,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,2,2,2,2,2,2,2,2,1},
    {1,2,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1},
    {1,2,2,2,1,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,1,2,2,2,1},
    {1,1,1,2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,2,1,2,1,1,1},
    {1,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,2,2,1,2,2,2,2,2,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

GameMap::GameMap(QObject *parent)
    : QObject(parent), remainingFakeVegetables(0)
{
    initializeMap();
}

GameMap::~GameMap()
{
}

void GameMap::initializeMap()
{
    remainingFakeVegetables = 0;
    
    for (int row = 0; row < MAP_ROWS; ++row) {
        for (int col = 0; col < MAP_COLS; ++col) {
            int cellType = mapTemplate[row][col];
            
            wallData[row][col] = (cellType == 1);
            fakeVegetableData[row][col] = (cellType == 2);
            
            if (cellType == 2) {
                remainingFakeVegetables++;
            }
        }
    }
}

bool GameMap::isWall(int row, int col) const
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) {
        return true; // 边界外视为墙
    }
    return wallData[row][col];
}

bool GameMap::hasFakeVegetable(int row, int col) const
{
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) {
        return false;
    }
    return fakeVegetableData[row][col];
}

void GameMap::removeFakeVegetable(int row, int col)
{
    if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
        if (fakeVegetableData[row][col]) {
            fakeVegetableData[row][col] = false;
            remainingFakeVegetables--;
        }
    }
}

QPointF GameMap::cellToPixel(int row, int col) const
{
    return QPointF(col * CELL_SIZE + CELL_SIZE / 2, row * CELL_SIZE + CELL_SIZE / 2);
}

QPoint GameMap::pixelToCell(const QPointF& pixel) const
{
    return QPoint(static_cast<int>(pixel.x() / CELL_SIZE), static_cast<int>(pixel.y() / CELL_SIZE));
}

bool GameMap::isValidPosition(int row, int col) const
{
    return !isWall(row, col);
}