#pragma once
#include "Maze.h"

enum class Difficulty {
    Easy,
    Medium,
    Hard
};

class Generator {
public:
    void generatePerfectMaze(Maze& maze, Difficulty difficulty = Difficulty::Medium);
};
