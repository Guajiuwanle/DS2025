#include <iostream>
#include <cstring>
#include <queue>
#include <stack>
#include <vector>
using namespace std;

const int MAX_V = 12;  // 图2最大顶点数(A-L对应0-11)
const int INF = 0x3f3f3f3f;

// 图类（邻接矩阵实现）
class Graph {
private:
    int vertexNum;          // 顶点数
    int edgeNum;            // 边数
    int adjMatrix[MAX_V][MAX_V];  // 邻接矩阵
    char vertices[MAX_V];   // 顶点名称(A-L)

public:
    // 构造函数：初始化图
    Graph(int vNum, const char v[], int eNum, const int edges[][3]) {
        vertexNum = vNum;
        edgeNum = eNum;
        memcpy(vertices, v, vNum * sizeof(char));  // 复制顶点名称
        // 初始化邻接矩阵为0（无边）
        memset(adjMatrix, 0, sizeof(adjMatrix));
        // 构建邻接矩阵（无向图，双向赋值）
        for (int i = 0; i < eNum; ++i) {
            int u = edges[i][0];
            int v = edges[i][1];
            int w = edges[i][2];
            adjMatrix[u][v] = w;
            adjMatrix[v][u] = w;
        }
    }

    // 输出邻接矩阵
    void printAdjMatrix() {
        cout << "邻接矩阵（顶点顺序：";
        for (int i = 0; i < vertexNum; ++i) cout << vertices[i] << " ";
        cout << "）：" << endl;
        for (int i = 0; i < vertexNum; ++i) {
            for (int j = 0; j < vertexNum; ++j) {
                cout << adjMatrix[i][j] << "\t";
            }
            cout << endl;
        }
    }

