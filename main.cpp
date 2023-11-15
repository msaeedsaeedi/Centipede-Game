#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using std::cout, std::endl;

/*
 ** Constant Declerations **
 */
const int resolutionX = 960;                      // X Resolution
const int resolutionY = 960;                      // Y Resolution
const int boxPixelsX = 32;                        // Pixels Per Box X
const int boxPixelsY = 32;                        // Pixels Per Box Y
const int gameRows = resolutionY / boxPixelsY;    // Total Rows
const int gameColumns = resolutionX / boxPixelsX; // Total Columns
const int MAX_BULLETS = 50;                       // Max Number of Bullets

/*
    ** Readability **
        - General
        - Bullet
        - Player Movement
*/
const int x = 0; // x-coordinate
const int y = 1; // y-coordinate

const int exists = 2; // Specifically for [Bullet]

const int UP = 0;    // Move UP
const int DOWN = 1;  // Move Down
const int RIGHT = 2; // Move Right
const int LEFT = 3;  // Move Left

/*
    Function Declerations
        - Player
        - Bullet
        - Input Handling
        - Miscellaneous
*/

void RenderPlayer(RenderWindow &window, float player[], Sprite &playersprite); // Render Player
void MovePlayer(float player[], int direction);                                // Control Player Movement

void SpawnBullet(float bullet[][3], const float player[]);                     // Spawn a single bullet
void RenderBullet(RenderWindow &window, float bullet[], Sprite &bulletSprite); // Render Bullet
void MoveBullet(float bullet[]);                                               // Control Bullet Movement

void HandlePlayer(float player[3]); // Handle KeyBoard Inputs

int main()
{
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(640, 640));
    window.setPosition(Vector2i(400, 0));

    /*
        Setup Objects For Rendering
            - Background
            - Player
    */
    Texture BackgroundTexture;
    Sprite BackgroundSprite;
    BackgroundTexture.loadFromFile("Textures/background.png");
    BackgroundSprite.setTexture(BackgroundTexture);
    BackgroundSprite.setColor(Color(255, 255, 255, 255 * 0.25));

    /*
        Game Main Loops
    */
    while (window.isOpen())
    {
        /*
            -> Handling Events
        */
        Event e;
        while (window.pollEvent(e))
        {
            /*
                Handle Closed Event
            */
            if (e.type == Event::Closed)
            {
                window.close();
            }
        }

        /*
            -> Render Objects
        */
        window.draw(BackgroundSprite);

        /*
             Refresh Frame
        */
        window.display();
        window.clear();
    }

    return 0;
}