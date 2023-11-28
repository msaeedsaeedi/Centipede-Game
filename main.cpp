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
const int OWalls = 5;     // Walls

const int exists = 2; // Specifically for [Bullet]
const int health = 2; // Specifically for [Mushroom]

const int CDirection = 2;
const int CSize = 3;
const int TDirection = 4;

const int UP = 0;    // Move UP
const int DOWN = 1;  // Move Down
const int RIGHT = 2; // Move Right
const int LEFT = 3;  // Move Left

/*
    Global Variables
*/
float delta = 0;
int gameGrid[gameRows][gameColumns] = {};
int MushroomsCount = 0;
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
void MoveLasers(float Laser[][3], int *&MushroomsPtr);                                                // Control Bullet Movement

void GenerateMushrooms(Sprite MushroomSprites[], Texture &MushroomTexture, int *&Mushrooms);
void RenderMushrooms(RenderWindow &Window, Sprite MushroomSprites[], int *&Mushrooms);
void DestructMushroom(int Position[], int *&MushroomsPtr);
void DestroyMushroom(int Position[], int *&MushroomsPtr);
int GetMushroom(int Position[], int *Mushroomsptr);

void HandlePlayer(int player[2]); // Handle KeyBoard Inputs

bool UpdateGrid(int Grid_x, int Grid_y, int object);
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction);

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int &centepedes_count);
void DeleteCentepede(int ***&centepede_ptr, int n, int &centepedes_count);
void RenderCentepedes(RenderWindow &Window, Texture CentepedeTexture, int ***&centepede_ptr, int centepedes_count);
void MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int *&MushroomsPtr);