    // BFS：从start顶点出发（顶点索引）
    void BFS(int start) {
        bool visited[MAX_V] = {false};
        queue<int> q;
        visited[start] = true;
        q.push(start);
        cout << "BFS遍历（起点" << vertices[start] << "）：";
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            cout << vertices[u] << " ";
            // 遍历所有邻接顶点
            for (int v = 0; v < vertexNum; ++v) {
                if (adjMatrix[u][v] != 0 && !visited[v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        cout << endl;
    }

    // DFS（递归版）：从start顶点出发
    void DFS(int start) {
        bool visited[MAX_V] = {false};
        cout << "DFS遍历（起点" << vertices[start] << "）：";
        DFSRecursive(start, visited);
        cout << endl;
    }

private:
    void DFSRecursive(int u, bool visited[]) {
        visited[u] = true;
        cout << vertices[u] << " ";
        for (int v = 0; v < vertexNum; ++v) {
            if (adjMatrix[u][v] != 0 && !visited[v]) {
                DFSRecursive(v, visited);
            }
        }
    }

public:
    // Dijkstra最短路径：从start到所有顶点
    void Dijkstra(int start) {
        int dist[MAX_V];     // 起点到各顶点距离
        bool visited[MAX_V] = {false};
        // 初始化距离
        for (int i = 0; i < vertexNum; ++i) {
            dist[i] = (adjMatrix[start][i] != 0) ? adjMatrix[start][i] : INF;
        }
        dist[start] = 0;
        visited[start] = true;

        // 迭代n-1次（确定n-1个顶点的最短路径）
        for (int i = 1; i < vertexNum; ++i) {
            // 找未访问的最小距离顶点
            int minDist = INF, u = -1;
            for (int v = 0; v < vertexNum; ++v) {
                if (!visited[v] && dist[v] < minDist) {
                    minDist = dist[v];
                    u = v;
                }
            }
            if (u == -1) break;  // 无连通顶点
            visited[u] = true;

            // 松弛操作
            for (int v = 0; v < vertexNum; ++v) {
                if (!visited[v] && adjMatrix[u][v] != 0 && dist[u] + adjMatrix[u][v] < dist[v]) {
                    dist[v] = dist[u] + adjMatrix[u][v];
                }
            }
        }

        // 输出结果
        cout << "Dijkstra最短路径（起点" << vertices[start] << "）：" << endl;
        for (int i = 0; i < vertexNum; ++i) {
            if (dist[i] == INF) {
                cout << vertices[start] << "到" << vertices[i] << "：无路径" << endl;
            } else {
                cout << vertices[start] << "到" << vertices[i] << "：" << dist[i] << endl;
            }
        }
    }

    // Prim最小生成树（从start顶点出发）
    void Prim(int start) {
        int key[MAX_V];      // 顶点到生成树的最小权值
        bool inMST[MAX_V] = {false};
        int parent[MAX_V];   // 生成树中父节点
        // 初始化
        for (int i = 0; i < vertexNum; ++i) {
            key[i] = INF;
            parent[i] = -1;
        }
        key[start] = 0;
        inMST[start] = true;

        // 构建生成树（n-1条边）
        for (int i = 1; i < vertexNum; ++i) {
            // 找未加入MST的最小key顶点
            int minKey = INF, u = -1;
            for (int v = 0; v < vertexNum; ++v) {
                if (!inMST[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }
            }
            if (u == -1) break;  // 图不连通
            inMST[u] = true;

            // 更新邻接顶点的key值
            for (int v = 0; v < vertexNum; ++v) {
                if (!inMST[v] && adjMatrix[u][v] != 0 && adjMatrix[u][v] < key[v]) {
                    key[v] = adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }

        // 输出结果
        cout << "Prim最小生成树（起点" << vertices[start] << "）：" << endl;
        int totalWeight = 0;
        for (int i = 0; i < vertexNum; ++i) {
            if (parent[i] != -1) {
                cout << vertices[parent[i]] << "-" << vertices[i] << "（权值：" << adjMatrix[parent[i]][i] << "）" << endl;
                totalWeight += adjMatrix[parent[i]][i];
            }
        }
        cout << "生成树总权值：" << totalWeight << endl;
    }

    // 双连通分量+关节点检测（Tarjan算法）
    void findBiconnectedComponents() {
        int disc[MAX_V], low[MAX_V], parent[MAX_V];
        bool visited[MAX_V] = {false}, ap[MAX_V] = {false};
        stack<pair<int, int> > edgeStack;  // 存储边（u,v）
        int time = 0;

        memset(disc, -1, sizeof(disc));
        memset(low, -1, sizeof(low));
        memset(parent, -1, sizeof(parent));

        cout << "双连通分量及关节点检测结果：" << endl;
        for (int i = 0; i < vertexNum; ++i) {
            if (!visited[i]) {
                tarjanAP(i, visited, disc, low, parent, ap, edgeStack, time);
                // 弹出剩余边（最后一个双连通分量）
                if (!edgeStack.empty()) {
                    cout << "双连通分量：";
                    while (!edgeStack.empty()) {
                        cout << vertices[edgeStack.top().first] << "-" << vertices[edgeStack.top().second] << " ";
                        edgeStack.pop();
                    }
                    cout << endl;
                }
            }
        }

        // 输出关节点
        cout << "关节点：";
        for (int i = 0; i < vertexNum; ++i) {
            if (ap[i]) cout << vertices[i] << " ";
        }
        cout << endl;
    }

private:
    void tarjanAP(int u, bool visited[], int disc[], int low[], int parent[], bool ap[], stack<pair<int, int> >& edgeStack, int& time) {
        visited[u] = true;
        disc[u] = low[u] = ++time;
        int children = 0;

        for (int v = 0; v < vertexNum; ++v) {
            if (adjMatrix[u][v] == 0) continue;  // 无边
            if (!visited[v]) {
                children++;
                parent[v] = u;
                edgeStack.push(make_pair(u, v));
                tarjanAP(v, visited, disc, low, parent, ap, edgeStack, time);

                // 更新low[u]
                low[u] = min(low[u], low[v]);

                // 情况1：根节点且子节点数≥2
                if (parent[u] == -1 && children > 1) {
                    ap[u] = true;
                    // 弹出边形成双连通分量
                    cout << "双连通分量：";
                    while (edgeStack.top() != make_pair(u, v)) {
                        cout << vertices[edgeStack.top().first] << "-" << vertices[edgeStack.top().second] << " ";
                        edgeStack.pop();
                    }
                    cout << vertices[u] << "-" << vertices[v] << " ";
                    edgeStack.pop();
                    cout << endl;
                }

                // 情况2：非根节点，low[v]≥disc[u]
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    ap[u] = true;
                    // 弹出边形成双连通分量
                    cout << "双连通分量：";
                    while (edgeStack.top() != make_pair(u, v)) {
                        cout << vertices[edgeStack.top().first] << "-" << vertices[edgeStack.top().second] << " ";
                        edgeStack.pop();
                    }
                    cout << vertices[u] << "-" << vertices[v] << " ";
                    edgeStack.pop();
                    cout << endl;
                }
            }
            // 回边（避免回退到父节点）
            else if (v != parent[u] && disc[v] < disc[u]) {
                edgeStack.push(make_pair(u, v));
                low[u] = min(low[u], disc[v]);
            }
        }
    }
};

// 图1配置（根据题目图1整理，顶点A-H对应0-7）
const char graph1Vertices[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
const int graph1EdgeNum = 14;
// 边格式：{u, v, 权值}（u/v为顶点索引）
const int graph1Edges[graph1EdgeNum][3] = {
    {0,1,2}, {0,2,3}, {1,3,4}, {1,4,12}, {2,4,1}, {2,7,10},
    {3,4,9}, {3,5,13}, {4,5,5}, {4,6,14}, {5,6,11}, {5,7,8},
    {6,7,2}, {3,6,2}
};

// 图2配置（顶点A-L对应0-11）
const char graph2Vertices[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'};
const int graph2EdgeNum = 18;
const int graph2Edges[graph2EdgeNum][3] = {
    {0,1,1}, {0,2,1}, {1,3,1}, {1,4,1}, {2,4,1}, {2,5,1},
    {3,6,1}, {3,7,1}, {4,7,1}, {4,8,1}, {5,8,1}, {5,9,1},
    {6,10,1}, {7,10,1}, {7,11,1}, {8,11,1}, {9,11,1}, {10,11,1}
};

int main() {
    // 任务1：图1邻接矩阵
    cout << "===== 任务1：图1邻接矩阵 =====" << endl;
    Graph g1(8, graph1Vertices, graph1EdgeNum, graph1Edges);
    g1.printAdjMatrix();

    // 任务2：图1 A点（索引0）BFS/DFS
    cout << "\n===== 任务2：图1 A点BFS/DFS =====" << endl;
    g1.BFS(0);
    g1.DFS(0);

    // 任务3：图1 A点最短路径+最小生成树
    cout << "\n===== 任务3：图1 A点最短路径+最小生成树 =====" << endl;
    g1.Dijkstra(0);
    g1.Prim(0);

    // 任务4：图2双连通分量+关节点（不同起点结果一致）
    cout << "\n===== 任务4：图2双连通分量+关节点 =====" << endl;
    Graph g2(12, graph2Vertices, graph2EdgeNum, graph2Edges);
    g2.findBiconnectedComponents();

    return 0;
}
