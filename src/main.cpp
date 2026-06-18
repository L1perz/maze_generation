#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include "Maze.h"
#include "Generator.h"
#include "Solver.h"
#include "BFSStrategy.h"
#include "DFSStrategy.h"
#include "AStarStrategy.h"

const int MIN_SIZE = 10;
const int MAX_SIZE = 50;
const int PANEL_WIDTH = 420;
const int PANEL_PADDING = 24;

enum class SearchAlgorithm { BFS, DFS, AStar };
enum class ScreenMode { Main, Help };

enum class ButtonAction {
    TabMain,
    TabHelp,
    SelectBFS,
    SelectDFS,
    SelectAStar,
    SelectEasy,
    SelectMedium,
    SelectHard,
    Smaller,
    Larger,
    FocusSize,
    ApplySize,
    Generate,
    FindPath,
    Clear
};

struct Button {
    sf::FloatRect bounds;
    std::string label;
    ButtonAction action;
    bool active;
};

struct PointHash {
    std::size_t operator()(const Point& p) const {
        return (static_cast<std::size_t>(p.x) << 32) ^ static_cast<std::size_t>(p.y);
    }
};

struct SearchTrace {
    std::vector<Point> visited;
    std::vector<Point> path;
};

std::shared_ptr<ISearchStrategy> makeStrategy(SearchAlgorithm algorithm) {
    switch (algorithm) {
    case SearchAlgorithm::DFS: return std::make_shared<DFSStrategy>();
    case SearchAlgorithm::AStar: return std::make_shared<AStarStrategy>();
    case SearchAlgorithm::BFS:
    default: return std::make_shared<BFSStrategy>();
    }
}

std::string algorithmName(SearchAlgorithm algorithm) {
    switch (algorithm) {
    case SearchAlgorithm::DFS: return "DFS";
    case SearchAlgorithm::AStar: return "A*";
    case SearchAlgorithm::BFS:
    default: return "BFS";
    }
}

std::string difficultyName(Difficulty difficulty) {
    switch (difficulty) {
    case Difficulty::Easy: return "Easy";
    case Difficulty::Hard: return "Hard";
    case Difficulty::Medium:
    default: return "Medium";
    }
}

void updateTitle(sf::RenderWindow& window, const Maze& maze, SearchAlgorithm algorithm, Difficulty difficulty) {
    window.setTitle("Maze Explorer | " + algorithmName(algorithm) + " | " +
        difficultyName(difficulty) + " | " + std::to_string(maze.getWidth()) + "x" +
        std::to_string(maze.getHeight()));
}

void configureWindow(sf::RenderWindow& window, const Maze& maze, SearchAlgorithm algorithm, Difficulty difficulty) {
    const auto size = window.getSize();
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, static_cast<float>(size.x), static_cast<float>(size.y))));
    updateTitle(window, maze, algorithm, difficulty);
}

int mazeAreaWidth(const sf::RenderWindow& window) {
    return std::max(100, static_cast<int>(window.getSize().x) - PANEL_WIDTH);
}

int cellSizeFor(const sf::RenderWindow& window, const Maze& maze) {
    const int byWidth = mazeAreaWidth(window) / maze.getWidth();
    const int byHeight = static_cast<int>(window.getSize().y) / maze.getHeight();
    return std::max(4, std::min(byWidth, byHeight));
}

float panelX(const sf::RenderWindow& window) {
    return static_cast<float>(window.getSize().x - PANEL_WIDTH);
}

