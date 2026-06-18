#include <catch2/catch_test_macros.hpp>
#include "Maze.h"
#include "Generator.h"
#include "Solver.h"
#include "BFSStrategy.h"
#include "DFSStrategy.h"
#include "AStarStrategy.h"

TEST_CASE("Scenario 1: Generate perfect maze and verify connectivity", "[Scenario]") {
    Maze maze(21, 21);
    Generator gen;
    gen.generatePerfectMaze(maze);
    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    int unreachable = 0;
    for (int y = 0; y < maze.getHeight(); ++y) {
        for (int x = 0; x < maze.getWidth(); ++x) {
            Point p(x, y);
            if (!maze.isWall(p) && solver.solve(maze, maze.getStart(), p).empty())
                unreachable++;
        }
    }
    REQUIRE(unreachable == 0);
}

TEST_CASE("Scenario 2: BFS finds path in empty maze of size 10x10", "[Scenario]") {
    Maze maze(10, 10);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(9, 9));
    REQUIRE_FALSE(path.empty());
    REQUIRE(path.front() == Point(0, 0));
    REQUIRE(path.back() == Point(9, 9));
}

TEST_CASE("Scenario 3: BFS finds alternative path when direct route blocked", "[Scenario]") {
    Maze maze(3, 3);
    maze.setWall(Point(1, 0), true); 
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(2, 2));
    REQUIRE_FALSE(path.empty());
}

TEST_CASE("Scenario 4: No path when target completely isolated", "[Scenario]") {
    Maze maze(3, 3);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 3; ++x)
            if (Point(x, y) != Point(2, 2))
                maze.setWall(Point(x, y), true);
    BFSStrategy bfs;
    auto path = bfs.findPath(maze, Point(0, 0), Point(2, 2));
    REQUIRE(path.empty());
}

TEST_CASE("Scenario 5: Maze reset clears all walls", "[Scenario]") {
    Maze maze(10, 10);
    for (int i = 0; i < 5; ++i) maze.setWall(Point(i, i), true);
    maze.reset();
    bool anyWall = false;
    for (int y = 0; y < maze.getHeight(); ++y)
        for (int x = 0; x < maze.getWidth(); ++x)
            if (maze.isWall(Point(x, y))) anyWall = true;
    REQUIRE_FALSE(anyWall);
    REQUIRE(maze.getStart() == Point(0, 0));
    REQUIRE(maze.getEnd() == Point(9, 9));
}

TEST_CASE("Scenario 6: Change maze size and regenerate", "[Scenario]") {
    Maze maze(10, 10);
    Generator gen;
    gen.generatePerfectMaze(maze);
    int oldWidth = maze.getWidth();
    maze.setSize(15, 15);
    REQUIRE(maze.getWidth() == 15);
    REQUIRE(maze.getHeight() == 15);
    REQUIRE_FALSE(maze.isWall(Point(0, 0)));
}

TEST_CASE("Scenario 7: User changes start and finish points", "[Scenario]") {
    Maze maze(10, 10);
    maze.setStart(Point(2, 2));
    maze.setEnd(Point(8, 8));

    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, maze.getStart(), maze.getEnd());

    REQUIRE_FALSE(path.empty());
    REQUIRE(path.front() == Point(2, 2));
    REQUIRE(path.back() == Point(8, 8));
}

TEST_CASE("Scenario 8: User switches search algorithm", "[Scenario]") {
    Maze maze(10, 10);
    Solver solver;

    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto bfsPath = solver.solve(maze, maze.getStart(), maze.getEnd());

    solver.setStrategy(std::make_shared<DFSStrategy>());
    auto dfsPath = solver.solve(maze, maze.getStart(), maze.getEnd());

    solver.setStrategy(std::make_shared<AStarStrategy>());
    auto astarPath = solver.solve(maze, maze.getStart(), maze.getEnd());

    REQUIRE_FALSE(bfsPath.empty());
    REQUIRE_FALSE(dfsPath.empty());
    REQUIRE_FALSE(astarPath.empty());
}
