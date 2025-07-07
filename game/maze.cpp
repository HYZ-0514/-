#include "maze.h"
#include<queue>
#include <QMessageBox>
#include<QTimer>
#include <algorithm>  // for std::shuffle
#include <random>     // for std::default_random_engine

//生成基础地图（单元格）
void maze::base() {
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if (i == 0 || j == 0 || i == level * 2 + 1 - 1 ||
                j == level * 2 + 1 - 1) {
                map[i][j] = -1;
            }
            else if (i % 2 != 0 && j % 2 != 0) {
                map[i][j] = 1;
            }
            else {
                map[i][j] = 0;
            }
        }
    }
}
//使点的周围设为待定（2）
void maze::_2(int i, int j) {
    if (map[i - 1][j] == 0) {
        map[i - 1][j] = 2;
    }
    if (map[i + 1][j] == 0) {
        map[i + 1][j] = 2;
    }
    if (map[i][j - 1] == 0) {
        map[i][j - 1] = 2;
    }
    if (map[i][j + 1] == 0) {
        map[i][j + 1] = 2;
    }
}
//设定迷宫开始延伸的起点
void maze::start() {
    map[start_x][start_y] = 5;
    _2(start_x, start_y);
}
//循环停止判定（是否存在未判定的区域）
bool maze::judge() {
    bool flag = 0;
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if (map[i][j] == 2) {
                flag = 1;
                return flag;
            }
        }
    }
    return flag;
}
//操作（如果相邻空单元（1）则打通（变为4），如果不相邻空单元则为墙壁（0））
void maze::op(int i, int j) {
    // 原条件：向下打通（上侧已访问，下侧未访问）
    if ((map[i - 1][j] == 3 || map[i - 1][j] == 5) && map[i + 1][j] == 1) {
        map[i][j] = 4;
        map[i + 1][j] = 3;
        _2(i + 1, j);
        start_x = i + 1;
        start_y = j;
    }
    // 原条件：向右打通（左侧已访问，右侧未访问）
    else if ((map[i][j - 1] == 3 || map[i][j - 1] == 5) && map[i][j + 1] == 1) {
        map[i][j] = 4;
        map[i][j + 1] = 3;
        _2(i, j + 1);
        start_x = i;
        start_y = j + 1;
    }
    // 原条件：向上打通（下侧已访问，上侧未访问）
    else if ((map[i + 1][j] == 3 || map[i + 1][j] == 5) && map[i - 1][j] == 1) {
        map[i][j] = 4;
        map[i - 1][j] = 3;
        _2(i - 1, j);
        start_x = i - 1;
        start_y = j;
    }
    // 原条件：向左打通（右侧已访问，左侧未访问）
    else if ((map[i][j + 1] == 3 || map[i][j + 1] == 5) && map[i][j - 1] == 1) {
        map[i][j] = 4;
        map[i][j - 1] = 3;
        _2(i, j - 1);
        start_x = i;
        start_y = j - 1;
    }
    // 新增条件：允许垂直方向打通两个已访问区域（形成环路）
    else if ((map[i - 1][j] == 3 || map[i - 1][j] == 5) &&
        (map[i + 1][j] == 3 || map[i + 1][j] == 5) &&
        (rand() % 100 < 20)) { // 20%概率打通
        map[i][j] = 4;
    }
    // 新增条件：允许水平方向打通两个已访问区域（形成环路）
    else if ((map[i][j - 1] == 3 || map[i][j - 1] == 5) &&
        (map[i][j + 1] == 3 || map[i][j + 1] == 5) &&
        (rand() % 100 < 20)) { // 20%概率打通
        map[i][j] = 4;
    }
    // 默认情况：保持为墙
    else {
        map[i][j] = 0;
    }
}
//随机选择一个待定墙壁判断并操作
void maze::random2() {
    int t = 0;
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if (map[i][j] == 2) {
                t++;
            }
        }
    }
    int k = rand() % t + 1;
    t = 0;
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if (map[i][j] == 2) {
                t++;
                if (t == k) {
                    op(i, j);
                    goto loopout;
                }
            }
        }
    }
loopout:
    if (!judge()) {
        map[start_x][start_y] = 6;
    }
}

