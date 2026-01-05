#include "../include/Game.h"
#include "../include/GoodSoldier.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp> 
#include <iostream>
#include "../include/Pathfinding.h"
#include "../include/Grid.h"
#include <algorithm>


// CONSTRUCTOR, this is Called when a Game object is created
Game::Game()
// Initialize window with 800x600 resolution
    : window(sf::VideoMode({ 800, 600 }), "Tower Clash")
{
    // Load texture and set up background
    if (!backgroundTexture.loadFromFile("assets/grass.jpg")) {
        std::cerr << "Failed to load assets/grass.jpg\n"; // Print error if loading fails
    }
    // Create a sprite using the loaded texture
    backgroundSprite = std::make_unique<sf::Sprite>(backgroundTexture);

    // Load the font file from Windows fonts directory
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font!\n"; // Print error if font loading fails
    }


    // Scale the background to fit the entire window 
    sf::Vector2u bgSize = backgroundTexture.getSize(); // Get original texture dimensions
    if (bgSize.x > 0 && bgSize.y > 0) {  // we check if texture has valid size
        backgroundSprite->setScale({
            800.f / static_cast<float>(bgSize.x), // Scale width to 800 pixels
            600.f / static_cast<float>(bgSize.y)  // Scale height to 600 pixels
            });
    }

    


    // Vertical road (path for enemies from top)
    road.setSize(sf::Vector2f(45.f, 600.f));   // Width 45 pixels, Height full window (600)
    road.setFillColor(sf::Color(50, 50, 50));  // Dark gray color
    road.setOrigin({ 22.5f, 0.f });    // Center the origin horizontally (half of 45)
    road.setPosition({ 420.f, 0 });  // Position at horizontal center, starting from top
    road.setOutlineColor(sf::Color::White);    // White border for visibilit
    road.setOutlineThickness(1.f);  // 1 pixel thick border


    // Horizontal road (path for enemies from Left)
    enemyPath.setSize(sf::Vector2f(900.f, 45.f)); // Width 800 , Height 45 pixels
    enemyPath.setFillColor(sf::Color(50, 50, 50));  // Dark gray color
    enemyPath.setOrigin({45.f, 0.f });  // Set origin for positioning
    enemyPath.setPosition({ 0.f, 300.f - 22.5f }); // Vertically centered (300 - half of 45)


    // Load the resource image
    if (!resourceTexture.loadFromFile("assets/resource.png")) {
        std::cerr << "Failed to load assets/resource.png\n";  // Print error if loading fails
    }
    // Create sprite from texture
    resourceSprite = std::make_unique<sf::Sprite>(resourceTexture);

    // Center origin so it stays in the middle
    sf::Vector2u size = resourceTexture.getSize();
    resourceSprite->setOrigin(sf::Vector2f(size.x / 2.f, size.y / 2.f));

    // Position the resource in the center of the window
    resourceSprite->setPosition(sf::Vector2f(420.f, 300.f));

    // Scale down the resource to 13% of its original size
    resourceSprite->setScale({ 0.13f, 0.13f });

