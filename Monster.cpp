#include <graphics.h>

#include "Monster.hpp"
#include "Map.hpp"

Monster::Monster(GridPosition p, vector<GridPosition> path)
    : GameObject(p), route(path) {}  // p → 基类 pos，path → 自己 route

// 每帧累计 moveTime。用 while 不是 if：卡顿时 dt 可能 > 450，要走够应有的步数。
void Monster::update(int dt, const Map& map) {
    if (!active || route.empty()) return;

    moveTime += dt;
    while (moveTime >= MOVE_TIME) {
        moveTime -= MOVE_TIME;                           // 扣掉 450ms，多余时间留到下一轮
        int next = (routeId + 1) % route.size();          // 下一个索引，到头绕回 0
        if (map.canWalk(route[next])) {                   // 下一格能走？
            routeId = next;
            pos = route[routeId];                         // 走过去
        }
        // 不能走（砖块还没炸开）→ 原地等，下一帧再试
    }
}

void Monster::draw() const {
    static IMAGE image;
    static bool loaded = false;
    if (!loaded) {
        loadimage(&image, L"assets\\monster.bmp", Map::BLOCK, Map::BLOCK);
        loaded = true;
    }
    putimage(Map::getX(pos.col), Map::getY(pos.row), &image);
}