//构造函数申请内存空间
maze::maze(int in_level) : level(in_level) {
    // 分配地图内存并初始化为0
    map = new int* [level * 2 + 1];
    for (int i = 0; i < level * 2 + 1; i++) {
        map[i] = new int[level * 2 + 1](); // 使用()初始化为0
    }

    // 初始化起点和玩家位置
    start_x = 1;
    start_y = 1;
    p_x = start_x;
    p_y = start_y;

    // 初始化终点（暂时设为右下角）
    end_x = level * 2 - 1;
    end_y = level * 2 - 1;

    // 初始化动画相关变量
    solveTimer = nullptr;
    currentStep = 0;
    animationIndex = 0;
    forHint = false;

    // 清空路径容器
    solutionPath.clear();
    animatedPath.clear();
    walkedPath.clear();

    // 初始化p数组（搜索用）
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            p[i][j] = 0;
        }
    }
}
maze::~maze() {
    stopSolveAnimation();  // 确保动画停止
    for (int i = 0; i < level * 2 + 1; i++) {
        delete[] map[i];
    }
    delete[] map;
}
//获取地图
int maze::getlevel() {
    return maze::level;
}
int** maze::getmap() {
    return map;
}
int maze::getside() {
    return level * 2 + 1;
}
//生成地图
void maze::makemap() {
    p_x = start_x;
    p_y = start_y;
    base();
    start();

    // 先生成完整迷宫
    while (judge()) {
        random2();
    }

    // 然后在已生成的迷宫上添加河流
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if ((map[i][j] == 3 || map[i][j] == 4) && rand() % 100 < 5) {
                map[i][j] = 8; // 设置为河流
            }
        }
    }// 然后在已生成的迷宫上添加河流
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if ((map[i][j] == 3 || map[i][j] == 4) && rand() % 100 < 3) {
                map[i][j] = 9; // 设置为河流
            }
        }
    }
}
int* maze::operator[](int index) {
    return map[index];
}
//重置地图
void maze::rebuildmap() {
    start_x = 1;
    start_y = 1;
    makemap();
}
bool maze::able(int k, int t) {
    Pos last = x[k - 1];
    Pos cur = last;
    if (t == 1 && p[cur.i][cur.j + 1] != 0)return 0;
    if (t == 2 && p[cur.i + 1][cur.j] != 0)return 0;
    if (t == 3 && p[cur.i][cur.j - 1] != 0)return 0;
    if (t == 4 && p[cur.i - 1][cur.j] != 0)return 0;

    return 1;
}
void maze::brush(int k) {
    if (k <= 0 || k > 1000) {
        return;
    }

    for (int m = 0; m < k; m++) {
        if (x[m].i >= 0 && x[m].i < level * 2 + 1 &&
            x[m].j >= 0 && x[m].j < level * 2 + 1) {
            map[x[m].i][x[m].j] = 7;
        }
    }

    if (x[k - 1].i >= 0 && x[k - 1].i < level * 2 + 1 &&
        x[k - 1].j >= 0 && x[k - 1].j < level * 2 + 1) {
        map[x[k - 1].i][x[k - 1].j] = 6;
    }
}
void maze::search(int k) {
    if (x[k - 1].i == end_x && x[k - 1].j == end_y) {
        brush(k);
    }
    else {
        for (int t = 1; t <= 4; t++) {
            if (able(k, t)) {
                Pos cur = x[k - 1];
                if (t == 1)cur.j += 1;
                if (t == 2)cur.i += 1;
                if (t == 3)cur.j -= 1;
                if (t == 4)cur.i -= 1;
                x[k] = cur;
                p[cur.i][cur.j] = 2;
                search(k + 1);
                p[cur.i][cur.j] = 0;
            }
        }
    }
}
int maze::heuristic(const Pos& a, const Pos& b) {
    return abs(a.i - b.i) + abs(a.j - b.j);
}

