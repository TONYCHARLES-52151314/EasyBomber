#pragma once

#include "GameObject.hpp"

// 怪物沿预设路线循环巡逻。路线是往返式列表，走到头自动折返。遇到砖块就原地等待。
class Monster : public GameObject {
public:
    Monster(GridPosition p, vector<GridPosition> path);  // p 给基类，path 自己存

    void update(int dt, const Map& map) override;
    void draw() const override;

private:
    vector<GridPosition> route;       // 巡逻路线（往返式列表）
    int routeId = 0;                  // 当前在路线上的索引
    int moveTime = 0;                // 累计毫秒数，到 450 就走一步
    static const int MOVE_TIME = 450;
};
