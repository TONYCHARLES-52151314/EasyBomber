#pragma once

#include "Types.hpp"

class Map;

// 所有动态游戏对象的公共基类。Player、Monster、Bomb、Explosion 都继承它。
// Game 用基类指针统一管理，调用 update/draw 时不用管具体类型（多态）。
class GameObject {
protected:
    GridPosition pos;     // 当前格子坐标
    bool active = true;   // 存活/活跃标记

public:
    GameObject(GridPosition p) : pos(p) {}
    virtual ~GameObject() = default;   // 保证 delete 基类指针时会调子类析构

    virtual void update(int dt, const Map& map) = 0;  // 纯虚函数，子类必须实现
    virtual void draw() const = 0;                    // 纯虚函数，子类必须实现

    GridPosition getPos() const { return pos; }
    bool isActive() const { return active; }
    void remove() { active = false; }   // 标记待删除，不真的 delete，由 Game 统一清理
};
