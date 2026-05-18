#include <catch2/catch_test_macros.hpp>
#include "Maze.h"
#include <algorithm>

TEST_CASE("Maze constructor and initial state", "[Maze]") {
    Maze maze(10, 10);
    REQUIRE(maze.getWidth() == 10);
    REQUIRE(maze.getHeight() == 10);
    REQUIRE(maze.getStart() == Point(0, 0));
    REQUIRE(maze.getEnd() == Point(9, 9));
    for (int y = 0; y < maze.getHeight(); ++y)
        for (int x = 0; x < maze.getWidth(); ++x)
            REQUIRE_FALSE(maze.isWall(Point(x, y)));
}

TEST_CASE("Maze setWall and isWall", "[Maze]") {
    Maze maze(5, 5);
    Point p(2, 2);
    maze.setWall(p, true);
    REQUIRE(maze.isWall(p));
    maze.setWall(p, false);
    REQUIRE_FALSE(maze.isWall(p));
}

TEST_CASE("Maze setSize resets walls and start/end", "[Maze]") {
    Maze maze(5, 5);
    maze.setWall(Point(1, 1), true);
    maze.setSize(10, 10);
    REQUIRE(maze.getWidth() == 10);
    REQUIRE(maze.getHeight() == 10);
    REQUIRE_FALSE(maze.isWall(Point(1, 1)));
    REQUIRE(maze.getStart() == Point(0, 0));
    REQUIRE(maze.getEnd() == Point(9, 9));
}

TEST_CASE("Maze getNeighbors - open field", "[Maze]") {
    Maze maze(3, 3);
    auto neighbors = maze.getNeighbors(Point(1, 1));
    REQUIRE(neighbors.size() == 4);
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(0, 1)) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(2, 1)) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(1, 0)) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(1, 2)) != neighbors.end());
}

TEST_CASE("Maze getNeighbors with walls", "[Maze]") {
    Maze maze(5, 5);
    maze.setWall(Point(1, 2), true);
    maze.setWall(Point(2, 1), true);
    auto neighbors = maze.getNeighbors(Point(2, 2));
    REQUIRE(neighbors.size() == 2);
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(3, 2)) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(2, 3)) != neighbors.end());
}

TEST_CASE("Maze getNeighbors on border", "[Maze]") {
    Maze maze(5, 5);
    auto neighbors = maze.getNeighbors(Point(0, 0));
    REQUIRE(neighbors.size() == 2);
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(1, 0)) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), Point(0, 1)) != neighbors.end());
}

TEST_CASE("Maze setStart and setEnd", "[Maze]") {
    Maze maze(10, 10);
    Point newStart(3, 4);
    Point newEnd(7, 8);
    maze.setStart(newStart);
    maze.setEnd(newEnd);
    REQUIRE(maze.getStart() == newStart);
    REQUIRE(maze.getEnd() == newEnd);
}

TEST_CASE("Maze reset clears walls and restores default start/end", "[Maze]") {
    Maze maze(10, 10);
    maze.setWall(Point(2, 2), true);
    maze.setStart(Point(5, 5));
    maze.reset();
    REQUIRE_FALSE(maze.isWall(Point(2, 2)));
    REQUIRE(maze.getStart() == Point(0, 0));
    REQUIRE(maze.getEnd() == Point(9, 9));
}