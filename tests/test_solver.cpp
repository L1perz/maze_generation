#include <catch2/catch_test_macros.hpp>
#include "Solver.h"
#include "BFSStrategy.h"
#include "Maze.h"

TEST_CASE("Solver: setStrategy and solve works", "[Solver]") {
    Maze maze(3, 3);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, Point(0, 0), Point(2, 2));
    REQUIRE_FALSE(path.empty());
}

TEST_CASE("Solver: no strategy returns empty path", "[Solver]") {
    Maze maze(3, 3);
    Solver solver;
    auto path = solver.solve(maze, Point(0, 0), Point(2, 2));
    REQUIRE(path.empty());
}

TEST_CASE("Solver: changing strategy works", "[Solver]") {
    Maze maze(3, 3);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path1 = solver.solve(maze, Point(0, 0), Point(2, 2));
    REQUIRE_FALSE(path1.empty());
    solver.setStrategy(nullptr);
    auto path2 = solver.solve(maze, Point(0, 0), Point(2, 2));
    REQUIRE(path2.empty());
}

TEST_CASE("Solver: solve with same start and end", "[Solver]") {
    Maze maze(5, 5);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, Point(3, 3), Point(3, 3));
    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == Point(3, 3));
}

TEST_CASE("Solver: solve impossible returns empty", "[Solver]") {
    Maze maze(2, 2);
    maze.setWall(Point(0, 1), true);
    maze.setWall(Point(1, 0), true);
    maze.setWall(Point(1, 1), true);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, Point(0, 0), Point(1, 1));
    REQUIRE(path.empty());
}