// Vertical path defenders

     // First defender, Left side of vertical path, positioned high up
    goodBoys.push_back(std::make_unique<GoodBoy>(
        420.f, -50.f,   // spawn from top center
        320.f, 120.f    // position is at left of vertical path
    ));

    // Second defender,Right side of vertical path, positioned slightly lower
    goodBoys.push_back(std::make_unique<GoodBoy>(
        420.f, -50.f,   // spawn from top center 
        520.f, 210.f    // position is at right of vertical path
    )); 

    // Horizontal path defenders
   
    // First horizontal defender, Early intercept position on horizontal path
    goodBoys.push_back(std::make_unique<GoodBoy>(
        -50.f, 200.f,   // spawn from LEFT
        180.f, 265.f    // position is at early intercept on horizontal path
    ));

    // Second horizontal defender, Mid-point intercept on horizontal path
    goodBoys.push_back(std::make_unique<GoodBoy>(
        -50.f, 400.f,   // spawn from LEFT
        320.f, 265.f    // position is at mid intercept on horizontal path
    )); 

    // Print initial soldier count to console for debugging
    std::cout << "Starting with " << goodBoys.size() << " soldiers.\n";

	// The Grid setup for Pathfinding //
    
    grid.resize(ROWS, std::vector<int>(COLS, 0)); // Initialize 2D grid with all cells set to 0 (walkable)

    // Calculate grid position of the resource
    int resourceCol = 400 / GRID_SIZE; // This will convert pixel X to grid column
	int resourceRow = 300 / GRID_SIZE; // This will convert pixel Y to grid row
    grid[resourceRow][resourceCol] = 1;
   

    //spawn first enemy
    float spawnX = 0.f; // Start at left edge of screen
    float spawnY = 300.f; // Vertical center on horizontal path

    //auto enemy = std::make_unique<BadBoy>(spawnX, spawnY);
	float healthBoost = 1.0f + (currentWave - 1) * 0.2f; // 20% more health per wave
    // Calculate enemy health with wave multiplier (increases 10% per wave)
	float enemyHealth = 100.f * (1.0f + (currentWave - 1) * 0.1f); // 20% more health per wave
    // Create enemy with calculated health
    auto enemy = std::make_unique<BadBoy>(spawnX, spawnY, enemyHealth);

    // Convert spawn position from world coordinates to grid coordinates
    sf::Vector2i start(
        spawnX / GRID_SIZE,     // Grid column
        spawnY / GRID_SIZE      // Grid row
    );
    // Set goal position from the resource location in grid coordinates
    sf::Vector2i goal(
        400 / GRID_SIZE,    // Resource column
        300 / GRID_SIZE     // Resource row
    );

    // Generate A* pathfinding route from spawn to resource
    enemy->path = Pathfinding::findPath(grid, start, goal, GRID_SIZE);
    badBoys.push_back(std::move(enemy)); // Add enemy to the list of active enemies
    grid[resourceRow][resourceCol] = 1; // This marks resource cell as blocked in the grid
}

// This is the main game loop that keeps the game running until window closes
void Game::run() {
    while (window.isOpen()) {
        processEvents();    // Handle user input (keyboard, mouse, window events)
        update();           // Update game logic (movement, collisions, spawning)
        render();           // Draw everything to the screen
    }
}
// This handle all user input and window events
void Game::processEvents() 
{
    // Poll for events (keyboard presses, mouse clicks, window close)
    while (const std::optional<sf::Event> event = window.pollEvent())

    {
        // Check if user clicked the X button to close window
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
        // Check if user pressed a key
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            // If ESC key was pressed, close the window
            if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
            {
                window.close();
            }
        }
    }
}

//Main game logic that calls every frame
void Game::update() {

    // GAME STATE CHECK
    // Stop updating if game has ended (win or lose)
    if (gameOver || gameWon) {
        return;  // Exit function immediately, freeze game state
    }

    // UPDATE FRIENDLY SOLDIERS 
    // Loop through all friendly soldiers and update their behavior
    for (auto& g : goodBoys)
        g->update(badBoys, projectiles); // Each soldier targets enemies and shoots

//  Update all projectiles
    // Move all projectiles toward their targets
for (auto& p : projectiles) {
    p->update();    // Update position based on velocity
}

//  This check projectile collisions with enemies
 // Loop through projectiles to check if they hit enemies
for (auto& p : projectiles) {
    if (!p->hasReachedTarget()) continue;   // Skip if projectile hasn't reached target yet

    sf::Vector2f pPos = p->getPosition();   // Get projectile position

    // Check collision with each enemy
    for (auto& enemy : badBoys) {
        if (enemy->isDead()) continue;  // Skip dead enemies

        sf::Vector2f ePos = enemy->getPosition(); // Get enemy position
            // Calculate distance between projectile and enemy
        float dx = pPos.x - ePos.x;
        float dy = pPos.y - ePos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        // If projectile is within hit radius (20 pixels)
        if (distance < 20.f) {  // Hit radius
            enemy->takeDamage(p->getDamage());  // Damage the enemy
            break;  // Stop checking other enemies (projectile can only hit once)
        }
    }
}

//Remove Projectiles 
    // Remove projectiles that have reached their target
projectiles.erase(
    std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p) {
            return p->hasReachedTarget();   // Return true if projectile should be removed
        }),
    projectiles.end()
);
   
