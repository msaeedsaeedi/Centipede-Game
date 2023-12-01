#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using std::cout, std::endl;

/*
 ** Constant Declerations **
 */
const int resolutionX = 960;  // X Resolution
const int resolutionY = 1088; // Y Resolution
const int OFFSET = 4;         // Play Area Offset From top (Grid Cols)
const int boxPixelsX = 32;    // Pixels Per Box X
const int boxPixelsY = 32;    // Pixels Per Box Y
const int gameRows = 30;      // Total Rows
const int gameColumns = 30;   // Total Columns

const int MAX_LASERS = 3;            // Max Number of Bullets
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
bool MoveLasers(float Laser[][3], int *&MushroomsPtr, int ***&CentipedePtr, int &centipedes_count);   // Control Bullet Movement

void GenerateMushrooms(int *&Mushrooms);
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int *&Mushrooms);
void DestructMushroom(int Position[], int *&MushroomsPtr);
void DestroyMushroom(int Position[], int *&MushroomsPtr);
int GetMushroom(int Position[], int *Mushroomsptr);

void HandlePlayer(int player[2]); // Handle KeyBoard Inputs

bool UpdateGrid(int Grid_x, int Grid_y, int object);
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction);

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int Direction, int T_Direction, int &centepedes_count);
void DeleteCentepede(int ***&centepede_ptr, int n, int &centepedes_count);
void RenderCentepedes(RenderWindow &Window, Texture CentepedeTexture_HEAD, Texture CentepedeTexture_BODY, int ***&centepede_ptr, int centepedes_count);
void MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int *&MushroomsPtr);
int GetCentipede(int ***&centipedeptr, int centipedes_count, int Position[]);
int GetCentipedeBodyIndex(int ***&centipedeptr, int centipede_n, int Position[]);

