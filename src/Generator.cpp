#include "Generator.h"
#include <random>
#include <stack>
#include <algorithm>

static std::mt19937 rng(std::random_device{}());

void Generator::generatePerfectMaze(Maze& maze) {
    for (int y = 0; y < maze.getHeight(); ++y)
        for (int x = 0; x < maze.getWidth(); ++x)
            maze.setWall({ x, y }, true);

    Point start = maze.getStart();
    maze.setWall(start, false);

    std::stack<Point> stack;
    std::vector<std::vector<bool>> visited(maze.getHeight(), std::vector<bool>(maze.getWidth(), false));
    visited[start.y][start.x] = true;
    stack.push(start);

    const int dx[] = { 2, -2, 0, 0 };
    const int dy[] = { 0, 0, 2, -2 };

    while (!stack.empty()) {
        Point current = stack.top();
        std::vector<Point> neighbors;
        for (int i = 0; i < 4; ++i) {
            Point n(current.x + dx[i], current.y + dy[i]);
            if (n.x >= 0 && n.x < maze.getWidth() && n.y >= 0 && n.y < maze.getHeight()) {
                if (!visited[n.y][n.x]) neighbors.push_back(n);
            }
        }
        if (!neighbors.empty()) {
            std::uniform_int_distribution<> dist(0, neighbors.size() - 1);
            Point chosen = neighbors[dist(rng)];
            Point mid((current.x + chosen.x) / 2, (current.y + chosen.y) / 2);
            maze.setWall(mid, false);
            maze.setWall(chosen, false);
            visited[chosen.y][chosen.x] = true;
            stack.push(chosen);
        }
        else {
            stack.pop();
        }
    }
}