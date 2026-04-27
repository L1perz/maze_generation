#pragma once
#include "ISearchStrategy.h"

class BFSStrategy : public ISearchStrategy {
public:
    std::vector<Point> findPath(const Maze& maze, Point start, Point end) override;
};