// Update Enemies
   for (auto& b : badBoys)
   {
       // Update enemy (returns true if enemy is stealing from resource)
       if (b->update()) {  
           resourceHealth -= 10;  // Reduce resource health by 10

           // Prevent health from going negative
           if (resourceHealth < 0) {
               resourceHealth = 0;
           }
           // Log health to console for debugging
           std::cout << "Resource Health: " << resourceHealth << "\n";
           // Check if resource is destroyed
           if (resourceHealth <= 0) {
               gameOver = true;     // Trigger game over state
               showWaveTransition = false;  // Hide wave transition immediately
               std::cout << "GAME OVER! Resource destroyed!\n";
           }
       }
   }

   // Double check game over state and stop updating if true
   if (gameOver) {
       return;  // Exit update function
   }


   // Remove enemies that escaped off - screen
       badBoys.erase(
           std::remove_if(badBoys.begin(), badBoys.end(),
               [](const std::unique_ptr<BadBoy>& b) {
                   sf::Vector2f pos = b->getPosition(); // Exit update function
                   // Remove if dead OR escaped off-screen
                   return b->isDead() ||
                       pos.x > 850.f ||  // Escaped right edge
                       pos.y > 650.f;     // Escaped bottom edge
               }),
           badBoys.end()
       );


   // Spawn BadBoy Logic
    // Check if it's time to spawn a new enemy
   if (waveActive &&            // Wave must be active
       enemiesSpawned < enemiesToSpawn &&       // Haven't spawned all enemies yet
       enemySpawnClock.getElapsedTime().asSeconds() >= enemySpawnInterval)  // Enough time has passed
   {
   float spawnX, spawnY;    // Spawn coordinates
   sf::Vector2i start, goal;    // Pathfinding start and goal

  

   // Randomly choose spawn location between horizontal (left) or vertical (top) path
            if (rand() % 2 == 0) {
                // Horizontal spawn from left side
            spawnX = -40.f; spawnY = 300.f; 
            start = sf::Vector2i(static_cast<int>(spawnX / GRID_SIZE),
                static_cast<int>(spawnY / GRID_SIZE));
            goal = sf::Vector2i(10, 7); // Grid position of resource
            
        } else {
                // Vertical spawn from top
            spawnX = 420.f; spawnY = -78.f; 
            start = sf::Vector2i(static_cast<int>(spawnX / GRID_SIZE),
                static_cast<int>(spawnY / GRID_SIZE));
            goal = sf::Vector2i(10, 7); // Grid position of resource
            
        }

            // Create enemy with health scaled by wave number 20 % more per wave
            float enemyHealth = 100.f * (1.0f + (currentWave - 1) * 0.2f);
            auto newEnemy = std::make_unique<BadBoy>(spawnX, spawnY, enemyHealth);

            // Calculate unique path for this enemy using A* algorithm
        newEnemy->path = Pathfinding::findPath(grid, start, goal, GRID_SIZE);

        // Add enemy to active enemies list
        badBoys.push_back(std::move(newEnemy));

       enemiesSpawned++;    // Increment spawn counter
       enemySpawnClock.restart(); // Reset spawn timer
   }

   //Wave Management
            // Check if current wave is complete (all enemies spawned and defeated)
   if (waveActive && enemiesSpawned == enemiesToSpawn && badBoys.empty()) {
       waveActive = false;  // Deactivate wave

       // Show transition screen
       showWaveTransition = true;
       waveTransitionClock.restart();

       currentWave++;           // Increment wave number
       enemiesToSpawn += 3;   // Add 3 more enemies for next wave
       enemiesSpawned = 0;      // Reset spawn counter

       // SPAWN REINFORCEMENTS  Only at Wave 4 and 5 (Bottom positions)
       if (currentWave == 4) {
           // Wave 4 Bottom-left reinforcement arrives
           goodBoys.push_back(std::make_unique<GoodBoy>(
               220.f, 650.f,   // Spawn from bottom-left
               220.f, 380.f    // Position is at bottom-left defensive spot
           ));
           std::cout << ">>> REINFORCEMENT ARRIVED (Bottom-Left)! Total soldiers: " << goodBoys.size() << " <<<\n";
       }
       else if (currentWave == 5) {
           // Wave 5 Bottom-right reinforcement
           goodBoys.push_back(std::make_unique<GoodBoy>(
               500.f, 650.f,   // Spawn from bottom-right
               500.f, 380.f    // Position is at bottom-right defensive spot
           ));
           std::cout << ">>> REINFORCEMENT ARRIVED (Bottom-Right)! Total soldiers: " << goodBoys.size() << " <<<\n";
       }

       // Make enemies spawn faster each wave 5% reduction in spawn interval
       enemySpawnInterval *= 0.95f;  
       // Set minimum spawn interval can't spawn faster than 0.5 seconds
       if (enemySpawnInterval < 0.5f) {
           enemySpawnInterval = 0.5f; // Minimum 0.5 seconds between spawns
       }
       // Log wave information to console
       std::cout << "Wave " << currentWave << " starting! ("
           << enemiesToSpawn << " enemies, spawn interval: "
           << enemySpawnInterval << "s)\n";
   }

   // Hide transition after 2 seconds and start wave
   if (showWaveTransition && waveTransitionClock.getElapsedTime().asSeconds() >= 2.0f) {
       showWaveTransition = false;  // Hide transition screen
       waveActive = true;       // Activate next wave
       std::cout << "Wave " << currentWave << " started!\n";
   }

  
   // victory condition
   // Check if player has survived all waves
   if (currentWave > waveGoal && !gameWon) {
       gameWon = true;          // Trigger victory state
       showWaveTransition = false;  // Hide wave transition immediately
       std::cout << "VICTORY! You survived " << waveGoal << " waves!\n";
   }

   // Stop updating if game is won
   if (gameWon) {
       return;
   }

}

