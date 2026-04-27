#include <SFML/Graphics.hpp>
#include <memory>
#include "Maze.h"
#include "Generator.h"
#include "Solver.h"
#include "BFSStrategy.h"

const int CELL_SIZE = 20;

void drawMaze(sf::RenderWindow& window, const Maze& maze, const std::vector<Point>& path = {}) {
    window.clear(sf::Color(30, 30, 30));
    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

    for (int y = 0; y < maze.getHeight(); ++y) {
        for (int x = 0; x < maze.getWidth(); ++x) {
            Point p(x, y);
            cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
            if (maze.isWall(p)) {
                cell.setFillColor(sf::Color::Black);
            }
            else if (p == maze.getStart()) {
                cell.setFillColor(sf::Color::Green);
            }
            else if (p == maze.getEnd()) {
                cell.setFillColor(sf::Color::Red);
            }
            else {
                bool onPath = false;
                for (const auto& pt : path)
                    if (pt == p) { onPath = true; break; }
                cell.setFillColor(onPath ? sf::Color::Yellow : sf::Color(180, 180, 180));
            }
            window.draw(cell);
        }
    }
    window.display();
}

int main() {
    const int W = 21, H = 21;
    Maze maze(W, H);
    Generator gen;
    gen.generatePerfectMaze(maze);

    Solver solver;
    solver.setStrategy(std::make_shared<BFSStrategy>());
    auto path = solver.solve(maze, maze.getStart(), maze.getEnd());

    sf::RenderWindow window(sf::VideoMode(W * CELL_SIZE, H * CELL_SIZE), "Maze Explorer");
    drawMaze(window, maze, path);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
    return 0;
}