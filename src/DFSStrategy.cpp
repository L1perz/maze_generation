#include "DFSStrategy.h"
#include <algorithm>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace {
struct PointHash {
    std::size_t operator()(const Point& p) const {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};
}

std::vector<Point> DFSStrategy::findPath(const Maze& maze, Point start, Point end) {
    if (maze.isWall(start) || maze.isWall(end)) return {};
    if (start == end) return { start };

    std::stack<Point> stack;
    std::unordered_set<Point, PointHash> visited;
    std::unordered_map<Point, Point, PointHash> cameFrom;

    stack.push(start);
    visited.insert(start);

    while (!stack.empty()) {
        Point current = stack.top();
        stack.pop();

        if (current == end) break;

        for (const Point& next : maze.getNeighbors(current)) {
            if (visited.find(next) == visited.end()) {
                visited.insert(next);
                cameFrom[next] = current;
                stack.push(next);
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
