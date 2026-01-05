#include "../include/Pathfinding.h"
#include <algorithm>
#include <cmath>

static float heuristic(sf::Vector2i a, sf::Vector2i b) {
    // Using Manhattan distance
    return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
}

std::vector<sf::Vector2f> Pathfinding::findPath(
    const std::vector<std::vector<int>>& grid,
    sf::Vector2i start,
    sf::Vector2i goal,
    int gridSize)
{
    // A pool to store all nodes created during this search. 
    // This handles memory cleanup automatically.
    std::vector<Node*> allNodes;

    auto createNode = [&](sf::Vector2i pos, float g, float h, Node* parent) {
        Node* n = new Node{ pos, g, h, parent };
        allNodes.push_back(n);
        return n;
        };

    std::vector<Node*> open;
    std::vector<Node*> closed;

    open.push_back(createNode(start, 0, heuristic(start, goal), nullptr));

    while (!open.empty()) {
        // Find node with lowest F cost
        auto currentIt = std::min_element(open.begin(), open.end(),
            [](Node* a, Node* b) { return a->f() < b->f(); });

        Node* current = *currentIt;

        // Check if reached goal
        if (current->pos == goal) {
            std::vector<sf::Vector2f> path;
            while (current) {
                float offsetX = 22.5f; // Default 20.f
                float offsetY = 22.5f; // Default 20.f

                path.push_back({
                    current->pos.x * (float)gridSize + offsetX,
                    current->pos.y * (float)gridSize + offsetY
                    });
                current = current->parent;
            }
            std::reverse(path.begin(), path.end());

            // CLEANUP before returning
            for (Node* n : allNodes) delete n;
            return path;
        }

        // Move current from open to closed
        open.erase(currentIt);
        closed.push_back(current);

        sf::Vector2i neighbors[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

        for (auto& dir : neighbors) {
            sf::Vector2i next = current->pos + dir;

            // Bounds check
            if (next.x < 0 || next.y < 0 || next.y >= (int)grid.size() || next.x >= (int)grid[0].size())
                continue;

            // Obstacle check (Don't block the goal!)
            if (grid[next.y][next.x] == 1 && next != goal)
                continue;

            // If already in closed, skip
            if (std::any_of(closed.begin(), closed.end(), [&](Node* n) { return n->pos == next; }))
                continue;

            float newG = current->g + 1;

            // Check if already in open
            auto openIt = std::find_if(open.begin(), open.end(), [&](Node* n) { return n->pos == next; });

            if (openIt == open.end()) {
                open.push_back(createNode(next, newG, heuristic(next, goal), current));
            }
            else if (newG < (*openIt)->g) {
                (*openIt)->g = newG;
                (*openIt)->parent = current;
            }
        }
    }

    // CLEANUP if no path found
    for (Node* n : allNodes) delete n;
    return {};
}
