#include <graphics.h>

#include "Player.hpp"
#include "Map.hpp"

Player::Player(GridPosition p) : GameObject(p) {}

// 玩家移动由键盘事件直接驱动，不需要每帧自动更新
void Player::update(int, const Map&) {}

void Player::draw() const {
    static IMAGE image;       // static: 只加载一次图片，不每帧重新读文件
    static bool loaded = false;
    if (!loaded) {
        loadimage(&image, L"assets\\player.bmp", Map::BLOCK, Map::BLOCK);
        loaded = true;
    }
    putimage(Map::getX(pos.col), Map::getY(pos.row), &image);  // 格子坐标 → 屏幕贴图
}

// ① 算目标 → ② 查能走吗 → ③ 查有炸弹吗 → ④ 移动
bool Player::tryMove(Direction dir, const Map& map, const vector<GridPosition>& bombs) {
    GridPosition next = moved(pos, dir);
    if (!map.canWalk(next) || containsPosition(bombs, next)) return false;  // 墙/砖块/炸弹挡路
    pos = next;
    return true;
}
