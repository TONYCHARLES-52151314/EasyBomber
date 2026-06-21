#include <graphics.h>
#include <windows.h>
#include <string>

#include "Game.hpp"
#include "Bomb.hpp"
#include "Explosion.hpp"
#include "Monster.hpp"

// 六只怪物的固定巡逻路线（往返式列表，走到头自动折返）
vector<vector<GridPosition>> monsterRoutes = {
    {{1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 12}, {1, 11}, {1, 10}},
    {{1, 19}, {1, 18}, {1, 17}, {1, 16}, {1, 15}, {1, 16}, {1, 17}, {1, 18}},
    {{5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 4}, {5, 3}, {5, 2}},
    {{5, 19}, {5, 18}, {5, 17}, {5, 16}, {5, 15}, {5, 16}, {5, 17}, {5, 18}},
    {{7, 9}, {7, 10}, {7, 11}, {7, 12}, {7, 13}, {7, 12}, {7, 11}, {7, 10}},
    {{11, 1}, {11, 2}, {11, 3}, {11, 4}, {11, 3}, {11, 2}},
};

Game::Game() { reset(); }

// ===================== 主循环：input → update → draw =====================
int Game::run() {
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    SetWindowText(GetHWnd(), L"简易炸弹人");
    setbkcolor(BLACK);
    setbkmode(TRANSPARENT);
    BeginBatchDraw();          // 开启双缓冲，所有绘制先画后台，FlushBatchDraw 再翻上屏幕

    DWORD lastTime = GetTickCount();
    while (running) {
        input();               // ① 处理键盘

        DWORD now = GetTickCount();
        int dt = now - lastTime;
        lastTime = now;
        if (dt > 100) dt = 100;  // 卡顿保护，防止 dt 过大导致瞬移

        update(dt);            // ② 更新逻辑
        draw();                // ③ 绘制
        FlushBatchDraw();      // ④ 翻屏
        Sleep(8);
    }

    EndBatchDraw();
    closegraph();
    return 0;
}

// ===================== 初始化 =====================
void Game::reset() {
    map.reset();                                         // 地图从字符重建
    state = PLAYING;
    player = make_unique<Player>(map.getPlayerStart());
    objects.clear();

    vector<GridPosition> starts = map.getMonsterStarts();
    for (int i = 0; i < starts.size(); i++) {
        objects.push_back(make_unique<Monster>(starts[i], monsterRoutes[i]));
    }
}

// ===================== 键盘 =====================
void Game::input() {
    ExMessage msg{};
    while (peekmessage(&msg, EX_KEY | EX_WINDOW)) {
        if (msg.message == WM_CLOSE) { running = false; continue; }
        if (msg.message != WM_KEYDOWN) continue;   // 只处理按下

        if (msg.vkcode == VK_ESCAPE) running = false;
        else if (msg.vkcode == 'R') reset();
        else if (state == PLAYING) {
            switch (msg.vkcode) {
            case VK_UP:    player->tryMove(UP,    map, getBombPos()); break;
            case VK_DOWN:  player->tryMove(DOWN,  map, getBombPos()); break;
            case VK_LEFT:  player->tryMove(LEFT,  map, getBombPos()); break;
            case VK_RIGHT: player->tryMove(RIGHT, map, getBombPos()); break;
            case VK_SPACE: putBomb(); break;
            default: break;
            }
        }
    }
}

// ===================== 每帧逻辑 =====================
void Game::update(int dt) {
    if (state != PLAYING) return;

    checkCollision();          // 第一轮碰撞（继承自上帧状态）
    if (state != PLAYING) return;

    player->update(dt, map);   // Player 的 update 是空的
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->isActive()) objects[i]->update(dt, map);  // 多态调用
    }

    explodeBombs();            // 检查并处理炸弹引爆
    checkCollision();          // 第二轮碰撞（新 Explosion 可能伤人）
    clearObjects();            // 清理 active=false 的对象

    if (state == PLAYING && countMonsters() == 0) state = WIN;
}

// ===================== 绘制 =====================
void Game::draw() const {
    cleardevice();
    map.draw();
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->isActive()) objects[i]->draw();  // 多态绘制
    }
    player->draw();
    drawInfo();
    drawEnd();
}

