#include "../include/GoodSoldier.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "../include/BadSoldier.h"

//Constructor Called when creating a new GoodBoy soldier
//   startX, startY: Where the soldier spawns (usually off screen)
//   targetX, targetY: Where the soldier will move to (defensive position)
GoodBoy::GoodBoy(float startX, float startY, float targetX, float targetY)
{

    // Load texture
    if (!texture.loadFromFile("assets/GoodSoldier.png")) {
        std::cerr << "Failed to load soldier.png! Using fallback.\n";   // If loading fails, print an error message to console

        // fallback simple blue rectangle
        sf::Image img({ 40, 40 }, sf::Color::Blue);

        if(!texture.loadFromImage(img)) {
            std::cerr << "Failed to create fallback texture!\n"; // Print error if this also fails
        }
    }

    // Create a sprite object that uses the loaded texture
   // unique_ptr automatically manages memory
    sprite = std::make_unique<sf::Sprite>(texture);

    // Scale down the soldier to 15% of original size (makes it fit the game better) 
    sprite->setScale(sf::Vector2f(0.15f, 0.15f));

    // Center the origin so they shoot from their middle
    // Get the sprite's dimensions (width and height)
    sf::FloatRect bounds = sprite->getLocalBounds();
    // We set the origin to the center of the sprite
    // to also ensures bullets shoot from the middle of the soldier
    sprite->setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });

    // Set starting position using sf::Vector2f
    sprite->setPosition(sf::Vector2f(startX, startY));
    // Store where the soldier should move to (their defensive position)
    targetPosition = sf::Vector2f(targetX, targetY);

}

// Updates call every frame to update soldier behavior
//   enemies, list of all enemy soldiers to potentially attack
//   projectiles, list where new bullets will be added
void GoodBoy::update(std::vector<std::unique_ptr<BadBoy>>& enemies, 
    std::vector<std::unique_ptr<Projectile>>& projectiles) {
    float speed = 1.5f;
    // Get the soldier's current position
    sf::Vector2f pos = sprite->getPosition();

    // Move to assigned position first
    if (!reachedPosition) {
        float speed = 2.0f;  // Movement speed 
        // Calculate direction vector from current position to target
        sf::Vector2f direction = targetPosition - pos;

        // Calculate distance to target using Pythagorean theorem
        // distance = ?(x² + y²)
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        // Check if soldier is close enough to target within 2 pixels
        if (distance < 2.f) {
            // Snap to exact target position
            sprite->setPosition(targetPosition);
            reachedPosition = true;  // Mark as arrived at position
        }
        else {
            // Still moving toward target
            // Normalize direction vector (divide by distance to get unit vector)
            // Then multiply by speed to get velocity
            sprite->move(direction / distance * speed);
        }
        return; // do not attack until positioned
    }
    
    //  TARGETING PHASE 
    // At this point, soldier has reached position and is ready to fight
// Find closest enemy in range and shoot projectile
    BadBoy* closestEnemy = nullptr; // Pointer to closest enemy (null if none found)
    float closestDistance = attackRange;   // Start with max attack range (soldiers can't shoot beyond this)

    // Loop through all enemies to find the closest one in range
    for (auto& enemy : enemies) {
        // Skip dead enemies (can't attack corpses)
        if (enemy->isDead()) continue;

        // Calculate distance to this enemy using Pythagorean theorem
        float dx = enemy->getPosition().x - pos.x;  // Horizontal distance
        float dy = enemy->getPosition().y - pos.y;  // Vertical distance
        float distance = std::sqrt(dx * dx + dy * dy); // Actual distance

        // Check if enemy is within attack range AND closer than current closest
        if (distance <= attackRange && distance < closestDistance) {
            closestEnemy = enemy.get(); // Remember this enemy
            closestDistance = distance;  // Update closest distance
        }
    }

    // Check if we find an enemy to attack
    if (closestEnemy != nullptr) {
        sprite->setColor(sf::Color::Red); // Enemy in range, change color to red to show attacking state

        if (attackClock.getElapsedTime().asSeconds() >= 0.8f) {  // Check if enough time has passed since last shot (0.8 seconds = fire rate)
            // Time to shoot
            // Create projectile from soldier to enemy
            auto projectile = std::make_unique<Projectile>(
                pos, // Bullet starts at soldier's position
                closestEnemy->getPosition(),  // Bullet flies toward enemy's position
                damage  // Bullet deals this much damage on hit
            );
            // Add the bullet to the game's projectile list
            projectiles.push_back(std::move(projectile));

            // Reset the attack timer (start counting down to next shot)
            attackClock.restart();
        }
    }
    else {
        // No enemy in range, return to normal color (white)
        sprite->setColor(sf::Color::White);
    }
}



// Draw soldier on the window
void GoodBoy::draw(sf::RenderWindow& window) {
    window.draw(*sprite);
}

