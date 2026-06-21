# 简易炸弹人（EasyBomber）

使用 C++17 和 EasyX 编写的单人炸弹人小游戏。代码参考 Pacman-EasyX 的课堂项目风格，使用简单变量名、普通循环和中文注释，方便阅读和讲解。

## 运行

1. 安装 Visual Studio 2026，并选择“使用 C++ 的桌面开发”。
2. 安装 EasyX，并将其配置到 Visual Studio 2026。
3. 双击 `EasyBomber.sln`。
4. 选择 `Debug | x64`，按 `F5` 或 `Ctrl + F5` 运行。

## 操作

- 方向键：移动
- `Space`：放置炸弹
- `R`：重新开始
- `Esc`：退出

炸弹两秒后向四个方向传播两格。普通墙无法炸毁，虚线砖块可以炸毁。消灭全部怪物即可胜利。

## 代码结构

```text
GameObject
├── Player
├── Monster
├── Bomb
└── Explosion
```

- `Game`：控制输入、更新、碰撞、绘制和胜负。
- `Map`：管理地图、出生点、砖块和爆炸传播。
- `GameObject`：玩家、怪物、炸弹和爆炸的共同基类。
- 使用 `unique_ptr` 自动管理动态对象，避免手动释放内存。
- `assets/`：游戏图片资源，构建后自动复制到程序输出目录。