const std::vector<std::string>& glyphFor(char ch) {
    static const std::unordered_map<char, std::vector<std::string>> glyphs = {
        {'A', {"01110", "10001", "10001", "11111", "10001", "10001", "10001"}},
        {'B', {"11110", "10001", "10001", "11110", "10001", "10001", "11110"}},
        {'C', {"01111", "10000", "10000", "10000", "10000", "10000", "01111"}},
        {'D', {"11110", "10001", "10001", "10001", "10001", "10001", "11110"}},
        {'E', {"11111", "10000", "10000", "11110", "10000", "10000", "11111"}},
        {'F', {"11111", "10000", "10000", "11110", "10000", "10000", "10000"}},
        {'G', {"01111", "10000", "10000", "10111", "10001", "10001", "01111"}},
        {'H', {"10001", "10001", "10001", "11111", "10001", "10001", "10001"}},
        {'I', {"11111", "00100", "00100", "00100", "00100", "00100", "11111"}},
        {'J', {"00111", "00010", "00010", "00010", "10010", "10010", "01100"}},
        {'K', {"10001", "10010", "10100", "11000", "10100", "10010", "10001"}},
        {'L', {"10000", "10000", "10000", "10000", "10000", "10000", "11111"}},
        {'M', {"10001", "11011", "10101", "10101", "10001", "10001", "10001"}},
        {'N', {"10001", "11001", "10101", "10011", "10001", "10001", "10001"}},
        {'O', {"01110", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'P', {"11110", "10001", "10001", "11110", "10000", "10000", "10000"}},
        {'Q', {"01110", "10001", "10001", "10001", "10101", "10010", "01101"}},
        {'R', {"11110", "10001", "10001", "11110", "10100", "10010", "10001"}},
        {'S', {"01111", "10000", "10000", "01110", "00001", "00001", "11110"}},
        {'T', {"11111", "00100", "00100", "00100", "00100", "00100", "00100"}},
        {'U', {"10001", "10001", "10001", "10001", "10001", "10001", "01110"}},
        {'V', {"10001", "10001", "10001", "10001", "10001", "01010", "00100"}},
        {'W', {"10001", "10001", "10001", "10101", "10101", "10101", "01010"}},
        {'X', {"10001", "10001", "01010", "00100", "01010", "10001", "10001"}},
        {'Y', {"10001", "10001", "01010", "00100", "00100", "00100", "00100"}},
        {'Z', {"11111", "00001", "00010", "00100", "01000", "10000", "11111"}},
        {'0', {"01110", "10001", "10011", "10101", "11001", "10001", "01110"}},
        {'1', {"00100", "01100", "00100", "00100", "00100", "00100", "01110"}},
        {'2', {"01110", "10001", "00001", "00010", "00100", "01000", "11111"}},
        {'3', {"11110", "00001", "00001", "01110", "00001", "00001", "11110"}},
        {'4', {"00010", "00110", "01010", "10010", "11111", "00010", "00010"}},
        {'5', {"11111", "10000", "10000", "11110", "00001", "00001", "11110"}},
        {'6', {"01110", "10000", "10000", "11110", "10001", "10001", "01110"}},
        {'7', {"11111", "00001", "00010", "00100", "01000", "01000", "01000"}},
        {'8', {"01110", "10001", "10001", "01110", "10001", "10001", "01110"}},
        {'9', {"01110", "10001", "10001", "01111", "00001", "00001", "01110"}},
        {'*', {"00100", "10101", "01110", "11111", "01110", "10101", "00100"}},
        {'+', {"00000", "00100", "00100", "11111", "00100", "00100", "00000"}},
        {'-', {"00000", "00000", "00000", "11111", "00000", "00000", "00000"}},
        {':', {"00000", "00100", "00100", "00000", "00100", "00100", "00000"}},
        {' ', {"00000", "00000", "00000", "00000", "00000", "00000", "00000"}}
    };
    static const std::vector<std::string> unknown = { "11111", "00001", "00010", "00100", "00100", "00000", "00100" };
    auto it = glyphs.find(ch);
    return it == glyphs.end() ? unknown : it->second;
}

void drawText(sf::RenderWindow& window, const std::string& value, int scale, sf::Vector2f position,
    sf::Color color = sf::Color::White) {
    sf::RectangleShape pixel(sf::Vector2f(static_cast<float>(scale), static_cast<float>(scale)));
    pixel.setFillColor(color);
    float cursorX = position.x;
    for (char raw : value) {
        char ch = static_cast<char>(std::toupper(static_cast<unsigned char>(raw)));
        const auto& glyph = glyphFor(ch);
        for (std::size_t row = 0; row < glyph.size(); ++row) {
            for (std::size_t col = 0; col < glyph[row].size(); ++col) {
                if (glyph[row][col] == '1') {
                    pixel.setPosition(cursorX + static_cast<float>(col * scale), position.y + static_cast<float>(row * scale));
                    window.draw(pixel);
                }
            }
        }
        cursorX += 6.f * static_cast<float>(scale);
    }
}

bool containsPoint(const std::vector<Point>& points, const Point& point) {
    return std::find(points.begin(), points.end(), point) != points.end();
}

std::vector<Button> buildButtons(const sf::RenderWindow& window, SearchAlgorithm algorithm,
    Difficulty difficulty, ScreenMode mode) {
    std::vector<Button> buttons;
    const float x = panelX(window) + PANEL_PADDING;
    const float fullWidth = static_cast<float>(PANEL_WIDTH - PANEL_PADDING * 2);
    const float halfWidth = (fullWidth - 12.f) / 2.f;
    const float height = 40.f;
    const float gap = 10.f;
    float y = 26.f;

    auto addAt = [&](float bx, float by, float bw, const std::string& label, ButtonAction action, bool active = false) {
        buttons.push_back({ sf::FloatRect(bx, by, bw, height), label, action, active });
    };
    auto add = [&](const std::string& label, ButtonAction action, bool active = false) {
        buttons.push_back({ sf::FloatRect(x, y, fullWidth, height), label, action, active });
        y += height + gap;
    };

    addAt(x, y, halfWidth, "MAIN", ButtonAction::TabMain, mode == ScreenMode::Main);
    addAt(x + halfWidth + 12.f, y, halfWidth, "HELP", ButtonAction::TabHelp, mode == ScreenMode::Help);
    y += height + 24.f;

    if (mode == ScreenMode::Help) {
        return buttons;
    }

    add("BFS", ButtonAction::SelectBFS, algorithm == SearchAlgorithm::BFS);
    add("DFS", ButtonAction::SelectDFS, algorithm == SearchAlgorithm::DFS);
    add("A*", ButtonAction::SelectAStar, algorithm == SearchAlgorithm::AStar);
    y += 10.f;
    add("EASY", ButtonAction::SelectEasy, difficulty == Difficulty::Easy);
    add("MEDIUM", ButtonAction::SelectMedium, difficulty == Difficulty::Medium);
    add("HARD", ButtonAction::SelectHard, difficulty == Difficulty::Hard);
    y += 10.f;
    addAt(x, y, halfWidth, "SIZE -", ButtonAction::Smaller);
    addAt(x + halfWidth + 12.f, y, halfWidth, "SIZE +", ButtonAction::Larger);
    y += height + gap;
    add("SIZE FIELD", ButtonAction::FocusSize);
    add("APPLY SIZE", ButtonAction::ApplySize);
    y += 10.f;
    add("GENERATE", ButtonAction::Generate);
    add("FIND PATH", ButtonAction::FindPath);
    add("CLEAR PATH", ButtonAction::Clear);
    return buttons;
}

const Button* findClickedButton(const std::vector<Button>& buttons, float x, float y) {
    for (const auto& button : buttons) {
        if (button.bounds.contains(x, y)) return &button;
    }
    return nullptr;
}

void drawButton(sf::RenderWindow& window, const Button& button) {
    sf::RectangleShape shape(sf::Vector2f(button.bounds.width, button.bounds.height));
    shape.setPosition(button.bounds.left, button.bounds.top);
    shape.setFillColor(button.active ? sf::Color(82, 137, 92) : sf::Color(70, 75, 82));
    shape.setOutlineThickness(1.f);
    shape.setOutlineColor(button.active ? sf::Color(160, 220, 168) : sf::Color(105, 112, 122));
    window.draw(shape);
    drawText(window, button.label, 2, { button.bounds.left + 14.f, button.bounds.top + 11.f });
}

void drawHelp(sf::RenderWindow& window) {
    const float x = panelX(window) + PANEL_PADDING;
    float y = 116.f;
    const sf::Color text(220, 224, 230);
    drawText(window, "KEYBOARD", 3, { x, y }); y += 48.f;
    drawText(window, "1 BFS", 2, { x, y }, text); y += 30.f;
    drawText(window, "2 DFS", 2, { x, y }, text); y += 30.f;
    drawText(window, "3 A*", 2, { x, y }, text); y += 30.f;
    drawText(window, "Q EASY", 2, { x, y }, text); y += 30.f;
    drawText(window, "W MEDIUM", 2, { x, y }, text); y += 30.f;
    drawText(window, "E HARD", 2, { x, y }, text); y += 30.f;
    drawText(window, "R GENERATE", 2, { x, y }, text); y += 30.f;
    drawText(window, "SPACE FIND PATH", 2, { x, y }, text); y += 30.f;
    drawText(window, "C CLEAR PATH", 2, { x, y }, text); y += 30.f;
    drawText(window, "+ SIZE PLUS", 2, { x, y }, text); y += 30.f;
    drawText(window, "- SIZE MINUS", 2, { x, y }, text); y += 50.f;
    drawText(window, "MOUSE", 3, { x, y }); y += 48.f;
    drawText(window, "LEFT CLICK START", 2, { x, y }, text); y += 30.f;
    drawText(window, "RIGHT CLICK FINISH", 2, { x, y }, text); y += 30.f;
    drawText(window, "ESC EXIT", 2, { x, y }, text);
}

void drawApp(sf::RenderWindow& window, const Maze& maze, const std::vector<Point>& path,
    const std::vector<Point>& visited, SearchAlgorithm algorithm, Difficulty difficulty,
    ScreenMode mode, const std::string& sizeText, bool sizeInputActive) {
    window.clear(sf::Color(28, 30, 32));
    const int cellSize = cellSizeFor(window, maze);
    const int mazePixelWidth = maze.getWidth() * cellSize;
    const int mazePixelHeight = maze.getHeight() * cellSize;
    const float mazeOffsetX = static_cast<float>((mazeAreaWidth(window) - mazePixelWidth) / 2);
    const float mazeOffsetY = static_cast<float>((static_cast<int>(window.getSize().y) - mazePixelHeight) / 2);

    sf::RectangleShape cell(sf::Vector2f(static_cast<float>(cellSize - 1), static_cast<float>(cellSize - 1)));
    for (int y = 0; y < maze.getHeight(); ++y) {
        for (int x = 0; x < maze.getWidth(); ++x) {
            Point p(x, y);
            cell.setPosition(mazeOffsetX + static_cast<float>(x * cellSize), mazeOffsetY + static_cast<float>(y * cellSize));
            if (maze.isWall(p)) cell.setFillColor(sf::Color::Black);
            else if (p == maze.getStart()) cell.setFillColor(sf::Color::Green);
            else if (p == maze.getEnd()) cell.setFillColor(sf::Color::Red);
            else if (containsPoint(path, p)) cell.setFillColor(sf::Color::Yellow);
            else if (containsPoint(visited, p)) cell.setFillColor(sf::Color(70, 130, 210));
            else cell.setFillColor(sf::Color(180, 180, 180));
            window.draw(cell);
        }
    }

    sf::RectangleShape panel(sf::Vector2f(static_cast<float>(PANEL_WIDTH), static_cast<float>(window.getSize().y)));
    panel.setPosition(panelX(window), 0.f);
    panel.setFillColor(sf::Color(42, 45, 49));
    window.draw(panel);

    const auto buttons = buildButtons(window, algorithm, difficulty, mode);
    for (const auto& button : buttons) {
        if (button.action == ButtonAction::FocusSize) continue;
        drawButton(window, button);
    }

    if (mode == ScreenMode::Help) {
        drawHelp(window);
    }
    else {
        sf::FloatRect inputRect;
        for (const auto& button : buttons) {
            if (button.action == ButtonAction::FocusSize) inputRect = button.bounds;
        }
        sf::RectangleShape input(sf::Vector2f(inputRect.width, inputRect.height));
        input.setPosition(inputRect.left, inputRect.top);
        input.setFillColor(sf::Color(38, 41, 45));
        input.setOutlineThickness(2.f);
        input.setOutlineColor(sizeInputActive ? sf::Color(160, 220, 168) : sf::Color(105, 112, 122));
        window.draw(input);
        drawText(window, "SIZE: " + sizeText, 2, { inputRect.left + 14.f, inputRect.top + 11.f });

        const float infoY = static_cast<float>(window.getSize().y) - 96.f;
        drawText(window, std::to_string(maze.getWidth()) + "x" + std::to_string(maze.getHeight()),
            2, { panelX(window) + PANEL_PADDING, infoY }, sf::Color(210, 214, 220));
        drawText(window, "OPEN HELP FOR KEYS", 2, { panelX(window) + PANEL_PADDING, infoY + 30.f }, sf::Color(210, 214, 220));
    }

    window.display();
}

std::vector<Point> reconstructPath(Point start, Point end,
    const std::unordered_map<Point, Point, PointHash>& cameFrom) {
    if (start == end) return { start };
    if (cameFrom.find(end) == cameFrom.end()) return {};
    std::vector<Point> path;
    for (Point current = end; current != start; current = cameFrom.at(current)) path.push_back(current);
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    return path;
}

int manhattan(Point a, Point b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

SearchTrace traceBFS(const Maze& maze) {
    SearchTrace trace;
    const Point start = maze.getStart();
    const Point end = maze.getEnd();
    if (maze.isWall(start) || maze.isWall(end)) return trace;
    std::queue<Point> frontier;
    std::unordered_set<Point, PointHash> visited;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    frontier.push(start);
    visited.insert(start);
    while (!frontier.empty()) {
        Point current = frontier.front();
        frontier.pop();
        trace.visited.push_back(current);
        if (current == end) break;
        for (const Point& next : maze.getNeighbors(current)) {
            if (visited.find(next) == visited.end()) {
                visited.insert(next);
                cameFrom[next] = current;
                frontier.push(next);
            }
        }
    }
    trace.path = reconstructPath(start, end, cameFrom);
    return trace;
}

SearchTrace traceDFS(const Maze& maze) {
    SearchTrace trace;
    const Point start = maze.getStart();
    const Point end = maze.getEnd();
    if (maze.isWall(start) || maze.isWall(end)) return trace;
    std::stack<Point> frontier;
    std::unordered_set<Point, PointHash> visited;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    frontier.push(start);
    visited.insert(start);
    while (!frontier.empty()) {
        Point current = frontier.top();
        frontier.pop();
        trace.visited.push_back(current);
        if (current == end) break;
        for (const Point& next : maze.getNeighbors(current)) {
            if (visited.find(next) == visited.end()) {
                visited.insert(next);
                cameFrom[next] = current;
                frontier.push(next);
            }
        }
    }
    trace.path = reconstructPath(start, end, cameFrom);
    return trace;
}

SearchTrace traceAStar(const Maze& maze) {
    struct Node {
        Point point;
        int priority;
        bool operator>(const Node& other) const { return priority > other.priority; }
    };
    SearchTrace trace;
    const Point start = maze.getStart();
    const Point end = maze.getEnd();
    if (maze.isWall(start) || maze.isWall(end)) return trace;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> frontier;
    std::unordered_map<Point, Point, PointHash> cameFrom;
    std::unordered_map<Point, int, PointHash> costSoFar;
    std::unordered_set<Point, PointHash> drawn;
    frontier.push({ start, 0 });
    costSoFar[start] = 0;
    while (!frontier.empty()) {
        Point current = frontier.top().point;
        frontier.pop();
        if (drawn.insert(current).second) trace.visited.push_back(current);
        if (current == end) break;
        for (const Point& next : maze.getNeighbors(current)) {
            int newCost = costSoFar[current] + 1;
            auto knownCost = costSoFar.find(next);
            if (knownCost == costSoFar.end() || newCost < knownCost->second) {
                costSoFar[next] = newCost;
                cameFrom[next] = current;
                frontier.push({ next, newCost + manhattan(next, end) });
            }
        }
    }
    trace.path = reconstructPath(start, end, cameFrom);
    return trace;
}

SearchTrace traceSearch(const Maze& maze, SearchAlgorithm algorithm) {
    switch (algorithm) {
    case SearchAlgorithm::DFS: return traceDFS(maze);
    case SearchAlgorithm::AStar: return traceAStar(maze);
    case SearchAlgorithm::BFS:
    default: return traceBFS(maze);
    }
}

bool animateSearch(sf::RenderWindow& window, const Maze& maze, SearchAlgorithm algorithm,
    Difficulty difficulty, ScreenMode mode, const std::string& sizeText,
    bool sizeInputActive, std::vector<Point>& path, std::vector<Point>& visited) {
    SearchTrace trace = traceSearch(maze, algorithm);
    path.clear();
    visited.clear();
    const std::size_t step = std::max<std::size_t>(1, trace.visited.size() / 180);
    for (std::size_t i = 0; i < trace.visited.size() && window.isOpen(); i += step) {
        visited.assign(trace.visited.begin(), trace.visited.begin() + static_cast<std::ptrdiff_t>(i + 1));
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
                return false;
            }
        }
        drawApp(window, maze, path, visited, algorithm, difficulty, mode, sizeText, sizeInputActive);
        std::this_thread::sleep_for(std::chrono::milliseconds(6));
    }
    visited = trace.visited;
    path = trace.path;
    drawApp(window, maze, path, visited, algorithm, difficulty, mode, sizeText, sizeInputActive);
    return !path.empty();
}

