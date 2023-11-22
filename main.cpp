#include <iostream>
#include <math.h>
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

const int MAX_LASERS = 50;           // Max Number of Bullets
const int MAX_MASHROOMS = 30;        // Max Number of Mushrooms
const int MAX_CENTEPEDE_LENGTH = 12; // Max Centepede Length
/*
    ** Readability **
        - General
        - Objects
        - Bullet
        - Player Movement
*/
const int x = 0; // x-coordinate
const int y = 1; // y-coordinate

const int ONone = 0;      // Nothing Object
const int OPlayer = 1;    // Player Object
const int OMushroom = 2;  // Mushroom Object
const int OLaser = 3;     // Mushroom Object
const int OCentepede = 4; // Centepede Object

const int exists = 2; // Specifically for [Bullet]

const int UP = 0;    // Move UP
const int DOWN = 1;  // Move Down
const int RIGHT = 2; // Move Right
const int LEFT = 3;  // Move Left

/*
    Global Variables
*/
float delta = 0;
int gameGrid[gameRows][gameColumns] = {};
int Centepede_initial_length = 12;

/*
    Function Declerations
        - Player
        - Centepede
        - Bullet
        - Mushrooms
        - Input Handling
        - Collision
        - Miscellaneous
*/

void RenderPlayer(RenderWindow &window, int player[], Sprite &playersprite); // Render Player
void MovePlayer(int player[], int direction);                                // Control Player Movement

void SpawnLaser(float Lasers[][3], const int player[], Sprite LaserSprites[], Texture &LaserTexture); // Spawn a single bullet
void RenderLasers(RenderWindow &window, float Lasers[][3], Sprite LaserSprite[]);                     // Render Bullet
void MoveLasers(float Laser[][3]);                                                                    // Control Bullet Movement

void HandlePlayer(int player[2]); // Handle KeyBoard Inputs

int main()
{
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(640, 640));
    window.setPosition(Vector2i(400, 200));

    /*
        Setup Objects For Rendering
            - Background
            - Player
            - Centepede
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

    Texture CentepedeTexture;
    CentepedeTexture.loadFromFile("Textures/c_head_left_walk.png");

    /*
        Setup Data
            - Player
            - Centepede
            - Lasers
            - Mushrooms
    */
    int Player[2]{};
    Player[x] = (gameColumns / 2);
    Player[y] = (gameRows - 5);
    gameGrid[Player[y]][Player[x]] = OPlayer;

    float centepede[MAX_CENTEPEDE_LENGTH][2]{};
    centepede[0][x] = (gameColumns - 1);
    centepede[0][y] = (0);

    float Lasers[MAX_LASERS][3]{};
    Sprite LaserSprites[MAX_LASERS]{};
    Texture LaserTexture;
    LaserTexture.loadFromFile("Textures/bullet.png");
    LaserTexture.setSmooth(true);

    int MushroomsCount = 0;
    Sprite MushroomSprites[MAX_MASHROOMS]{};
    Texture MushroomTexture;
    MushroomTexture.loadFromFile("Textures/mushroom.png");

    /*
        Clocks
    */
    Clock PlayerMovementClock;
    Clock LaserClock;
    Clock DeltaClock;

    /*
        Game Main Loops
    */
    while (window.isOpen())
    {
        /*
            Calculating Delta Seconds for Speed
        */
        Time elapsed = DeltaClock.restart();
        delta = elapsed.asSeconds();

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
                - Player Movement
                - Laser Shots
        */
        if (PlayerMovementClock.getElapsedTime().asMilliseconds() > 100)
        {
            HandlePlayer(Player);
            PlayerMovementClock.restart();
        }
        if (LaserClock.getElapsedTime().asMilliseconds() > 200)
        {
            if (Keyboard::isKeyPressed(Keyboard::Z))
            {
                SpawnLaser(Lasers, Player, LaserSprites, LaserTexture);
                LaserClock.restart();
            }
        }

        /*
            Real-Time Animations
                - Lasers
                - Centepede
        */
        MoveLasers(Lasers);


        system("clear");
        for (int i = 0; i < gameRows; i++)
        {
            for (int j = 0; j < gameColumns; j++)
            {
                cout << gameGrid[i][j] << "  ";
            }
            cout << endl;
        }

        /*
            -> Render Objects
        */
        window.draw(BackgroundSprite);
        RenderPlayer(window, Player, PlayerSprite);
        RenderLasers(window, Lasers, LaserSprites);
        /*
             Refresh Frame
        */
        window.display();
        window.clear();
    }

    return 0;
}
void HandlePlayer(int player[2])
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
void MovePlayer(int player[], int direction)
{
    gameGrid[player[y]][player[x]] = 0;
    switch (direction)
    {
    case UP:
        if (player[y] >= (gameRows - 4))
        {
            if (gameGrid[player[y] - 1][player[x]] != 0)
            {
                cout << "Collided" << endl;
            }
            else
                player[y] -= 1;
        }
        break;
    case DOWN:
        if (player[y] <= (gameRows - 2))
        {
            if (gameGrid[player[y] + 1][player[x]] != 0)
            {
                cout << "Collided" << endl;
            }
            else
                player[y] += 1;
        }
        break;
    case LEFT:
        if (player[x] >= 1)
        {
            if (gameGrid[player[y]][player[x] - 1] != 0)
            {
                cout << "Collided" << endl;
            }
            else
                player[x] -= 1;
        }
        break;
    case RIGHT:
        if (player[x] <= (gameColumns - 2))
        {
            if (gameGrid[player[y]][player[x] + 1] != 0)
            {
                cout << "Collided" << endl;
            }
            else
                player[x] += 1;
        }
        break;
    }
    gameGrid[player[y]][player[x]] = 1;
}
void RenderPlayer(RenderWindow &window, int player[], Sprite &playersprite)
{
    playersprite.setPosition(player[x] * boxPixelsX, player[y] * boxPixelsY);
    window.draw(playersprite);
}
void SpawnLaser(float Lasers[][3], const int player[], Sprite LaserSprites[], Texture &LaserTexture)
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Lasers[i][exists] == false)
        {
            Lasers[i][x] = player[x];
            Lasers[i][y] = player[y];
            Lasers[i][exists] = true;
            LaserSprites[i].setTexture(LaserTexture);
            break;
        }
    }
}
void RenderLasers(RenderWindow &window, float Lasers[][3], Sprite LaserSprite[])
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Lasers[i][exists] == true)
        {
            LaserSprite[i].setPosition(Lasers[i][x] * boxPixelsX, Lasers[i][y] * boxPixelsY);
            window.draw(LaserSprite[i]);
        }
    }
}
void MoveLasers(float Laser[][3])
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Laser[i][exists] == true)
        {
            int Position[] = {int(Laser[i][x]), int(floor(Laser[i][y]) + 1)};
            gameGrid[int(floor(Laser[i][y]) + 1)][int(Laser[i][x])] = ONone;

            Laser[i][y] -= 50 * delta;
            if (Laser[i][y] < -1)
            {
                Laser[i][exists] = false;
            }
            else if (Laser[i][y] > -1)
            {
                int collided_object = 0;
                gameGrid[int(floor(Laser[i][y]) + 1)][int(Laser[i][x])] = OLaser;
                
            }
        }
    }
}
