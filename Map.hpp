#pragma once

#include "Types.hpp"

class Map {
public:
    // 15 行 × 21 列，每格 36×36 像素
    static const int ROWS = 15;
    static const int COLS = 21;
    static const int BLOCK = 36;
    static const int START_X = 20;   // 地图左上角离屏幕左边的像素偏移
    static const int START_Y = 70;   // 地图左上角离屏幕顶边的像素偏移

    Map();
    void reset();   // 从字符地图重建初始地图
    void draw() const;

    // —— 查询（只读） ——
    TileType getTile(GridPosition p) const;   // 查格子类型，越界返回 WALL
    bool inside(GridPosition p) const;        // 坐标是否在 15×21 范围内
    bool canWalk(GridPosition p) const;       // 能不能走入（只有 ROAD 能走）

    // —— 修改 ——
    bool breakBrick(GridPosition p);          // 砖块炸成空地
    vector<GridPosition> getBlast(GridPosition center, int range) const; // 爆炸传播

    GridPosition getPlayerStart() const { return playerStart; }
    vector<GridPosition> getMonsterStarts() const { return monsterStarts; }

    // 格子坐标 → 屏幕像素
    static int getX(int col) { return START_X + col * BLOCK; }
    static int getY(int row) { return START_Y + row * BLOCK; }

private:
    vector<vector<TileType>> data;   // 二维数组，data[row][col] 就是地图数据
    GridPosition playerStart{};
    vector<GridPosition> monsterStarts;
};
