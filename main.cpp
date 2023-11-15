#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using sf::Keyboard, sf::Event, sf::Clock, sf::Texture;
using std::cout, std::endl;


/*
    ** Constant Declerations **
*/
const int resolutionX = 960;                        // X Resolution
const int resolutionY = 960;                        // Y Resolution
const int boxPixelsX = 32;                          // Pixels Per Box X
const int boxPixelsY = 32;                          // Pixels Per Box Y
const int gameRows = resolutionY / boxPixelsY;      // Total Rows
const int gameColumns = resolutionX / boxPixelsX;   // Total Columns
const int MAX_BULLETS = 50;                         // Max Number of Bullets



/*
    ** Readability **
        - General
        - Bullet
        - Player Movement
*/
const int x = 0;        // x-coordinate
const int y = 1;        // y-coordinate

const int exists = 2;   // Specifically for [Bullet]

const int UP = 0;       // Move UP
const int DOWN = 1;     // Move Down
const int RIGHT = 2;    // Move Right
const int LEFT = 3;     // Move Left



/*
    Function Declerations
        - Player
        - Bullet
        - Input Handling
        - Miscellaneous
*/
void RenderPlayer(sf::RenderWindow &window, float player[], sf::Sprite &playersprite);      // Render Player
void MovePlayer(float player[], int direction);                                             // Control Player Movement

void SpawnBullet(float bullet[][3], const float player[]);                                  // Spawn a single bullet
void RenderBullet(sf::RenderWindow &window, float bullet[], sf::Sprite &bulletSprite);      // Render Bullet
void MoveBullet(float bullet[]);                                                            // Control Bullet Movement

void ProcessMovement(float player[]);           // Handle KeyBoard Input for Player Movement

int main()
{
    return 0;
}