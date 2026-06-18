#include <catch2/catch_test_macros.hpp>
#include "AStarStrategy.h"
#include "BFSStrategy.h"
#include "Generator.h"
#include "Maze.h"

TEST_CASE("A*: finds shortest path in empty maze", "[AStar]") {
    Maze maze(3, 3);
    AStarStrategy astar;
    auto path = astar.findPath(maze, Point(0, 0), Point(2, 2));

    REQUIRE(path.size() == 5);
    REQUIRE(path.front() == Point(0, 0));
    REQUIRE(path.back() == Point(2, 2));
}

TEST_CASE("A*: start equals end", "[AStar]") {
    Maze maze(5, 5);
    AStarStrategy astar;
    auto path = astar.findPath(maze, Point(2, 2), Point(2, 2));

    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == Point(2, 2));
}

TEST_CASE("A*: impossible path returns empty", "[AStar]") {
    Maze maze(2, 2);
    maze.setWall(Point(0, 1), true);
    maze.setWall(Point(1, 0), true);
    maze.setWall(Point(1, 1), true);

    AStarStrategy astar;
    auto path = astar.findPath(maze, Point(0, 0), Point(1, 1));
    REQUIRE(path.empty());
}

TEST_CASE("A*: matches BFS shortest path length", "[AStar]") {
    Maze maze(7, 7);
    for (int y = 0; y < 6; ++y) {
        maze.setWall(Point(3, y), true);
    }

    AStarStrategy astar;
    BFSStrategy bfs;
    auto astarPath = astar.findPath(maze, Point(0, 0), Point(6, 0));
    auto bfsPath = bfs.findPath(maze, Point(0, 0), Point(6, 0));

    REQUIRE_FALSE(astarPath.empty());
    REQUIRE(astarPath.size() == bfsPath.size());
}

TEST_CASE("A*: finds path in generated maze", "[AStar]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);

    AStarStrategy astar;
    auto path = astar.findPath(maze, maze.getStart(), maze.getEnd());

    REQUIRE_FALSE(path.empty());
    REQUIRE(path.front() == maze.getStart());
    REQUIRE(path.back() == maze.getEnd());
}
