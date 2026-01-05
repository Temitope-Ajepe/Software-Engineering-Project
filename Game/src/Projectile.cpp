#include "../include/Projectile.h"
#include <cmath>

//This is the Constructor that initializes the projectile's look and calculates its travel path
Projectile::Projectile(sf::Vector2f startPos, sf::Vector2f targetPos, float damage)
    : targetPos(targetPos), damage(damage)
{
    //This is the Visual Setup that define size, colors, outline and create a small yellow bullet 
    shape.setRadius(4.f);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOutlineColor(sf::Color(255, 165, 0)); 
    shape.setOutlineThickness(1.f);
    shape.setOrigin({ 4.f, 4.f }); //This center the circle
    shape.setPosition(startPos);

    // The Maths section
    //We get the vector pointing from start to target
    sf::Vector2f direction = targetPos - startPos;
    // Then, we calculate the distance
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // We normalize the vector and apply speed
    if (length > 0) {
        velocity = (direction / length) * speed;
    }
}

// This is the logic that move the projectile and checks if it has arrived
void Projectile::update() {
    // If the projectile already hit, stop moving
    if (reachedTarget) return;

    // We then apply the velocity to the current position
    shape.move(velocity);

    // Also, we check if we've reached the target
    sf::Vector2f pos = shape.getPosition();
    float dx = targetPos.x - pos.x;
    float dy = targetPos.y - pos.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance < speed) {
        reachedTarget = true;
    }
}

// This draws the projectile's shape on the screen
void Projectile::draw(sf::RenderWindow& window) {
    window.draw(shape);
}