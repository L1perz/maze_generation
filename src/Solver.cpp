#include "Solver.h"

void Solver::setStrategy(std::shared_ptr<ISearchStrategy> strat) {
    strategy_ = strat;
}

std::vector<Point> Solver::solve(const Maze& maze, Point start, Point end) {
    if (strategy_)
        return strategy_->findPath(maze, start, end);
    return {};
}