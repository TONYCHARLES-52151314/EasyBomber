#include <graphics.h>
#include <string>

#include "Map.hpp"

// 字符地图：#=WALL  B=BRICK  P=玩家出生点  M=怪物出生点  .=ROAD
string initialMap[Map::ROWS] = {
    "#####################",
    "#P..B....M....B....M#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#...B.....B.....B...#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#M....B.......B....M#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#...B....M....B.....#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#.....B.......B.....#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#M...B.....B........#",
    "#.#.#.#.#.#.#.#.#.#.#",
    "#....B.......B......#",
    "#####################",
};

Map::Map() { reset(); }

// 翻译字符地图：先全部填 ROAD，再根据字符覆盖成 WALL/BRICK/出生点
void Map::reset() {
    data.assign(ROWS, vector<TileType>(COLS, ROAD));  // 全部初始化为 ROAD
    monsterStarts.clear();   // 清空旧出生点，防止反复重开越积越多

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            char c = initialMap[row][col];
            GridPosition p{row, col};   // 聚合初始化：{row, col} → p.row, p.col

            switch (c) {
            case '#': data[row][col] = WALL;                   break;
            case 'B': data[row][col] = BRICK;                  break;
            case 'P': playerStart = p;                         break;  // 只记坐标，脚下是 ROAD
            case 'M': monsterStarts.push_back(p);              break;  // 只记坐标
            default: break;  // . 保持 ROAD
            }
        }
    }
}

void Map::draw() const {
    setbkcolor(BLACK);
    setfillcolor(BLACK);

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int left = getX(col), top = getY(row);
            int right = left + BLOCK, bottom = top + BLOCK;

            // 所有格子画暗灰网格线（底纹）
            setlinecolor(RGB(45, 45, 45));
            setlinestyle(PS_SOLID, 1);
            rectangle(left, top, right, bottom);

            if (data[row][col] == WALL) {
                setlinecolor(RGB(145, 145, 145));       // 浅灰粗线框
                setlinestyle(PS_SOLID, 2);
                rectangle(left + 2, top + 2, right - 2, bottom - 2);
            } else if (data[row][col] == BRICK) {
                setlinecolor(RGB(110, 110, 110));       // 虚线框 + 中间一道横线
                setlinestyle(PS_DASH, 1);
                rectangle(left + 5, top + 5, right - 5, bottom - 5);
                line(left + 5, top + BLOCK / 2, right - 5, top + BLOCK / 2);
            }
            // ROAD 不额外画
        }
    }
    setlinestyle(PS_SOLID, 1);   // 恢复实线，避免影响后续绘制
}

// —— 三个查询，自上而下调用：canWalk → getTile → inside ——

bool Map::inside(GridPosition p) const {
    return p.row >= 0 && p.row < ROWS && p.col >= 0 && p.col < COLS;
}

TileType Map::getTile(GridPosition p) const {
    if (!inside(p)) return WALL;   // 地图外面统统当墙，这样调用者不用自己判空
    return data[p.row][p.col];
}

bool Map::canWalk(GridPosition p) const {
    return getTile(p) == ROAD;     // 只有空地能走
}

bool Map::breakBrick(GridPosition p) {
    if (getTile(p) != BRICK) return false;
    data[p.row][p.col] = ROAD;     // 砖块 → 空地
    return true;
}

// 爆炸传播：四个方向各走 range 步
//   WALL  → 停止，不加入
//   BRICK → 加入列表，然后停止（不穿透）
//   ROAD  → 加入列表，继续前进
vector<GridPosition> Map::getBlast(GridPosition center, int range) const {
    vector<GridPosition> cells;
    cells.push_back(center);                // 爆炸中心一定在
    Direction dirs[4] = {UP, DOWN, LEFT, RIGHT};

    for (int i = 0; i < 4; i++) {          // 四个方向
        GridPosition now = center;          // 每个方向都从中心重新出发
        for (int j = 0; j < range; j++) {   // 最多走 range 步
            now = moved(now, dirs[i]);
            TileType tile = getTile(now);

            if (tile == WALL) break;        // 碰墙：不加入，立刻停
            cells.push_back(now);           // 砖块和空地都加入
            if (tile == BRICK) break;       // 砖块：加入但停（挡住爆炸）
        }
    }
    return cells;
}