// 获取可通行的相邻节点
std::vector<Pos> maze::getNeighbors(const Pos& current) {
    std::vector<Pos> neighbors;
    int dirs[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} }; // 右、下、左、上

    for (auto& dir : dirs) {
        int ni = current.i + dir[0];
        int nj = current.j + dir[1];

        // 检查边界和可通行性（现在包括河流类型8）
        if (ni >= 0 && ni < level * 2 + 1 && nj >= 0 && nj < level * 2 + 1 &&
            (map[ni][nj] == 3 || map[ni][nj] == 4 || map[ni][nj] == 5 ||
                map[ni][nj] == 6 || map[ni][nj] == 7 || map[ni][nj] == 8|| map[ni][nj] == 9)) {
            neighbors.push_back({ ni, nj });
        }
    }
    return neighbors;
}
// 重建路径并标记
void maze::reconstructPath(const std::vector<std::vector<Pos>>& cameFrom, Pos current) {
    // 先清除之前的路径标记(7)
    for (int i = 0; i < level * 2 + 1; i++) {
        for (int j = 0; j < level * 2 + 1; j++) {
            if (map[i][j] == 7) {
                map[i][j] = 3; // 恢复为已定单元
            }
        }
    }

    // 标记新路径
    while (!(current.i == p_x && current.j == p_y)) {
        if (map[current.i][current.j] != 5 && map[current.i][current.j] != 6) {
            map[current.i][current.j] = 7;
        }
        current = cameFrom[current.i][current.j];

        // 防止无限循环
        if (current.i == -1 || current.j == -1) break;
    }
}

// A*算法主函数
void maze::aStarSolve(bool forHint) {
    // 首先确保终点位置正确设置
    bool foundEnd = false;
    for (int i = 0; i < level * 2 + 1 && !foundEnd; i++) {
        for (int j = 0; j < level * 2 + 1 && !foundEnd; j++) {
            if (map[i][j] == 6) {
                end_x = i;
                end_y = j;
                foundEnd = true;
            }
        }
    }

    if (!foundEnd) {
        QMessageBox::information(nullptr, "错误", "未找到终点位置");
        return;
    }

    // 初始化优先队列
    std::priority_queue<AStarNode> openSet;

    // 记录节点来源
    std::vector<std::vector<Pos>> cameFrom(level * 2 + 1, std::vector<Pos>(level * 2 + 1, { -1,-1 }));

    // 记录g值
    std::vector<std::vector<int>> gScore(level * 2 + 1, std::vector<int>(level * 2 + 1, INT_MAX));

    // 起点初始化
    Pos start = { p_x, p_y };
    Pos end = { end_x, end_y };

    gScore[p_x][p_y] = 0;
    int h = heuristic(start, end);
    openSet.push({ start, h, 0, h });

    while (!openSet.empty()) {
        AStarNode current = openSet.top();
        openSet.pop();

        // 到达终点后的处理
        if (current.pos == end) {
            // 存储完整路径
            solutionPath.clear();
            Pos node = current.pos;

            while (node != start) {
                solutionPath.push_back(node);
                node = cameFrom[node.i][node.j];

                // 安全保护，防止意外无限循环
                if (solutionPath.size() > level * level * 4) {
                    qDebug() << "Path reconstruction too long, aborting";
                    solutionPath.clear();
                    return;
                }
            }
            solutionPath.push_back(start);
            std::reverse(solutionPath.begin(), solutionPath.end());
            // 只有提示模式立即标记路径
            if (forHint) {
                for (const auto& pos : solutionPath) {
                    if (map[pos.i][pos.j] != 5 && map[pos.i][pos.j] != 6) {
                        map[pos.i][pos.j] = 7;
                    }
                }
            }
            return;
        }

        // 遍历邻居
        for (Pos& neighbor : getNeighbors(current.pos)) {
            // 根据地形类型计算移动代价
            int moveCost = 1; // 默认普通道路代价为1
            if (map[neighbor.i][neighbor.j] == 8) { // 河流代价为2
                moveCost = 2;
            }
            if (map[neighbor.i][neighbor.j] == 9)
            {
                moveCost = 5;
            }

            int tentativeG = gScore[current.pos.i][current.pos.j] + moveCost;

            // 找到更优路径
            if (tentativeG < gScore[neighbor.i][neighbor.j]) {
                cameFrom[neighbor.i][neighbor.j] = current.pos;
                gScore[neighbor.i][neighbor.j] = tentativeG;
                int f = tentativeG + heuristic(neighbor, end);
                openSet.push({ neighbor, f, tentativeG, f - tentativeG });
            }
        }
    }

    // 如果没有找到路径
    QMessageBox::information(nullptr, "提示", "未找到可行路径");
}
void maze::startSolveAnimation() {
    // 清除之前的状态
    walkedPath.clear();

    // 如果没有路径，先计算路径
    if (solutionPath.empty()) {
        aStarSolve(true);
    }

    currentStep = 0;
    solveTimer = new QTimer();
    QObject::connect(solveTimer, &QTimer::timeout, [this]() {
        this->nextSolveStep();
        });
    solveTimer->start(100);  // 每100毫秒移动一步
}

