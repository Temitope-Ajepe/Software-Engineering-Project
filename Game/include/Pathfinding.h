// Pathfinding.h
#pragma once
#include <SFML/System.hpp>
#include <vector>

struct Node {
    sf::Vector2i pos;
    float g = 0, h = 0;
    Node* parent = nullptr; // We will use this only within the scope of the findPath function

    float f() const { return g + h; }
};

class Pathfinding {
public:
    static std::vector<sf::Vector2f> findPath(
        const std::vector<std::vector<int>>& grid,
        sf::Vector2i start,
        sf::Vector2i goal,
        int gridSize
    );
};
/* #pragma once
#include <SFML/System.hpp>
#include <vector>

struct Node {
    sf::Vector2i pos;
    float g, h;
    Node* parent;

    float f() const { return g + h; }
};

class Pathfinding {
public:
    static std::vector<sf::Vector2f> findPath(
        const std::vector<std::vector<int>>& grid,
        sf::Vector2i start,
        sf::Vector2i goal,
        int gridSize
    );
};
*/