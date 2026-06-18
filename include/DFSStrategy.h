#pragma once
#include "ISearchStrategy.h"

class DFSStrategy : public ISearchStrategy {
public:
    std::vector<Point> findPath(const Maze& maze, Point start, Point end) override;
};