void maze::nextSolveStep() {
    if (currentStep < solutionPath.size()) {
        // 移动当前位置
        p_x = solutionPath[currentStep].i;
        p_y = solutionPath[currentStep].j;

        // 记录走过的路径（不包括起点和终点）
        if (currentStep > 0 && currentStep < solutionPath.size() - 1) {
            walkedPath.push_back(solutionPath[currentStep]);
            map[p_x][p_y] = 7;  // 走过的路径变黄
        }

        currentStep++;
    }
    else {
        stopSolveAnimation();
    }
}

void maze::stopSolveAnimation() {
    /*if (solveTimer) {
        solveTimer->stop();
        delete solveTimer;
        solveTimer = nullptr;
    }*/
    solutionPath.clear();
}



AntColony::~AntColony() {
    m_map = nullptr; // 防止悬空指针
}

void AntColony::start(Pos startPos) {
    clearAllAnts(); // 这会重置访问矩阵

    m_timer.start();
    Ant initialAnt;
    initialAnt.position = startPos;
    initialAnt.path.push_back(startPos);
    m_ants.push_back(initialAnt);

    // 标记起点为已访问
    m_visited[startPos.i][startPos.j] = true;
}

void AntColony::update() {
    if (m_foundPath || m_ants.empty()) return;

    std::vector<Ant> newAnts;
    newAnts.reserve(m_ants.size() * 2); // 预分配空间

    for (auto& ant : m_ants) {
        if (!ant.active) continue;

        auto possibleMoves = getPossibleMoves(ant);
        if (possibleMoves.empty()) {
            ant.active = false; // 死胡同，直接终止
            continue;
        }

        // 主蚂蚁走第一条路
        moveAnt(ant, possibleMoves[0]);
        if (checkFinish(ant)) return;

        // 分叉时克隆蚂蚁（动态调整最大数量）
        int maxClones = std::min(static_cast<int>(possibleMoves.size() - 1),
            m_maxAnts - static_cast<int>(m_ants.size() + newAnts.size()));

        for (int i = 1; i <= maxClones; ++i) {
            Ant newAnt = ant;
            moveAnt(newAnt, possibleMoves[i]);
            newAnts.push_back(newAnt);
            if (checkFinish(newAnt)) return;
        }
    }

    // 合并新蚂蚁（动态调整最大数量）
    if (m_ants.size() + newAnts.size() <= m_maxAnts * 2) { // 允许临时超过限制
        m_ants.insert(m_ants.end(), newAnts.begin(), newAnts.end());
    }
}


// 修改构造函数，根据迷宫大小动态设置最大蚂蚁数
AntColony::AntColony(maze* mazeMap)
    : m_map(mazeMap),
    m_foundPath(false),
    m_elapsedTime(0) {
    if (m_map) {
        int side = m_map->getside();
        m_visited.resize(side, std::vector<bool>(side, false));
        m_globalVisited.resize(side, std::vector<bool>(side, false));

        // 动态设置最大蚂蚁数：迷宫阶数的平方/2
        m_maxAnts = std::max(50, (side * side) / 2);
    }
    m_ants.clear();
    m_timer.start();
}

void AntColony::clearAllAnts() {
    m_ants.clear();
    m_foundPath = false;
    m_elapsedTime = 0;

    // 重置访问矩阵
    if (m_map) {
        int side = m_map->getside();
        m_visited.assign(side, std::vector<bool>(side, false));
    }
}

std::vector<Pos> AntColony::getPossibleMoves(const Ant& ant) {
    std::vector<Pos> moves;
    int dirs[4][2] = { {0,1}, {1,0}, {0,-1}, {-1,0} };

    for (auto& dir : dirs) {
        Pos next = {
            ant.position.i + dir[0],
            ant.position.j + dir[1]
        };

        // 只检查是否有效移动和蚂蚁是否访问过
        if (isValidMove(next) && !ant.hasVisited(next)) {
            moves.push_back(next);
        }
    }
    return moves;
}

void AntColony::moveAnt(Ant& ant, const Pos& moveTo) {
    ant.position = moveTo;
    ant.path.push_back(moveTo);
    m_globalVisited[moveTo.i][moveTo.j] = true; // 标记为已访问
}

const std::vector<std::vector<bool>>& AntColony::getVisitedMatrix() const {
    return m_visited;
}