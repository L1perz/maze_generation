#pragma once
#include <vector>
#include "Point.h"

class Maze {
public:
    Maze(int width = 10, int height = 10);
    void reset();
    void setSize(int width, int height);
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    bool isWall(const Point& p) const;
    void setWall(const Point& p, bool wall);
    void setStart(const Point& s) { start_ = s; }
    void setEnd(const Point& e) { end_ = e; }
    Point getStart() const { return start_; }
    Point getEnd() const { return end_; }
    std::vector<Point> getNeighbors(const Point& p) const;

private:
    int width_, height_;
    std::vector<std::vector<bool>> walls_;
    Point start_, end_;
};