bool applySizeText(int& size, Maze& maze, Generator& gen, Difficulty difficulty,
    const std::string& sizeText, std::vector<Point>& path, std::vector<Point>& visited) {
    if (sizeText.empty()) return false;
    int requested = std::clamp(std::stoi(sizeText), MIN_SIZE, MAX_SIZE);
    size = requested;
    maze.setSize(size, size);
    gen.generatePerfectMaze(maze, difficulty);
    path.clear();
    visited.clear();
    return true;
}

bool applyButtonAction(ButtonAction action, int& size, Maze& maze, Generator& gen,
    Solver& solver, SearchAlgorithm& algorithm, Difficulty& difficulty,
    std::vector<Point>& path, std::vector<Point>& visited, sf::RenderWindow& window,
    ScreenMode& mode, std::string& sizeText, bool& sizeInputActive) {
    switch (action) {
    case ButtonAction::TabMain:
        mode = ScreenMode::Main;
        sizeInputActive = false;
        break;
    case ButtonAction::TabHelp:
        mode = ScreenMode::Help;
        sizeInputActive = false;
        break;
    case ButtonAction::SelectBFS:
        algorithm = SearchAlgorithm::BFS;
        solver.setStrategy(makeStrategy(algorithm));
        path.clear(); visited.clear();
        break;
    case ButtonAction::SelectDFS:
        algorithm = SearchAlgorithm::DFS;
        solver.setStrategy(makeStrategy(algorithm));
        path.clear(); visited.clear();
        break;
    case ButtonAction::SelectAStar:
        algorithm = SearchAlgorithm::AStar;
        solver.setStrategy(makeStrategy(algorithm));
        path.clear(); visited.clear();
        break;
    case ButtonAction::SelectEasy:
        difficulty = Difficulty::Easy;
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::SelectMedium:
        difficulty = Difficulty::Medium;
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::SelectHard:
        difficulty = Difficulty::Hard;
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::Smaller:
        size = std::max(MIN_SIZE, size - 1);
        sizeText = std::to_string(size);
        maze.setSize(size, size);
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::Larger:
        size = std::min(MAX_SIZE, size + 1);
        sizeText = std::to_string(size);
        maze.setSize(size, size);
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::FocusSize:
        sizeInputActive = true;
        break;
    case ButtonAction::ApplySize:
        applySizeText(size, maze, gen, difficulty, sizeText, path, visited);
        sizeText = std::to_string(size);
        sizeInputActive = false;
        break;
    case ButtonAction::Generate:
        gen.generatePerfectMaze(maze, difficulty);
        path.clear(); visited.clear();
        break;
    case ButtonAction::FindPath:
        return false;
    case ButtonAction::Clear:
        path.clear(); visited.clear();
        break;
    }
    configureWindow(window, maze, algorithm, difficulty);
    return true;
}

