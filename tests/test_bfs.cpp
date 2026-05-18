#include <catch2/catch_test_macros.hpp>
#include "BFSStrategy.h"
#include "Maze.h"
#include "Generator.h"

TEST_CASE("BFS: direct path when no walls", "[BFS]") {
    Maze maze(3, 3);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(2, 2));
    REQUIRE(path.size() == 5);
}

TEST_CASE("BFS: path with single obstacle", "[BFS]") {
    Maze maze(3, 3);
    maze.setWall(Point(1, 0), true); 
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(2, 0));
    REQUIRE(path.size() == 5);
}

TEST_CASE("BFS: start equals end", "[BFS]") {
    Maze maze(5, 5);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(2, 2), Point(2, 2));
    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == Point(2, 2));
}

TEST_CASE("BFS: impossible path (fully blocked)", "[BFS]") {
    Maze maze(2, 2);
    maze.setWall(Point(0, 1), true);
    maze.setWall(Point(1, 0), true);
    maze.setWall(Point(1, 1), true);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(1, 1));
    REQUIRE(path.empty());
}

TEST_CASE("BFS: path in generated perfect maze", "[BFS]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, maze.getStart(), maze.getEnd());
    REQUIRE_FALSE(path.empty());
    for (const auto& p : path) {
        REQUIRE_FALSE(maze.isWall(p));
    }
}

TEST_CASE("BFS: path length reasonable for small maze", "[BFS]") {
    Maze maze(10, 10);
    for (int i = 0; i < 10; ++i) maze.setWall(Point(i, 1), true); 
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(9, 0));
    REQUIRE(path.size() <= 100);
    REQUIRE(path.size() >= 10);
}