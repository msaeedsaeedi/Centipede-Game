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

    Texture PlayerTexture;
    Sprite PlayerSprite;
    PlayerTexture.loadFromFile("Textures/player.png");
    PlayerSprite.setTexture(PlayerTexture);

    /*
        Setup Data
            - Player
    */
    float Player[3]{};
    Player[x] = (gameColumns / 2) * boxPixelsX;
    Player[y] = (gameRows - 5) * boxPixelsY;

    /*
        Clocks
    */
    Clock PlayerMovementClock;

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
            Handle Keyboard
        */
        if (PlayerMovementClock.getElapsedTime().asMilliseconds() > 100)
        {
            HandlePlayer(Player);
            PlayerMovementClock.restart();
        }

        /*
            -> Render Objects
        */
        window.draw(BackgroundSprite);
        RenderPlayer(window, Player, PlayerSprite);

        /*
             Refresh Frame
        */
        window.display();
        window.clear();
    }

    return 0;
}
void HandlePlayer(float player[3])
{
    if (Keyboard::isKeyPressed(Keyboard::Up))
        MovePlayer(player, UP);
    if (Keyboard::isKeyPressed(Keyboard::Down))
        MovePlayer(player, DOWN);
    if (Keyboard::isKeyPressed(Keyboard::Left))
        MovePlayer(player, LEFT);
    if (Keyboard::isKeyPressed(Keyboard::Right))
        MovePlayer(player, RIGHT);
}
void MovePlayer(float player[], int direction)
{
    switch (direction)
    {
    case UP:
        if (player[y] >= (gameRows - 4) * boxPixelsY)
            player[y] -= boxPixelsY;
        break;
    case DOWN:
        if (player[y] <= (gameRows - 2) * boxPixelsY)
            player[y] += boxPixelsY;
        break;
    case LEFT:
        if (player[x] >= boxPixelsX)
            player[x] -= boxPixelsX;
        break;
    case RIGHT:
        if (player[x] <= (gameColumns - 2) * boxPixelsX)
            player[x] += boxPixelsX;
        break;
    }
}
void RenderPlayer(RenderWindow &window, float player[], Sprite &playersprite)
{
    playersprite.setPosition(player[x], player[y]);
    window.draw(playersprite);
}