int main()
{
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(640, 640));
    window.setPosition(Vector2i(400, 50));

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
    UpdateGrid(Player[x], Player[y], OPlayer);

    int centepedes_count = 0;
    int size = 12;
    int Position[2] = {30 - 12, 0};
    int ***centepede_ptr = new int **[centepedes_count];

    float Lasers[MAX_LASERS][3]{};
    Sprite LaserSprites[MAX_LASERS]{};
    Texture LaserTexture;
    LaserTexture.loadFromFile("Textures/bullet.png");
    LaserTexture.setSmooth(true);

    int MushroomsCount = 0;
    int *MushroomsPtr = NULL;
    Sprite MushroomSprites[MAX_MASHROOMS]{};
    Texture MushroomTexture;
    MushroomTexture.loadFromFile("Textures/mushroom.png");

    /*
        Initialization
    */
    GenerateMushrooms(MushroomSprites, MushroomTexture, MushroomsPtr);
    GenerateCentipede(centepede_ptr, 12, Position, centepedes_count);

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
            MoveCentepedes(centepede_ptr, centepedes_count, MushroomsPtr);
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
        MoveLasers(Lasers, MushroomsPtr);

        /* system("clear");
        for (int i = 0; i < gameRows; i++)
        {
            for (int j = 0; j < gameColumns; j++)
            {
                cout << gameGrid[i][j] << "  ";
            }
            cout << "\n";
        }
        cout << endl; */

        /*
            -> Render Objects
        */
        window.draw(BackgroundSprite);
        RenderPlayer(window, Player, PlayerSprite);
        RenderCentepedes(window, CentepedeTexture, centepede_ptr, centepedes_count);
        RenderLasers(window, Lasers, LaserSprites);
        RenderMushrooms(window, MushroomSprites, MushroomsPtr);
        /*
             Refresh Frame
        */
        window.display();
        window.clear();
    }
    delete[] MushroomsPtr;
    return 0;
}
void HandlePlayer(int player[2])
{
    if (Keyboard::isKeyPressed(Keyboard::Up))
        MovePlayer(player, UP);
    else if (Keyboard::isKeyPressed(Keyboard::Down))
        MovePlayer(player, DOWN);
    else if (Keyboard::isKeyPressed(Keyboard::Left))
        MovePlayer(player, LEFT);
    else if (Keyboard::isKeyPressed(Keyboard::Right))
        MovePlayer(player, RIGHT);
}
void MovePlayer(int player[], int direction)
{
    UpdateGrid(player[x], player[y], ONone); // Modify this code Whole function
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
    UpdateGrid(player[x], player[y], OPlayer);
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
void MoveLasers(float Laser[][3], int *&MushroomsPtr)
{
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Laser[i][exists] == true)
        {
            int Position[] = {int(Laser[i][x]), int(floor(Laser[i][y]) + 1)};
            UpdateGrid(Position[x], Position[y], ONone);
            Laser[i][y] -= 50 * delta;
            if (Laser[i][y] < -1)
                Laser[i][exists] = false;
            else if (Laser[i][y] > -1)
            {
                int collided_object = 0;
                Position[x] = int(Laser[i][x]);
                Position[y] = int(floor(Laser[i][y]) + 1);
                if (UpdateGrid(Position[x], Position[y], OLaser, collided_object, UP))
                {
                    switch (collided_object)
                    {
                    case OMushroom:
                        DestructMushroom(Position, MushroomsPtr);
                        break;
                    }
                    Laser[i][exists] = false;
                }
            }
        }
    }
}
void DestructMushroom(int Position[], int *&MushroomsPtr)
{
    int *mushroom_health = (MushroomsPtr + GetMushroom(Position, MushroomsPtr) + health);
    *mushroom_health = *mushroom_health - 1;
    if (*mushroom_health == 0)
        UpdateGrid(Position[x], Position[y], ONone);
}
void DestroyMushroom(int Position[], int *&MushroomsPtr)
{
    *(MushroomsPtr + GetMushroom(Position, MushroomsPtr) + health) = 0;
    UpdateGrid(Position[x], Position[y], ONone);
}
void GenerateMushrooms(Sprite MushroomSprites[], Texture &MushroomTexture, int *&Mushrooms)
{
    srand(time(0));
    MushroomsCount = (rand() % 11) + 20;
    Mushrooms = new int[MushroomsCount * 3];

    for (int i = 0; i < MushroomsCount; i++)
    {
        int Grid_X = rand() % gameColumns;
        int Grid_Y = rand() % (gameRows - 5);
        if (gameGrid[Grid_Y][Grid_X] == ONone)
        {
            UpdateGrid(Grid_X, Grid_Y, OMushroom);
            MushroomSprites[i].setTexture(MushroomTexture);
            MushroomSprites[i].setTextureRect(IntRect(0, boxPixelsX, boxPixelsX, boxPixelsY));
            *(Mushrooms + x + i * 3) = Grid_X;
            *(Mushrooms + y + i * 3) = Grid_Y;
            *(Mushrooms + health + i * 3) = 2;
        }
        else
            i--;
    }
}
void RenderMushrooms(RenderWindow &Window, Sprite MushroomSprites[], int *&Mushrooms)
{
    for (int i = 0; i < MushroomsCount; i++)
    {
        if (*(Mushrooms + health + i * 3) != 0)
        {
            if (*(Mushrooms + i * 3 + health) == 1)
                MushroomSprites[i].setTextureRect(IntRect(boxPixelsX, boxPixelsX, boxPixelsX, boxPixelsY));
            MushroomSprites[i].setPosition(*(Mushrooms + x + i * 3) * boxPixelsX, *(Mushrooms + y + i * 3) * boxPixelsY);
            Window.draw(MushroomSprites[i]);
        }
    }
}
int GetMushroom(int Position[], int *Mushroomsptr)
{
    for (int i = 0; i < MushroomsCount; i++)
        if (*(Mushroomsptr + x + i * 3) == Position[x] && *(Mushroomsptr + i * 3 + y) == Position[y])
            return i * 3;
    return 0;
}

bool UpdateGrid(int Grid_x, int Grid_y, int object)
{
    if (object != ONone)
    {
        if ((gameGrid[Grid_y][Grid_x] != OPlayer) || (object != OLaser))
        {
            if (gameGrid[Grid_y][Grid_x] != ONone)
            {
                return true;
            }
        }
    }
    gameGrid[Grid_y][Grid_x] = object;
    return false;
}
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction)
{
    switch (Direction)
    {
    case LEFT:
        if (Grid_x == 0)
        {
            collidedObject = OWalls;
            return true;
        }
        break;
    case RIGHT:
        if (Grid_x == gameColumns - 1)
        {
            collidedObject = OWalls;
            return true;
        }
    }

    if (object != ONone)
    {
        if ((gameGrid[Grid_y][Grid_x] != OPlayer) || (object != OLaser))
        {
            if (gameGrid[Grid_y][Grid_x] != ONone)
            {
                collidedObject = gameGrid[Grid_y][Grid_x];
                return true;
            }
        }
    }
    gameGrid[Grid_y][Grid_x] = object;
    return false;
}

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int &centepedes_count)
{
    centepedes_count++;
    int ***D_temp = 0;
    D_temp = new int **[centepedes_count];
    for (int i = 0; i < centepedes_count - 1; i++)
    {
        D_temp[i] = centepede_ptr[i];
    }
    delete[] centepede_ptr;
    centepede_ptr = D_temp;

    centepede_ptr[centepedes_count - 1] = new int *[size];
    centepede_ptr[centepedes_count - 1][0] = new int[5];
    *(centepede_ptr[centepedes_count - 1][0] + CSize) = size;
    *(centepede_ptr[centepedes_count - 1][0] + x) = Position[x];
    *(centepede_ptr[centepedes_count - 1][0] + y) = Position[y];
    *(centepede_ptr[centepedes_count - 1][0] + CDirection) = LEFT;
    *(centepede_ptr[centepedes_count - 1][0] + TDirection) = DOWN;
    UpdateGrid(Position[x], Position[y], OCentepede);
    for (int j = 1; j < size; j++)
    {
        centepede_ptr[centepedes_count - 1][j] = new int[2];
        *(centepede_ptr[centepedes_count - 1][j] + x) = Position[x] + j;
        *(centepede_ptr[centepedes_count - 1][j] + y) = Position[y];
        UpdateGrid(Position[x] + j, Position[y], OCentepede);
    }
}