int main()
{
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(600, 680));
    window.setPosition(Vector2i(400, 0));

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

    Texture PlayerTexture;
    Sprite PlayerSprite;
    PlayerTexture.loadFromFile("Textures/player.png");
    PlayerSprite.setTexture(PlayerTexture);

    Texture CentepedeTexture_HEAD;
    Texture CentepedeTexture_BODY;
    CentepedeTexture_HEAD.loadFromFile("Textures/c_head_left_walk.png");
    CentepedeTexture_BODY.loadFromFile("Textures/c_body_left_walk.png");
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
    bool LaserShoted = false;
    Sprite LaserSprites[MAX_LASERS]{};
    Texture LaserTexture;
    LaserTexture.loadFromFile("Textures/bullet.png");
    LaserTexture.setSmooth(true);

    int MushroomsCount = 0;
    int *MushroomsPtr = NULL;
    Texture MushroomTexture;
    MushroomTexture.loadFromFile("Textures/mushroom.png");

    /*
        Initialization
    */
    GenerateCentipede(centepede_ptr, 10, Position, LEFT, DOWN, centepedes_count);
    GenerateMushrooms(MushroomsPtr);

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
                LaserShoted = true;
                SpawnLaser(Lasers, Player, LaserSprites, LaserTexture);
                LaserClock.restart();
            }
        }

        /*
            Real-Time Animations
                - Lasers
                - Centepede
        */
        if (LaserShoted)
        {
            if (MoveLasers(Lasers, MushroomsPtr, centepede_ptr, centepedes_count) == false)
                LaserShoted = false;
        }

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
        RenderCentepedes(window, CentepedeTexture_HEAD, CentepedeTexture_BODY, centepede_ptr, centepedes_count);
        RenderLasers(window, Lasers, LaserSprites);
        RenderMushrooms(window, MushroomTexture, MushroomsPtr);
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
    playersprite.setPosition(player[x] * boxPixelsX, (player[y] + OFFSET) * boxPixelsY);
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
            LaserSprite[i].setPosition(Lasers[i][x] * boxPixelsX, (Lasers[i][y] + OFFSET) * boxPixelsY);
            window.draw(LaserSprite[i]);
        }
    }
}
bool MoveLasers(float Laser[][3], int *&MushroomsPtr, int ***&CentipedePtr, int &centipedes_count)
{
    bool lasers_in_motion = false;
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Laser[i][exists] == true)
        {
            lasers_in_motion = true;
            int Position[] = {int(Laser[i][x]), int(ceil(Laser[i][y]))};
            UpdateGrid(Position[x], Position[y], ONone);
            Laser[i][y] -= 35 * delta;
            if (Laser[i][y] < 0)
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
                    case OCentepede:
                    {
                        int centipede_n = GetCentipede(CentipedePtr, centipedes_count, Position);
                        if (centipede_n == -1)
                            break;
                        int size = CentipedePtr[centipede_n][0][CSize];
                        int body_index = GetCentipedeBodyIndex(CentipedePtr, centipede_n, Position);
                        if (body_index == -1)
                            break;
                        int Direction = CentipedePtr[centipede_n][0][CDirection];
                        int T_Direction = CentipedePtr[centipede_n][0][TDirection];
                        DeleteCentepede(CentipedePtr, centipede_n, centipedes_count);
                        if (body_index != 0)
                        {
                            if (Direction == LEFT)
                                Position[x] -= body_index;
                            else
                                Position[x] += body_index;
                            GenerateCentipede(CentipedePtr, body_index, Position, Direction, T_Direction, centipedes_count);
                            if (Direction == LEFT)
                                Position[x] += size;
                            else
                                Position[x] -= size;
                            GenerateCentipede(CentipedePtr, size - body_index, Position, (Direction == LEFT) ? (RIGHT) : (LEFT), T_Direction, centipedes_count);
                        }
                    }
                    break;
                    }
                    Laser[i][exists] = false;
                }
            }
        }
    }
    return lasers_in_motion;
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
void GenerateMushrooms(int *&Mushrooms)
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
            *(Mushrooms + x + i * 3) = Grid_X;
            *(Mushrooms + y + i * 3) = Grid_Y;
            *(Mushrooms + health + i * 3) = 2;
        }
        else
            i--;
    }
}
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int *&Mushrooms)
{
    for (int i = 0; i < MushroomsCount; i++)
    {
        if (*(Mushrooms + health + i * 3) != 0)
        {
            Sprite mushroom;
            mushroom.setTexture(MushroomTexture);

            if (*(Mushrooms + i * 3 + health) == 1)
                mushroom.setTextureRect(IntRect(boxPixelsX, boxPixelsX, boxPixelsX, boxPixelsY));
            else
                mushroom.setTextureRect(IntRect(0, boxPixelsX, boxPixelsX, boxPixelsY));

            mushroom.setPosition(*(Mushrooms + x + i * 3) * boxPixelsX, (*(Mushrooms + y + i * 3) + OFFSET) * boxPixelsY);
            Window.draw(mushroom);
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

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int Direction, int T_Direction, int &centepedes_count)
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
    *(centepede_ptr[centepedes_count - 1][0] + CDirection) = Direction;
    *(centepede_ptr[centepedes_count - 1][0] + TDirection) = T_Direction;
    UpdateGrid(Position[x], Position[y], OCentepede);
    for (int j = 1; j < size; j++)
    {
        centepede_ptr[centepedes_count - 1][j] = new int[3];
        if (Direction == LEFT)
            *(centepede_ptr[centepedes_count - 1][j] + x) = Position[x] + j;
        else
            *(centepede_ptr[centepedes_count - 1][j] + x) = Position[x] - j;
        *(centepede_ptr[centepedes_count - 1][j] + y) = Position[y];
        *(centepede_ptr[centepedes_count - 1][j] + CDirection) = Direction;
        UpdateGrid(Position[x] + j, Position[y], OCentepede);
    }
}

void DeleteCentepede(int ***&centepede_ptr, int n, int &centepedes_count)
{
    int size_of_centepede = centepede_ptr[n][0][CSize];
    for (int j = 0; j < size_of_centepede; j++)
    {
        UpdateGrid(centepede_ptr[n][j][x], centepede_ptr[n][j][y], ONone);
        delete[] centepede_ptr[n][j];
    }
    delete[] centepede_ptr[n];
    centepede_ptr[n] = 0;
    int ***D_temp = 0;
    if (centepedes_count != 0)
        D_temp = new int **[centepedes_count - 1];
    for (int i = 0, j = 0; i < centepedes_count; i++)
    {
        if (centepede_ptr[i] != 0)
            D_temp[j] = centepede_ptr[i], j++;
    }
    delete[] centepede_ptr;
    centepede_ptr = D_temp;
    centepedes_count--;
}
void RenderCentepedes(RenderWindow &Window, Texture CentepedeTexture_HEAD, Texture CentepedeTexture_BODY, int ***&centepede_ptr, int centepedes_count)
{
    for (int i = 0; i < centepedes_count; i++)
    {
        int size = centepede_ptr[i][0][CSize];

        /*
            HEAD Rendering
        */
        {
            Sprite head;
            int X = (centepede_ptr[i][0][x]);
            int Y = (centepede_ptr[i][0][y]);
            head.setTexture(CentepedeTexture_HEAD);
            head.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            head.setPosition(X * boxPixelsX, (Y + OFFSET) * boxPixelsY);
            if (centepede_ptr[i][0][CDirection] == RIGHT)
                head.setOrigin(boxPixelsX, boxPixelsY), head.setRotation(180);
            Window.draw(head);
        }

        for (int j = 1; j < size; j++)
        {
            Sprite sm;
            int X = (centepede_ptr[i][j][x]);
            int Y = (centepede_ptr[i][j][y]);
            sm.setTexture(CentepedeTexture_BODY);
            sm.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            sm.setPosition(X * boxPixelsX, (Y + OFFSET) * boxPixelsY);
            if (centepede_ptr[i][j][CDirection] == RIGHT)
                sm.setOrigin(boxPixelsX, boxPixelsY), sm.setRotation(180);
            Window.draw(sm);
        }
    }
}
void MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int *&mushroom_ptr)
{
    const int step_size = 1;
    for (int i = 0; i < centepedes_count; i++)
    {
        bool direction_changed = false;
        int collided_object = 0;
        int size = centepede_ptr[i][0][CSize];
        int P_direction = centepede_ptr[i][0][CDirection];
        int Position[2]{
            centepede_ptr[i][0][x],
            centepede_ptr[i][0][y]};
        int PreviousObject = gameGrid[Position[y]][Position[x]];

        for (int j = 0; j < size; j++)
            UpdateGrid(centepede_ptr[i][j][x], centepede_ptr[i][j][y], ONone);
        for (int j = size - 1; j > 0; j--)
        {
            centepede_ptr[i][j][x] = centepede_ptr[i][j - 1][x];
            centepede_ptr[i][j][y] = centepede_ptr[i][j - 1][y];
            centepede_ptr[i][j][CDirection] = centepede_ptr[i][j - 1][CDirection];
        }
        if (PreviousObject == OMushroom || UpdateGrid(Position[x], Position[y], OCentepede, collided_object, P_direction))
        {
            if (collided_object != OCentepede)
            {
                if (PreviousObject == OMushroom)
                    DestroyMushroom(Position, mushroom_ptr);
                centepede_ptr[i][0][CDirection] = ((centepede_ptr[i][0][CDirection]) == RIGHT) ? (LEFT) : (RIGHT);

                if (Position[y] == (gameRows - 1))
                {
                    centepede_ptr[i][0][TDirection] = UP;
                }
                else if (Position[y] < (gameRows - 4))
                {
                    centepede_ptr[i][0][TDirection] = DOWN;
                }
                if (centepede_ptr[i][0][TDirection] == DOWN)
                    centepede_ptr[i][0][y] += step_size;
                else
                    centepede_ptr[i][0][y] -= step_size;
                P_direction = centepede_ptr[i][0][CDirection];
                direction_changed = true;
            }
        }
        /*
            Below Lines are coded intentionally to match the speed of each centipede
        */
        if (!direction_changed)
            UpdateGrid(centepede_ptr[i][0][x], centepede_ptr[i][0][y], ONone);
        if (!direction_changed)
        {
            if (P_direction == LEFT)
                centepede_ptr[i][0][x] -= step_size;
            else
                centepede_ptr[i][0][x] += step_size;
        }
        for (int j = 0; j < size; j++)
        {
            if (!direction_changed)
                UpdateGrid(centepede_ptr[i][j][x], centepede_ptr[i][j][y], OCentepede);
        }
    }
}
int GetCentipede(int ***&centipedeptr, int centipedes_count, int Position[])
{
    for (int i = 0; i < centipedes_count; i++)
    {
        int size = centipedeptr[i][0][CSize];
        for (int j = 0; j < size; j++)
        {
            if (centipedeptr[i][j][x] == Position[x] && centipedeptr[i][j][y] == Position[y])
            {
                return i;
            }
        }
    }
    return -1;
}
int GetCentipedeBodyIndex(int ***&centipedeptr, int centipede_n, int Position[])
{
    int size = centipedeptr[centipede_n][0][CSize];
    for (int j = 0; j < size; j++)
    {
        if (centipedeptr[centipede_n][j][x] == Position[x] && centipedeptr[centipede_n][j][y] == Position[y])
        {
            return j;
        }
    }
    return -1;
}
