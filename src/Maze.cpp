#include "Maze.h"

Maze::Maze(int width, int height) : width_(width), height_(height) {
    reset();
}

void Maze::reset() {
    walls_.assign(height_, std::vector<bool>(width_, false));
    start_ = Point(0, 0);
    end_ = Point(width_ - 1, height_ - 1);
}

void Maze::setSize(int width, int height) {
    width_ = width;
    height_ = height;
    reset();
}

bool Maze::isWall(const Point& p) const {
    if (p.x < 0 || p.x >= width_ || p.y < 0 || p.y >= height_) return true;
    return walls_[p.y][p.x];
}

void Maze::setWall(const Point& p, bool wall) {
    if (p.x >= 0 && p.x < width_ && p.y >= 0 && p.y < height_)
        walls_[p.y][p.x] = wall;
}

std::vector<Point> Maze::getNeighbors(const Point& p) const {
    std::vector<Point> neighbors;
    const int dx[] = { 1, -1, 0, 0 };
    const int dy[] = { 0, 0, 1, -1 };
    for (int i = 0; i < 4; ++i) {
        Point n(p.x + dx[i], p.y + dy[i]);
        if (n.x >= 0 && n.x < width_ && n.y >= 0 && n.y < height_ && !isWall(n))
            neighbors.push_back(n);
    }
    return neighbors;
}