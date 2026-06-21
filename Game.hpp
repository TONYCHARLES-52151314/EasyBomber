#pragma once

#include "GameObject.hpp"
#include "Map.hpp"
#include "Player.hpp"

#include <memory>

// 游戏总控：编排输入、更新、碰撞和绘制。本身不实现具体逻辑，只负责按顺序调用。
class Game {
public:
    static const int WIN_WIDTH = Map::START_X * 2 + Map::COLS * Map::BLOCK;
    static const int WIN_HEIGHT = Map::START_Y + Map::ROWS * Map::BLOCK + 70;

    Game();
    int run();               // 主循环

private:
    void reset();            // 重建地图 + 创建玩家 + 创建六只怪物
    void input();            // 处理方向键/空格/R/Esc
    void update(int dt);     // 每帧逻辑：碰撞 → 更新 → 引爆 → 再碰撞 → 清理 → 判胜
    void draw() const;       // 清屏 → 地图 → 对象 → 玩家 → UI
    void drawInfo() const;   // 怪物剩余、炸弹状态、操作提示
    void drawEnd() const;    // 胜利/失败弹窗

    void putBomb();          // 放炸弹（同时只能有一颗）
    void explodeBombs();     // 炸弹到时 → getBlast → breakBrick → 生成 Explosion
    void checkCollision();   // 玩家碰怪物/爆炸 → LOSE，爆炸碰怪物 → remove
    void clearObjects();     // 清理 active=false 的对象

    vector<GridPosition> getBombPos() const;  // 当前炸弹坐标（防止玩家踩上去）
    int countMonsters() const;
    int countBombs() const;

    Map map;
    unique_ptr<Player> player;              // 单独持有，方便高频访问
    vector<unique_ptr<GameObject>> objects; // Monster/Bomb/Explosion 混在一起，多态管理
    GameState state = PLAYING;
    bool running = true;
};
