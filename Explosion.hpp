#pragma once

#include "GameObject.hpp"

// 爆炸：保存本次波及的所有格子，显示 450ms 后消失。Game 用 hits() 判定伤害。
class Explosion : public GameObject {
public:
    Explosion(GridPosition center, vector<GridPosition> blast);
    // center → 基类 pos，blast → 自己 cells

    void update(int dt, const Map& map) override;
    void draw() const override;
    bool hits(GridPosition p) const;   // 目标是否在爆炸范围内

private:
    vector<GridPosition> cells;        // 爆炸覆盖的所有格子
    int timer = 0;
    static const int SHOW_TIME = 450;  // 火焰显示 450ms（不到半秒）
};
