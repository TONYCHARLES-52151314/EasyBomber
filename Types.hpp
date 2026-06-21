#pragma once

#include <vector>

using namespace std;

// 地图中的格子行列坐标
struct GridPosition {
    int row;
    int col;

    // 运算符重载，可以用 pos1 == pos2 判断坐标是否相等
    bool operator==(const GridPosition& other) const {
        return row == other.row && col == other.col;
    }
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
};

enum TileType {
    ROAD,    // 空地，能走
    WALL,    // 不可破坏的墙，挡住爆炸
    BRICK,   // 可炸毁的砖块，挡住通行和爆炸
};

enum GameState {
    PLAYING,
    WIN,
    LOSE,
};

// inline：允许函数定义在头文件中，避免多个 .cpp 包含时产生重复定义错误
inline GridPosition moved(GridPosition position, Direction direction) {
    switch (direction) {
    case UP: --position.row; break;
    case DOWN: ++position.row; break;
    case LEFT: --position.col; break;
    case RIGHT: ++position.col; break;
    }
    return position;
}

// 判断 target 坐标是否在列表中（线性查找）
inline bool containsPosition(const vector<GridPosition>& positions, GridPosition target) {
    for (GridPosition pos : positions) {
        if (pos == target) return true;
    }
    return false;
}
