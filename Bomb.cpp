#include <graphics.h>
#include <string>

#include "Bomb.hpp"
#include "Map.hpp"

Bomb::Bomb(GridPosition p) : GameObject(p) {}

void Bomb::update(int dt, const Map&) {
    timer += dt;
    if (timer > FUSE_TIME) timer = FUSE_TIME;  // 卡在 2000，防止 getSeconds 算成负数
}

void Bomb::draw() const {
    static IMAGE image;
    static bool loaded = false;
    if (!loaded) {
        loadimage(&image, L"assets\\bomb.bmp", Map::BLOCK, Map::BLOCK);
        loaded = true;
    }
    int x = Map::getX(pos.col), y = Map::getY(pos.row);
    putimage(x, y, &image);                          // 贴炸弹图

    setbkmode(TRANSPARENT);                          // 文字背景透明
    settextcolor(WHITE);
    settextstyle(16, 0, L"Consolas");
    wstring text = to_wstring(getSeconds());
    outtextxy(x + 19, y + 16, text.c_str());          // 炸弹中间画白色倒计时数字
}

// 向上取整：+999 保证剩 1ms 也显示 1，真正归零才显示 0
int Bomb::getSeconds() const {
    int seconds = (FUSE_TIME - timer + 999) / 1000;
    if (seconds < 0) seconds = 0;
    return seconds;
}
