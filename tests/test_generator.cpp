#include <catch2/catch_test_macros.hpp>
#include "Generator.h"
#include "Maze.h"
#include "Solver.h"
#include "BFSStrategy.h"

TEST_CASE("Generator: start and end are not walls", "[Generator]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);
    REQUIRE_FALSE(maze.isWall(maze.getStart()));
    REQUIRE_FALSE(maze.isWall(maze.getEnd()));
}

TEST_CASE("Generator: generated maze is perfect (all passable cells reachable)", "[Generator]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    int unreachable = 0;
    int totalPassable = 0;
    for (int y = 0; y < maze.getHeight(); ++y) {
        for (int x = 0; x < maze.getWidth(); ++x) {
            Point p(x, y);
            if (!maze.isWall(p)) {
                totalPassable++;
                if (solver.solve(maze, maze.getStart(), p).empty())
                    unreachable++;
            }
        }
    }
    REQUIRE(unreachable == 0);
    REQUIRE(totalPassable > 0);
}

TEST_CASE("Generator: maze size unchanged after generation", "[Generator]") {
    Maze maze(15, 15);
    Generator gen;
    gen.generatePerfectMaze(maze);
    REQUIRE(maze.getWidth() == 15);
    REQUIRE(maze.getHeight() == 15);
}

TEST_CASE("Generator: start is at (0,0) and end at (width-1,height-1) by default", "[Generator]") {
    Maze maze(20, 20);
    Generator gen;
    gen.generatePerfectMaze(maze);
    REQUIRE(maze.getStart() == Point(0, 0));
    REQUIRE(maze.getEnd() == Point(19, 19));
}

TEST_CASE("Generator: generated maze has at least one path from start to end", "[Generator]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, maze.getStart(), maze.getEnd());
    REQUIRE_FALSE(path.empty());
}

TEST_CASE("Generator: repeated generation produces different mazes (not identical)", "[Generator]") {
    Maze maze1(10, 10);
    Maze maze2(10, 10);
    Generator gen;
    gen.generatePerfectMaze(maze1);
    gen.generatePerfectMaze(maze2);
    int walls1 = 0, walls2 = 0;
    for (int y = 0; y < 10; ++y)
        for (int x = 0; x < 10; ++x) {
            if (maze1.isWall(Point(x, y))) walls1++;
            if (maze2.isWall(Point(x, y))) walls2++;
        }
    bool same = true;
    for (int y = 0; y < 10 && same; ++y)
        for (int x = 0; x < 10 && same; ++x)
            if (maze1.isWall(Point(x, y)) != maze2.isWall(Point(x, y))) same = false;
    REQUIRE_FALSE(same);
}