void DeleteCentepede(int ***&centepede_ptr, int n, int &centepedes_count)
{
    centepedes_count--;
    int size_of_centepede = centepede_ptr[n][0][CSize];
    for (int j = 0; j < size_of_centepede; j++)
    {
        delete[] centepede_ptr[n][j];
    }
    delete[] centepede_ptr[n];
    centepede_ptr[n] = 0;
    int ***D_temp = 0;
    if (centepedes_count != 0)
        D_temp = new int **[centepedes_count];
    for (int i = 0; i < centepedes_count - 1; i++)
    {
        if (centepede_ptr[i] != 0)
            D_temp[i] = centepede_ptr[i];
    }
    delete[] centepede_ptr;
    centepede_ptr = D_temp;
}
void RenderCentepedes(RenderWindow &Window, Texture CentepedeTexture, int ***&centepede_ptr, int centepedes_count)
{
    for (int i = 0; i < centepedes_count; i++)
    {
        int size = centepede_ptr[i][0][CSize];
        for (int j = 0; j < size; j++)
        {
            Sprite sm;
            int X = (centepede_ptr[i][j][x]);
            int Y = (centepede_ptr[i][j][y]);
            sm.setTexture(CentepedeTexture);
            sm.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            sm.setPosition(X * boxPixelsX, Y * boxPixelsY);
            Window.draw(sm);
        }
    }
}
void MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int *&mushroom_ptr)
{
    const int step_size = 1;
    for (int i = 0; i < centepedes_count; i++)
    {
        int size = centepede_ptr[i][0][CSize];
        for (int j = 1; j < size; j++)
            UpdateGrid(centepede_ptr[i][j][x], centepede_ptr[i][j][y], ONone);
        for (int j = size - 1; j > 0; j--)
        {
            centepede_ptr[i][j][x] = centepede_ptr[i][j - 1][x];
            centepede_ptr[i][j][y] = centepede_ptr[i][j - 1][y];
        }
        for (int j = 1; j < size; j++)
            UpdateGrid(centepede_ptr[i][j][x], centepede_ptr[i][j][y], OCentepede);
    }
    int P_direction = centepede_ptr[0][0][CDirection];
    int Position[2]{};
    Position[x] = centepede_ptr[0][0][x];
    Position[y] = centepede_ptr[0][0][y];
    int collided_object = 0;
    if (UpdateGrid(Position[x], Position[y], OCentepede, collided_object, P_direction))
    {
        if (collided_object == OWalls)
        {
            DestroyMushroom(Position, mushroom_ptr);
        }
        if (collided_object != OCentepede)
        {
            if (collided_object == OMushroom)
                DestroyMushroom(Position, mushroom_ptr);
            *(centepede_ptr[0][0] + CDirection) = (*(centepede_ptr[0][0] + CDirection) == RIGHT) ? (LEFT) : (RIGHT);

            if (Position[y] == (gameRows - 1))
            {
                *(centepede_ptr[0][0] + TDirection) = UP;
            }
            else if (Position[y] < (gameRows - 4))
            {
                *(centepede_ptr[0][0] + TDirection) = DOWN;
            }
            switch (*(centepede_ptr[0][0] + TDirection))
            {
            case DOWN:
                *(centepede_ptr[0][0] + y) += step_size;
                break;
            case UP:
                *(centepede_ptr[0][0] + y) -= step_size;
                break;
            }
            P_direction = centepede_ptr[0][0][CDirection];
            return;
        }
    }
    switch (P_direction)
    {
    case LEFT:
        *(centepede_ptr[0][0] + x) -= step_size;
        break;
    case RIGHT:
        *(centepede_ptr[0][0] + x) += step_size;
    }
}
