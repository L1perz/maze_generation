#include <catch2/catch_test_macros.hpp>
#include "DFSStrategy.h"
#include "Generator.h"
#include "Maze.h"

TEST_CASE("DFS: finds path in empty maze", "[DFS]") {
    Maze maze(3, 3);
    DFSStrategy dfs;
    auto path = dfs.findPath(maze, Point(0, 0), Point(2, 2));

    REQUIRE_FALSE(path.empty());
    REQUIRE(path.front() == Point(0, 0));
    REQUIRE(path.back() == Point(2, 2));
}

TEST_CASE("DFS: start equals end", "[DFS]") {
    Maze maze(5, 5);
    DFSStrategy dfs;
    auto path = dfs.findPath(maze, Point(2, 2), Point(2, 2));

    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == Point(2, 2));
}

TEST_CASE("DFS: impossible path returns empty", "[DFS]") {
    Maze maze(2, 2);
    maze.setWall(Point(0, 1), true);
    maze.setWall(Point(1, 0), true);
    maze.setWall(Point(1, 1), true);

    DFSStrategy dfs;
    auto path = dfs.findPath(maze, Point(0, 0), Point(1, 1));
    REQUIRE(path.empty());
}

TEST_CASE("DFS: path does not go through walls", "[DFS]") {
    Maze maze(5, 5);
    maze.setWall(Point(1, 0), true);
    maze.setWall(Point(1, 1), true);
    maze.setWall(Point(1, 2), true);

    DFSStrategy dfs;
    auto path = dfs.findPath(maze, Point(0, 0), Point(4, 4));

    REQUIRE_FALSE(path.empty());
    for (const auto& p : path) {
        REQUIRE_FALSE(maze.isWall(p));
    }
}

TEST_CASE("DFS: finds path in generated maze", "[DFS]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);

    DFSStrategy dfs;
    auto path = dfs.findPath(maze, maze.getStart(), maze.getEnd());

    REQUIRE_FALSE(path.empty());
    REQUIRE(path.front() == maze.getStart());
    REQUIRE(path.back() == maze.getEnd());
}
