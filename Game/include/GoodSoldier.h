#pragma once
#include <SFML/Graphics.hpp>
#include "../include/BadSoldier.h"
#include "../include/Projectile.h"

class GoodBoy {
public:
    // Constructor: sets starting position
    GoodBoy(float startX, float startY, float targetX, float targetY);

    // Update logic (movement, AI, etc.)
    void update(std::vector<std::unique_ptr<BadBoy>>& enemies, 
        std::vector<std::unique_ptr<Projectile>>& projectiles);

    // Draw the sprite on the window
    void draw(sf::RenderWindow& window);

  
   

private:
    sf::Texture texture;  // sprite texture
    std::unique_ptr<sf::Sprite> sprite;          // pointer to sprite

    float attackRange = 120.f;
    float damage = 35.f;
    sf::Clock attackClock;

    float stopY;


    bool reachedPosition = false;
    sf::Vector2f targetPosition;


};

