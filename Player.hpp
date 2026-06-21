#pragma once

#include "GameObject.hpp"

class Player : public GameObject {
public:
    Player(GridPosition p);

    void update(int dt, const Map& map) override;
    void draw() const override;
    bool tryMove(Direction dir, const Map& map, const vector<GridPosition>& bombs);
    // 尝试移动：算目标坐标 → 检查路和炸弹 → 能走就更新 pos
};
