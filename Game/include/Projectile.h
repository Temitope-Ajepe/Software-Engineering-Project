#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Projectile {
public:
    // This is the Constructor that Sets the starting point, the destination, and damage value
    Projectile(sf::Vector2f startPos, sf::Vector2f targetPos, float damage);

    // This moves the projectile toward the target every frame
    void update();
    // This renders the projectile shape to the game window
    void draw(sf::RenderWindow& window);

    // This are public functions to safely access the private data
    bool hasReachedTarget() const { return reachedTarget; } // To check if projectile is done
    // This get the current x,y coordinates
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    float getDamage() const { return damage; } // This get the damage value

private:
    sf::CircleShape shape; // This is the visual representation of the projectile
    sf::Vector2f velocity; // The direction and magnitude of the movement
    sf::Vector2f targetPos;  // The coordinate in which the projectile is heading towards
    float speed = 5.0f;   // The constant travel speed
    float damage;           // The amount of damage to deal on impact
    bool reachedTarget = false;  // This tracks if the projectile has finished its journey
};