#include "Generator.h"
#include <random>
#include <stack>
#include <algorithm>

static std::mt19937 rng(std::random_device{}());

namespace {
bool hasPassableNeighbor(const Maze& maze, const Point& p) {
    const int dx[] = { 1, -1, 0, 0 };
    const int dy[] = { 0, 0, 1, -1 };

    for (int i = 0; i < 4; ++i) {
        Point neighbor(p.x + dx[i], p.y + dy[i]);
        if (neighbor.x >= 0 && neighbor.x < maze.getWidth() &&
            neighbor.y >= 0 && neighbor.y < maze.getHeight() &&
            !maze.isWall(neighbor)) {
            return true;
        }
    }

    return false;
}

void openAdditionalPassages(Maze& maze, Difficulty difficulty) {
    int divisor = 0;
    if (difficulty == Difficulty::Easy) {
        divisor = 3;
    }
    else if (difficulty == Difficulty::Medium) {
        divisor = 7;
    }

    if (divisor == 0) return;

    for (int y = 0; y < maze.getHeight(); ++y) {
        for (int x = 0; x < maze.getWidth(); ++x) {
            Point p(x, y);
            if (maze.isWall(p) &&
                hasPassableNeighbor(maze, p) &&
                ((x * 31 + y * 17) % divisor == 0)) {
                maze.setWall(p, false);
            }
        }
    }
}
}

void Generator::generatePerfectMaze(Maze& maze, Difficulty difficulty) {
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
            std::uniform_int_distribution<> dist(0, static_cast<int>(neighbors.size()) - 1);
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

    Point end = maze.getEnd();
    maze.setWall(start, false);
    maze.setWall(end, false);
    if (end.x > 0) maze.setWall({ end.x - 1, end.y }, false);
    if (end.y > 0) maze.setWall({ end.x, end.y - 1 }, false);

    openAdditionalPassages(maze, difficulty);
}
