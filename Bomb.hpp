#pragma once

#include "GameObject.hpp"

// 炸弹：倒计 2 秒。只计时不引爆，引爆由 Game::explodeBombs 统一处理。
class Bomb : public GameObject {
public:
    Bomb(GridPosition p);

    void update(int dt, const Map& map) override;
    void draw() const override;

    bool isReady() const { return timer >= FUSE_TIME; }  // 到时间了？
    int getSeconds() const;                               // 还剩几秒（画倒计时用）

private:
    int timer = 0;                     // 累计毫秒数
    static const int FUSE_TIME = 2000;  // 引信 2000ms = 2 秒
};
