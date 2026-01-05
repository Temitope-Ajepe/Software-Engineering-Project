#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp> 
#include <memory>
#include "../include/GoodSoldier.h"
#include "../include/BadSoldier.h"
#include <vector>
#include "../include/Projectile.h"

// This class manages the entire game: window, objects, logic, and rendering
class Game
{ 
public:     //Can be accessed from outside the class
    Game();      // Constructor Called when creating a Game object (sets up the game)
    void run();     // Main game loop Runs continuously until window closes

private:  //Can only be accessed from inside the class
    void processEvents();
    void update();
    void render();

    sf::RenderWindow window;
    sf::Texture backgroundTexture;
    std::unique_ptr<sf::Sprite> backgroundSprite;


    //To add road for the GoodSoldier
	sf::RectangleShape road; //vertical path
	sf::RectangleShape enemyPath; //horizontal path
    sf::Clock enemySpawnClock;
    float enemySpawnInterval = 2.0f; // spawn every 2 seconds


    sf::Texture resourceTexture;
    std::unique_ptr<sf::Sprite> resourceSprite;  // pointer to sprite
    std::vector<std::unique_ptr<BadBoy>> badBoys;
    std::vector<std::unique_ptr<GoodBoy>> goodBoys;
    std::vector<std::unique_ptr<Projectile>> projectiles;


   
    //WAVE SYSTEM
    int currentWave = 1;    // Current wave number (starts at 1)
    int enemiesToSpawn = 5;  // How many enemies to spawn this wave 
    int enemiesSpawned = 0;
    bool waveActive = true;

    sf::Font font;  
    int resourceHealth = 100;
    bool gameOver = false;

    static constexpr int GRID_SIZE = 40; // Each grid cell is 40x40 pixels
    static constexpr int COLS = 800 / GRID_SIZE;    // Number of columns: 800÷40 = 20 columns
    static constexpr int ROWS = 600 / GRID_SIZE;    // Number of rows: 600÷40 = 15 rows
                                                    // 2D grid (15 rows × 20 columns)
                                                      // 0 = walkable, 1 = blocked
                                                      // Enemies use this to navigate to the resource

    std::vector<std::vector<int>> grid;


    bool showWaveTransition = false;
    sf::Clock waveTransitionClock;

    int waveGoal = 8;  // Win after surviving 8 waves
    bool gameWon = false;
    
    

};

