#include "../include/BadSoldier.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../include/Pathfinding.h"
#include <memory>


BadBoy::BadBoy(float x, float y, float startHealth) {

    if (!texture.loadFromFile("assets/BadSoldier.png")) {
        std::cerr << "Failed to load BadSoldier.png!" << std::endl;
    }

    sprite = std::make_unique<sf::Sprite>(texture);

   
    // This creates a 64x64 pixel window on the first pose
    frameRect = sf::IntRect({ 0, 0 }, { 225, 225 });
    sprite->setTextureRect(frameRect);

    // Set the origin to the center of the FRAME (32, 32) instead of the whole image

     sprite->setOrigin({ 112.5f, 112.5f });
    

    // Set scale of the bad soilder 
    sprite->setScale(sf::Vector2f(0.18f, 0.18f));

    // Set starting position using sf::Vector2f
    sprite->setPosition(sf::Vector2f(x, y));

	health = startHealth;

    if (y < 0.f) {
        moveVertical = true;
    }
    

}


bool BadBoy::update() {
    //float speed = 0.6f;
    // Slower speed for vertical enemies, normal for horizontal
    float speed = moveVertical ? 0.3f : 0.6f;

    if (!hasStolen) {
        // Follow the A* Path
        if (!path.empty() && currentPathIndex < path.size()) {
            sf::Vector2f target = path[currentPathIndex];
            sf::Vector2f pos = sprite->getPosition();

            sf::Vector2f dir = target - pos;
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist < 3.f) {
                currentPathIndex++; // Move to next node
            }
            else {
                sprite->move((dir / dist) * speed);
            }
        }
        // If we reached the end of the path, we are at the diamond
        else if (!path.empty() && currentPathIndex >= path.size()) {
            hasStolen = true;
            stealTimer.restart();
            std::cout << "Resource stolen!\n";
            return true; // Return true when stealing
        }
    }
    else if (stealTimer.getElapsedTime().asSeconds() < 1.0f) {
        return false; // Stand still while "stealing"
    }
    else {
        // FIX: Check if vertical or horizontal to decide escape direction
        if (moveVertical) {
            sprite->move({ 0.f, speed * 0.5f }); // Top enemies continue DOWN off-screen
        }
        else {
            // Escape: move right off screen
            sprite->move({ speed * 0.5f, 0.f });
        }
    }
    if (hitRecently && hitTimer.getElapsedTime().asSeconds() > 0.4f) {
        hitRecently = false;
    }


    // --- Animation Logic ---
    if (animationClock.getElapsedTime().asSeconds() > 0.15f) {
        currentFrame = (currentFrame + 1) % totalFrames; // Loop 0, 1, 2, 3...

        // Slide the window to the next frame
        frameRect.position.x = currentFrame * frameWidth;
        frameRect.position.y = 0; // Staying on the top row
        sprite->setTextureRect(frameRect);

        animationClock.restart();
    }
    return false; // ADD THIS - Return false if not stealing this frame
}



void BadBoy::draw(sf::RenderWindow& window) {
    window.draw(*sprite);
}

void BadBoy::takeDamage(float amount) {
    health -= amount;

    if (health < 0.f)
        health = 0.f;

    showHitEffect();
}

bool BadBoy::isDead() const {
    return health <= 0.f;
}

sf::Vector2f BadBoy::getPosition() const {
    return sprite->getPosition();
}