int main(int argc, char** argv) {
    const bool windowedMode = argc >= 2 && std::string(argv[1]) == "--windowed";

    int size = 21;
    std::string sizeText = std::to_string(size);
    bool sizeInputActive = false;
    ScreenMode mode = ScreenMode::Main;
    Maze maze(size, size);
    Generator gen;
    Difficulty difficulty = Difficulty::Medium;
    gen.generatePerfectMaze(maze, difficulty);
    SearchAlgorithm algorithm = SearchAlgorithm::BFS;
    Solver solver;
    solver.setStrategy(makeStrategy(algorithm));
    std::vector<Point> path;
    std::vector<Point> visited;

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::VideoMode videoMode = windowedMode ? sf::VideoMode(1180, 700, 32) : desktop;
    sf::Uint32 style = windowedMode ? sf::Style::Default : sf::Style::Fullscreen;
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 2;
    settings.minorVersion = 1;
    sf::RenderWindow window(videoMode, "Maze Explorer", style, settings);
    configureWindow(window, maze, algorithm, difficulty);
    drawApp(window, maze, path, visited, algorithm, difficulty, mode, sizeText, sizeInputActive);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            bool redraw = false;
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::TextEntered && sizeInputActive) {
                if (event.text.unicode >= '0' && event.text.unicode <= '9' && sizeText.size() < 2) {
                    sizeText.push_back(static_cast<char>(event.text.unicode));
                    redraw = true;
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) window.close();
                else if (sizeInputActive && event.key.code == sf::Keyboard::BackSpace) {
                    if (!sizeText.empty()) sizeText.pop_back();
                    redraw = true;
                }
                else if (sizeInputActive && event.key.code == sf::Keyboard::Enter) {
                    applySizeText(size, maze, gen, difficulty, sizeText, path, visited);
                    sizeText = std::to_string(size);
                    sizeInputActive = false;
                    redraw = true;
                }
                else if (event.key.code == sf::Keyboard::F1) {
                    mode = (mode == ScreenMode::Main) ? ScreenMode::Help : ScreenMode::Main;
                    sizeInputActive = false;
                    redraw = true;
                }
                else if (event.key.code == sf::Keyboard::Num1) {
                    algorithm = SearchAlgorithm::BFS; solver.setStrategy(makeStrategy(algorithm));
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::Num2) {
                    algorithm = SearchAlgorithm::DFS; solver.setStrategy(makeStrategy(algorithm));
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::Num3) {
                    algorithm = SearchAlgorithm::AStar; solver.setStrategy(makeStrategy(algorithm));
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::E) {
                    difficulty = event.key.code == sf::Keyboard::Q ? Difficulty::Easy :
                        event.key.code == sf::Keyboard::E ? Difficulty::Hard : Difficulty::Medium;
                    gen.generatePerfectMaze(maze, difficulty);
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::R) {
                    gen.generatePerfectMaze(maze, difficulty);
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::C) {
                    path.clear(); visited.clear(); redraw = true;
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    animateSearch(window, maze, algorithm, difficulty, mode, sizeText, sizeInputActive, path, visited);
                }
                else if (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal ||
                    event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::Hyphen) {
                    size += (event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Equal) ? 1 : -1;
                    size = std::clamp(size, MIN_SIZE, MAX_SIZE);
                    sizeText = std::to_string(size);
                    maze.setSize(size, size);
                    gen.generatePerfectMaze(maze, difficulty);
                    path.clear(); visited.clear(); redraw = true;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
                const float mx = static_cast<float>(event.mouseButton.x);
                const float my = static_cast<float>(event.mouseButton.y);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    auto buttons = buildButtons(window, algorithm, difficulty, mode);
                    if (const Button* button = findClickedButton(buttons, mx, my)) {
                        if (button->action == ButtonAction::FindPath) {
                            animateSearch(window, maze, algorithm, difficulty, mode, sizeText, sizeInputActive, path, visited);
                        }
                        else {
                            applyButtonAction(button->action, size, maze, gen, solver, algorithm, difficulty,
                                path, visited, window, mode, sizeText, sizeInputActive);
                            redraw = true;
                        }
                    }
                    else if (mx < static_cast<float>(mazeAreaWidth(window))) {
                        const int cellSize = cellSizeFor(window, maze);
                        const int mazePixelWidth = maze.getWidth() * cellSize;
                        const int mazePixelHeight = maze.getHeight() * cellSize;
                        const int ox = (mazeAreaWidth(window) - mazePixelWidth) / 2;
                        const int oy = (static_cast<int>(window.getSize().y) - mazePixelHeight) / 2;
                        int cx = (event.mouseButton.x - ox) / cellSize;
                        int cy = (event.mouseButton.y - oy) / cellSize;
                        if (cx >= 0 && cx < maze.getWidth() && cy >= 0 && cy < maze.getHeight()) {
                            Point cell(cx, cy);
                            maze.setWall(cell, false);
                            maze.setStart(cell);
                            path.clear(); visited.clear(); redraw = true;
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right && mx < static_cast<float>(mazeAreaWidth(window))) {
                    const int cellSize = cellSizeFor(window, maze);
                    const int mazePixelWidth = maze.getWidth() * cellSize;
                    const int mazePixelHeight = maze.getHeight() * cellSize;
                    const int ox = (mazeAreaWidth(window) - mazePixelWidth) / 2;
                    const int oy = (static_cast<int>(window.getSize().y) - mazePixelHeight) / 2;
                    int cx = (event.mouseButton.x - ox) / cellSize;
                    int cy = (event.mouseButton.y - oy) / cellSize;
                    if (cx >= 0 && cx < maze.getWidth() && cy >= 0 && cy < maze.getHeight()) {
                        Point cell(cx, cy);
                        maze.setWall(cell, false);
                        maze.setEnd(cell);
                        path.clear(); visited.clear(); redraw = true;
                    }
                }
            }
            if (redraw) {
                configureWindow(window, maze, algorithm, difficulty);
                drawApp(window, maze, path, visited, algorithm, difficulty, mode, sizeText, sizeInputActive);
            }
        }
    }
    return 0;
}
