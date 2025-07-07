#ifndef MAZE_H
#define MAZE_H
#include <iostream>
#include<vector>
#include<queue>
#include<cmath>
#include<QTimer>
#include <QElapsedTimer>
using namespace std;
/**************************************************
 * -1:边界 0:墙壁
 * 1:空单元(初始化后未判定的单元)
 * 2:待定墙壁(新建已定单元时建立)
 * 3:已定单元(确定迷宫树已延伸到的单元)
 * 4:打通墙壁(延伸操作)
 * 5.起点
 * 6.终点
 * 7.已经过路径
 * 8.河流（权重值为2）
 * **************************************************/
struct Pos {
    int i;
    int j;
    bool operator==(const Pos& other) const {
        return i == other.i && j == other.j;
    }

    // 建议同时添加!=运算符
    bool operator!=(const Pos& other) const {
        return !(*this == other);
    }
};
struct AStarNode {
    Pos pos;
    int f;  // f = g + h
    int g;  // 从起点到当前节点的实际代价
    int h;  // 启发式估计值

    // 优先队列比较函数
    bool operator<(const AStarNode& other) const {
        return f > other.f; // 小顶堆，f值小的优先
    }
};




class maze {
private:
    int level;  //迷宫阶数
    int** map;  //地图存储空间
    int start_x, start_y;   //起终点
    int end_x, end_y;
    //生成基础地图（单元格）
    void base();
    //使点的周围设为待定（2）
    void _2(int i, int j);
    //设定迷宫开始延伸的起点
    void start();
    //循环停止判定（是否存在未判定的区域）
    bool judge();
    //操作（如果相邻空单元（1）则打通（变为4），如果不相邻空单元则为墙壁（0））
    void op(int i, int j);
    //随机选择一个待定墙壁判断并操作
    void random2();
    int heuristic(const Pos& a, const Pos& b); // 启发式函数
    std::vector<Pos> getNeighbors(const Pos& current); // 获取相邻节点
    void reconstructPath(const std::vector<std::vector<Pos>>& cameFrom, Pos current); // 重建路径
    QTimer* solveTimer;  // 解决动画计时器
    std::vector<Pos> solutionPath;// 存储解决路径
    std::vector<Pos> animatedPath;  // 存储已动画展示的路径
    int animationIndex;             // 当前动画位置
    int currentStep;  // 当前动画步数
    bool forHint;
    std::vector<Pos> walkedPath;    // 存储已经走过的路径
public:
    //构造函数申请内存空间
    maze(int in_level);
    ~maze();
    //获取地图
    int getlevel();
    int** getmap();
    int getside();
    //生成地图
    void makemap();
    int p_x, p_y; //当前位置
    Pos x[1000];
    int p[100][100];
    bool able(int k, int t);
    void brush(int k);
    void search(int k);
    void aStarSolve(bool forHint); // 替换原有的solve函数
    //重置地图
    void rebuildmap();
    int* operator[](int index);
    void startSolveAnimation();  // 开始解决动画
    void nextSolveStep();  // 动画下一步
    void stopSolveAnimation();  // 停止动画
};

struct Ant {
    Pos position;
    std::vector<Pos> path; // 已走过的路径
    bool active = true;
    int steps = 0; // 记录步数

    // 新增：防止绕圈的逻辑
    bool hasVisited(const Pos& pos) const {
        return std::find(path.begin(), path.end(), pos) != path.end();
    }
};

// 蚁群算法类
class AntColony {
public:
    explicit AntColony(maze* mazeMap);  // 添加 explicit 避免隐式转换
    void update();
    ~AntColony();
    void start(Pos startPos);
    bool foundPath() const { return m_foundPath; }
    int getElapsedTime() const { return m_elapsedTime; }
    const std::vector<Ant>& getAnts() const { return m_ants; }
    void clearAllAnts(); // 新增清除所有蚂蚁的函数
    const std::vector<std::vector<bool>>& getVisitedMatrix() const;

private:
    maze* m_map;
    std::vector<std::vector<bool>> m_globalVisited; // 全局访问记录（所有蚂蚁共享）
    int m_maxAnts = 100; // 限制最大蚂蚁数量
    std::vector<Ant> m_ants;
    QElapsedTimer m_timer;
    bool m_foundPath = false;
    int m_elapsedTime = 0;
    std::vector<std::vector<bool>> m_visited; // 记录所有蚂蚁走过的路径
    QElapsedTimer m_calcTimer; // 专门用于计算时间的计时器

    std::vector<Pos> getPossibleMoves(const Ant& ant);
    void moveAnt(Ant& ant, const Pos& moveTo);
    bool checkFinish(const Ant& ant) {
        if ((*m_map)[ant.position.i][ant.position.j] == 6) {
            m_foundPath = true;
            m_elapsedTime = m_timer.elapsed();
            return true;
        }
        return false;
    }
    bool isValidMove(const Pos& pos) const {
        // 边界检查
        if (pos.i < 0 || pos.i >= m_map->getside() ||
            pos.j < 0 || pos.j >= m_map->getside()) {
            return false;
        }

        // 可通行区域检查
        int cellValue = (*m_map)[pos.i][pos.j];
        return (cellValue == 3 || cellValue == 4 || cellValue == 6 || cellValue == 7||cellValue==8);
    }

};

#endif // MAZE_H
