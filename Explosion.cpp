#include <graphics.h>

#include "Explosion.hpp"
#include "Map.hpp"

Explosion::Explosion(GridPosition center, vector<GridPosition> blast)
    : GameObject(center), cells(blast) {}

void Explosion::update(int dt, const Map&) {
    timer += dt;
    if (timer >= SHOW_TIME) active = false;   // 到了 450ms 就自杀
}

// 在波及的每一个格子上画火焰贴图（其他实体只画一格，这个画一片）
void Explosion::draw() const {
    static IMAGE image;
    static bool loaded = false;
    if (!loaded) {
        loadimage(&image, L"assets\\explosion.bmp", Map::BLOCK, Map::BLOCK);
        loaded = true;
    }
    for (GridPosition cell : cells) {
        putimage(Map::getX(cell.col), Map::getY(cell.row), &image);
    }
}

// 自己还活着，且目标坐标在波及范围内 → 命中
bool Explosion::hits(GridPosition p) const {
    return active && containsPosition(cells, p);
}
