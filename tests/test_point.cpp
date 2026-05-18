#include <catch2/catch_test_macros.hpp>
#include "Point.h"

TEST_CASE("Point default constructor", "[Point]") {
    Point p;
    REQUIRE(p.x == 0);
    REQUIRE(p.y == 0);
}

TEST_CASE("Point parameterized constructor", "[Point]") {
    Point p(3, 5);
    REQUIRE(p.x == 3);
    REQUIRE(p.y == 5);
}

TEST_CASE("Point equality operator", "[Point]") {
    Point p1(2, 3);
    Point p2(2, 3);
    Point p3(4, 5);
    REQUIRE(p1 == p2);
    REQUIRE_FALSE(p1 == p3);
}

TEST_CASE("Point inequality operator", "[Point]") {
    Point p1(2, 3);
    Point p2(2, 3);
    Point p3(4, 5);
    REQUIRE_FALSE(p1 != p2);
    REQUIRE(p1 != p3);
}

TEST_CASE("Point copy", "[Point]") {
    Point p1(7, 8);
    Point p2 = p1;
    REQUIRE(p2.x == 7);
    REQUIRE(p2.y == 8);
}