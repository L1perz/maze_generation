#pragma once
#include <memory>
#include "ISearchStrategy.h"

class Solver {
public:
    void setStrategy(std::shared_ptr<ISearchStrategy> strat);
    std::vector<Point> solve(const Maze& maze, Point start, Point end);
private:
    std::shared_ptr<ISearchStrategy> strategy_;
};