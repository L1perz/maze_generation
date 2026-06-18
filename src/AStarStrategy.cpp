#include "AStarStrategy.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>

namespace {
struct PointHash {
    std::size_t operator()(const Point& p) const {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};

int heuristic(Point a, Point b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

struct QueueNode {
    Point point;
    int priority;

    bool operator>(const QueueNode& other) const {
        return priority > other.priority;
    }
};
}

std::vector<Point> AStarStrategy::findPath(const Maze& maze, Point start, Point end) {
    if (maze.isWall(start) || maze.isWall(end)) return {};
    if (start == end) return { start };

    std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>> frontier;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::unordered_map<Point, int, PointHash> costSoFar;

    frontier.push({ start, 0 });
    costSoFar[start] = 0;

    while (!frontier.empty()) {
        Point current = frontier.top().point;
        frontier.pop();

        if (current == end) break;

        for (const Point& next : maze.getNeighbors(current)) {
            int newCost = costSoFar[current] + 1;
            auto knownCost = costSoFar.find(next);
            if (knownCost == costSoFar.end() || newCost < knownCost->second) {
                costSoFar[next] = newCost;
                frontier.push({ next, newCost + heuristic(next, end) });
                cameFrom[next] = current;
            }
        }
    }

    if (cameFrom.find(end) == cameFrom.end()) return {};

    std::vector<Point> path;
    for (Point current = end; current != start; current = cameFrom[current]) {
        path.push_back(current);
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
