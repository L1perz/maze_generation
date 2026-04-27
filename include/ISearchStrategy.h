#pragma once
#include <vector>
#include "Maze.h"

class ISearchStrategy {
public:
    virtual std::vector<Point> findPath(const Maze& maze, Point start, Point end) = 0;
    virtual ~ISearchStrategy() = default;
};