void Game::drawInfo() const {
    settextstyle(20, 0, L"Microsoft YaHei");
    settextcolor(RGB(180, 180, 180));

    wstring monsterText = L"怪物剩余: " + to_wstring(countMonsters());
    wstring bombText = L"炸弹: ";
    bombText += countBombs() == 0 ? L"可用" : L"倒计时中";

    outtextxy(Map::START_X, 20, monsterText.c_str());
    outtextxy(WIN_WIDTH - 150, 20, bombText.c_str());

    settextstyle(16, 0, L"Microsoft YaHei");
    settextcolor(RGB(130, 130, 130));
    outtextxy(Map::START_X, WIN_HEIGHT - 42, L"方向键移动  SPACE 放炸弹  R 重开  ESC 退出");
}

void Game::drawEnd() const {
    if (state == PLAYING) return;   // 进行中不画

    int left = 170, top = 260, right = WIN_WIDTH - 170, bottom = top + 120;
    setfillcolor(BLACK);
    setlinecolor(RGB(180, 180, 180));
    solidrectangle(left, top, right, bottom);  // 黑色背景
    rectangle(left, top, right, bottom);       // 浅灰边框

    settextstyle(30, 0, L"Microsoft YaHei");
    settextcolor(state == WIN ? RGB(255, 210, 70) : RGB(240, 80, 80));
    const wchar_t* title = state == WIN ? L"全部怪物已炸死，胜利！" : L"游戏失败";
    outtextxy(left + 45, top + 22, title);

    settextstyle(17, 0, L"Microsoft YaHei");
    settextcolor(RGB(160, 160, 160));
    outtextxy(left + 105, top + 78, L"按 R 重新开始");
}

// ===================== 辅助 =====================

void Game::putBomb() {
    if (countBombs() == 0) {    // 同时只能有一颗炸弹
        objects.push_back(make_unique<Bomb>(player->getPos()));
    }
}

void Game::explodeBombs() {
    bool exploded = false;
    GridPosition center{};
    vector<GridPosition> cells;

    for (int i = 0; i < objects.size(); i++) {
        Bomb* bomb = dynamic_cast<Bomb*>(objects[i].get());  // 筛选出 Bomb
        if (bomb != nullptr && bomb->isActive() && bomb->isReady()) {
            center = bomb->getPos();
            cells = map.getBlast(center, 2);   // 算爆炸范围
            bomb->remove();
            exploded = true;
        }
    }
    if (!exploded) return;

    for (GridPosition p : cells) map.breakBrick(p);   // 炸掉砖块
    objects.push_back(make_unique<Explosion>(center, cells));  // 生成火焰
}

void Game::checkCollision() {
    // 玩家碰到怪物 → 输
    for (int i = 0; i < objects.size(); i++) {
        Monster* monster = dynamic_cast<Monster*>(objects[i].get());
        if (monster != nullptr && monster->isActive() && monster->getPos() == player->getPos())
            state = LOSE;
    }

    // 爆炸：打中玩家 → 输，打中怪物 → 怪物死
    for (int i = 0; i < objects.size(); i++) {
        Explosion* explosion = dynamic_cast<Explosion*>(objects[i].get());
        if (explosion == nullptr || !explosion->isActive()) continue;

        if (explosion->hits(player->getPos())) state = LOSE;

        for (int j = 0; j < objects.size(); j++) {
            Monster* monster = dynamic_cast<Monster*>(objects[j].get());
            if (monster != nullptr && monster->isActive() && explosion->hits(monster->getPos()))
                monster->remove();
        }
    }
}

// erase 后元素会前移，索引不能直接 ++，所以用 while
void Game::clearObjects() {
    int i = 0;
    while (i < objects.size()) {
        if (!objects[i]->isActive()) objects.erase(objects.begin() + i);  // 删了 i 不变
        else i++;
    }
}

vector<GridPosition> Game::getBombPos() const {
    vector<GridPosition> positions;
    for (int i = 0; i < objects.size(); i++) {
        Bomb* bomb = dynamic_cast<Bomb*>(objects[i].get());
        if (bomb != nullptr && bomb->isActive()) positions.push_back(bomb->getPos());
    }
    return positions;
}

int Game::countMonsters() const {
    int count = 0;
    for (int i = 0; i < objects.size(); i++) {
        Monster* monster = dynamic_cast<Monster*>(objects[i].get());
        if (monster != nullptr && monster->isActive()) count++;
    }
    return count;
}

int Game::countBombs() const {
    int count = 0;
    for (int i = 0; i < objects.size(); i++) {
        Bomb* bomb = dynamic_cast<Bomb*>(objects[i].get());
        if (bomb != nullptr && bomb->isActive()) count++;
    }
    return count;
}

int main() {
    Game game;
    return game.run();
}
