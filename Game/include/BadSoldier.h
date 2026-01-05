#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class BadBoy {
public:
    BadBoy(float x, float y, float startHealth = 100.f);


    bool update(); // Returns true if just stole

    //void update();
    void draw(sf::RenderWindow& window);

    void takeDamage(float amount);
    bool isDead() const;
    float getHealth() const { return health; }

    sf::Vector2f getPosition() const;

    void showHitEffect() {
        hitRecently = true;
        hitTimer.restart();
    }
    bool isHit() const { return hitRecently; }

    std::vector<sf::Vector2f> path;
    int currentPathIndex = 0;

private:
    sf::Texture texture;
    std::unique_ptr<sf::Sprite> sprite;

    bool hasStolen = false;  //  has reached resource
    sf::Clock stealTimer;   //  used to pause after stealing

    float health = 100.f;

    bool hitRecently = false;
    sf::Clock hitTimer;

    bool moveVertical = false;
    bool exiting = false;        // after stealing


    //Sprite sheet animation
    sf::IntRect frameRect;      // The window showing one pose
    int currentFrame = 0;       // Current frame index (0, 1, 2.)
    sf::Clock animationClock;   // Controls animation speed

    // Check image, If it has 4 poses, set these
    const int frameWidth = 225;  // Width of ONE pose in pixels
    const int frameHeight = 225; // Height of ONE pose in pixels
    const int totalFrames = 1;  // Number of poses in your strip
};