void Game::render() {
    window.clear(sf::Color::Green);

    // Draw background and resource
    window.draw(*backgroundSprite);     // Draw grass background
    window.draw(road);  // Draw vertical path
    window.draw(enemyPath);     // Draw horizontal path
    window.draw(*resourceSprite);   // Draw resource

    // Draw all soldiers
    for (auto& b : goodBoys)
        b->draw(window);

    // Draw projectiles
    for (auto& p : projectiles) {
        p->draw(window);    // Draw each bullet
    }

    //  Draw enemies with their health bars
    for (auto& b : badBoys) {
        b->draw(window);

        sf::Vector2f pos = b->getPosition();

    }
    //  UI ELEMENTS - OUTSIDE THE ENEMY LOOP
    //  RESOURCE HEALTH BAR (only show during gameplay)
    if (!gameOver && !gameWon) {
        // Text Label
        sf::Text label(font);
        label.setString("Resource Health:");
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);
        label.setPosition({ 10.f, 13.f });
        window.draw(label);

        // Background bar (dark gray container)
        sf::RectangleShape resourceHealthBack(sf::Vector2f(150.f, 20.f));
        resourceHealthBack.setFillColor(sf::Color(50, 50, 50));
        resourceHealthBack.setOutlineColor(sf::Color::White);
        resourceHealthBack.setOutlineThickness(1.f);
        resourceHealthBack.setPosition({ 130.f, 10.f });
        window.draw(resourceHealthBack);

        // Foreground bar (colored based on health percentage)
        float healthPercent = resourceHealth / 100.f;
        sf::Color barColor;
        if (healthPercent > 0.6f) {
            barColor = sf::Color::Green; // Healthy green
        }
        else if (healthPercent > 0.3f) {
            barColor = sf::Color::Yellow; // Warning yellow
        }
        else {
            barColor = sf::Color::Red;   // Critical red
        }
        sf::RectangleShape resourceHealthBar(sf::Vector2f(150.f * healthPercent, 20.f));
        resourceHealthBar.setFillColor(barColor);
        resourceHealthBar.setPosition({ 130.f, 10.f });
        window.draw(resourceHealthBar);

        // Health percentage number
        sf::Text healthText(font);
        healthText.setString(std::to_string(resourceHealth) + "%");
        healthText.setCharacterSize(12);
        healthText.setFillColor(sf::Color::White);
        healthText.setStyle(sf::Text::Bold);
        healthText.setPosition({ 195.f, 13.f });
        window.draw(healthText);
    }

    // WAVE COUNTER (only show during gameplay)
    if (!gameOver && !gameWon) {
        sf::Text waveText(font);
        waveText.setString("Wave: " + std::to_string(currentWave));
        waveText.setCharacterSize(16);
        waveText.setFillColor(sf::Color::White);
        waveText.setPosition({ 700.f, 12.f });  // Top-right corner
        window.draw(waveText);

        // Display soldier count
        sf::Text soldierText(font);
        soldierText.setString("Soldiers: " + std::to_string(goodBoys.size()));
        soldierText.setCharacterSize(16);
        soldierText.setFillColor(sf::Color::White);
        soldierText.setPosition({ 680.f, 35.f });  // Below wave counter
        window.draw(soldierText);

    }

    //  GAME OVER SCREEN
   
    if (gameOver) {
        // Dark semi-transparent overlay
        sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        // Game Over box (centered container)
        sf::RectangleShape gameOverBox(sf::Vector2f(400.f, 300.f)); 
        gameOverBox.setFillColor(sf::Color(40, 40, 40)); // Dark gray
        gameOverBox.setOutlineColor(sf::Color::Red);
        gameOverBox.setOutlineThickness(3.f);
        gameOverBox.setOrigin({ 200.f, 150.f });    // Center origin
        gameOverBox.setPosition({ 400.f, 300.f });  // Screen center
        window.draw(gameOverBox);

        // "GAME OVER!" text
        sf::Text gameOverText(font);
        gameOverText.setString("GAME OVER!");
        gameOverText.setCharacterSize(50);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setStyle(sf::Text::Bold);
        // Center the text
        sf::FloatRect textBounds = gameOverText.getGlobalBounds();
        gameOverText.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
        gameOverText.setPosition({ 400.f, 240.f });
        window.draw(gameOverText);

        // "Resource Destroyed!" text
        sf::Text subText(font);
        subText.setString("Resource Destroyed!");
        subText.setCharacterSize(22);
        subText.setFillColor(sf::Color::White);
        // Center the text
        sf::FloatRect subBounds = subText.getGlobalBounds();
        subText.setOrigin({ subBounds.size.x / 2.f, subBounds.size.y / 2.f });
        subText.setPosition({ 400.f, 300.f });
        window.draw(subText);

        // Exit instructions
        sf::Text instructionText(font);
        instructionText.setString("Press ESC to exit");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(sf::Color(200, 200, 200));
        // Center the text
        sf::FloatRect instrBounds = instructionText.getGlobalBounds();
        instructionText.setOrigin({ instrBounds.size.x / 2.f, instrBounds.size.y / 2.f });
        instructionText.setPosition({ 400.f, 360.f });
        window.draw(instructionText);
    }

    
    // VICTORY SCREEN
    
    else if (gameWon) {
        // Dark overlay
        sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        // // Victory box (centered container)
        sf::RectangleShape victoryBox(sf::Vector2f(400.f, 300.f));
        victoryBox.setFillColor(sf::Color(40, 40, 40)); // Dark gray
        victoryBox.setOutlineColor(sf::Color::Green);   // Green border
        victoryBox.setOutlineThickness(3.f);
        victoryBox.setOrigin({ 200.f, 150.f });     // Center origin
        victoryBox.setPosition({ 400.f, 300.f });   // Screen center
        window.draw(victoryBox);

        // VICTORY! text
        sf::Text victoryText(font);
        victoryText.setString("VICTORY!");
        victoryText.setCharacterSize(50);
        victoryText.setFillColor(sf::Color::Green);
        victoryText.setStyle(sf::Text::Bold);
        // Center the text
        sf::FloatRect textBounds = victoryText.getGlobalBounds();
        victoryText.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
        victoryText.setPosition({ 400.f, 240.f });
        window.draw(victoryText);

        // "You Survived X Waves!" text
        sf::Text subText(font);
        subText.setString("You Survived " + std::to_string(waveGoal) + " Waves!");
        subText.setCharacterSize(22);
        subText.setFillColor(sf::Color::White);
        // Center the text
        sf::FloatRect subBounds = subText.getGlobalBounds();
        subText.setOrigin({ subBounds.size.x / 2.f, subBounds.size.y / 2.f });
        subText.setPosition({ 400.f, 300.f });
        window.draw(subText);

        // Instructions
        sf::Text instructionText(font);
        instructionText.setString("Press ESC to exit");
        instructionText.setCharacterSize(16);
        instructionText.setFillColor(sf::Color(200, 200, 200));
        // Center the text
        sf::FloatRect instrBounds = instructionText.getGlobalBounds();
        instructionText.setOrigin({ instrBounds.size.x / 2.f, instrBounds.size.y / 2.f });
        instructionText.setPosition({ 400.f, 360.f });
        window.draw(instructionText);
    }

    
    //  WAVE TRANSITION SCREEN (shows between waves)
    
    if (showWaveTransition) {
        // Semi-transparent overlay
        sf::RectangleShape overlay(sf::Vector2f(800.f, 600.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        // Transition box
        sf::RectangleShape transitionBox(sf::Vector2f(400.f, 200.f));
        transitionBox.setFillColor(sf::Color(40, 40, 40));
        transitionBox.setOutlineColor(sf::Color::Cyan);
        transitionBox.setOutlineThickness(3.f);
        transitionBox.setOrigin({ 200.f, 100.f });
        transitionBox.setPosition({ 400.f, 300.f });
        window.draw(transitionBox);

        // "WAVE X" text
        sf::Text waveText(font);
        waveText.setString("WAVE " + std::to_string(currentWave));
        waveText.setCharacterSize(50);
        waveText.setFillColor(sf::Color::Cyan);
        waveText.setStyle(sf::Text::Bold);

        sf::FloatRect bounds = waveText.getGlobalBounds();
        waveText.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
        waveText.setPosition({ 400.f, 280.f });
        window.draw(waveText);

        // "Get Ready!" text
        sf::Text readyText(font);
        readyText.setString("Get Ready!");
        readyText.setCharacterSize(24);
        readyText.setFillColor(sf::Color::White);

        sf::FloatRect readyBounds = readyText.getGlobalBounds();
        readyText.setOrigin({ readyBounds.size.x / 2.f, readyBounds.size.y / 2.f });
        readyText.setPosition({ 400.f, 340.f });
        window.draw(readyText);
    }

    window.display();    // Display everything we've drawn to the screen
}

















