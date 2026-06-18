#include "BFSStrategy.h"
#include <queue>
#include <unordered_map>
#include <algorithm>

namespace std {
    template<> struct hash<Point> {
        size_t operator()(const Point& p) const { return p.x * 1000 + p.y; }
    };
}

std::vector<Point> BFSStrategy::findPath(const Maze& maze, Point start, Point end) {
    if (maze.isWall(start) || maze.isWall(end)) return {};
    if (start == end) return { start };
    std::queue<Point> q;
    std::unordered_map<Point, Point> cameFrom;
    std::unordered_map<Point, bool> visited;

    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        Point cur = q.front(); q.pop();
        if (cur == end) break;
        for (const Point& next : maze.getNeighbors(cur)) {
            if (!visited[next]) {
                visited[next] = true;
                cameFrom[next] = cur;
                q.push(next);
            }
        }
    }
    if (cameFrom.find(end) == cameFrom.end() && !(start == end))
        return {};
    std::vector<Point> path;
    Point cur = end;
    while (!(cur == start)) {
        path.push_back(cur);
        cur = cameFrom[cur];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}
