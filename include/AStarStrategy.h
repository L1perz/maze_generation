#pragma once
#include "ISearchStrategy.h"

class AStarStrategy : public ISearchStrategy {
public:
    std::vector<Point> findPath(const Maze& maze, Point start, Point end) override;
};
