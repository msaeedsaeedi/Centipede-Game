#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using std::cout, std::endl;

/*
 ** Constant Declerations **
 */
const int resolutionX = 960;  // X Resolution -> Ratio (15:17)
const int resolutionY = 1120; // Y Resolution -> Ratio (15:17)
const int OFFSET = 5;         // Play Area Offset From top (Grid Cols)
const int boxPixelsX = 32;    // Pixels Per Box X
const int boxPixelsY = 32;    // Pixels Per Box Y
const int gameRows = 30;      // Total Rows
const int gameColumns = 30;   // Total Columns

const int MAX_SCORE_LENGTH = 4;

const int MAX_LASERS = 3;            // Max Number of Bullets
const int MAX_MASHROOMS = 30;        // Max Number of Mushrooms
const int MAX_CENTEPEDE_LENGTH = 12; // Max Centepede Length
const int CentipedeHeadDataSize = 5;
const int CentipedeBodyDataSize = 3;

/*
    ** Readability **
        - General
        - Objects
        - Bullet
        - Player Movement
        - Sound Effects
*/
const int x = 0; // x-coordinate
const int y = 1; // y-coordinate

const int ONone = 0;      // Nothing Object
const int OPlayer = 1;    // Player Object
const int ODMushroom = 2; // Mushroom Object
const int OPMushroom = 3; // Mushroom Object
const int OLaser = 4;     // Mushroom Object
const int OCentepede = 5; // Centepede Object
const int OWalls = 6;     // Walls

const int exists = 2; // Specifically for [Bullet]
const int health = 2; // Specifically for [Mushroom]
const int MType = 3;

const int CDirection = 2;
const int CSize = 3;
const int TDirection = 4;

const int UP = 0;    // Move UP
const int DOWN = 1;  // Move Down
const int RIGHT = 2; // Move Right
const int LEFT = 3;  // Move Left

const int State_Menu = 0;
const int State_Play = 1;
const int State_GameOver = 2;
const int State_Exit = 3;

/*
    Global Variables
*/
float delta = 0;
int gameGrid[gameRows][gameColumns] = {};

/*
    Function Declerations
        - Player
        - Centepede
        - Bullet
        - Mushrooms
        - Input Handling
        - Collision
        - Sound Effects
        - Miscellaneous
*/

void RenderPlayer(RenderWindow &window, int player[], Sprite &playersprite); // Render Player
void MovePlayer(int player[], int direction);                                // Control Player Movement
bool isInPlayerArea(int Position[]);

void SpawnLaser(float Lasers[][3], const int player[], Sprite LaserSprites[], Texture &LaserTexture);                                      // Spawn a single bullet
void RenderLasers(RenderWindow &window, float Lasers[][3], Sprite LaserSprite[]);                                                          // Render Bullet
bool MoveLasers(float Laser[][3], int **&Mushrooms_Ptr, int &MushroomsCount, int ***&CentipedePtr, int &centipedes_count, char C_Score[]); // Control Bullet Movement

void GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, bool ManageHeap = false);
bool GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, int Position[], bool ManageHeap = true);

void GenerateMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount);
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int **&Mushrooms_Ptr, int &MushroomsCount);
void DestructMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[]);
void DestroyMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[]);
int GetMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount);
void PurgeMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount);

void HandlePlayer(int player[2]); // Handle KeyBoard Inputs

bool UpdateGrid(int Grid_x, int Grid_y, int object);
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction);

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int Direction, int T_Direction, int &centepedes_count, int **PreviousBody = nullptr, bool SplitDown = false);
void DeleteCentepede(int ***&centepede_ptr, int n, int &centepedes_count);
void RenderCentepedes(RenderWindow &Window, Texture CentepedeTexture_HEAD, Texture CentepedeTexture_BODY, int ***&centepede_ptr, int centepedes_count);
int MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[]);
int GetCentipede(int ***&centipedeptr, int centipedes_count, int Position[]);
int GetCentipedeBodyIndex(int ***&centipedeptr, int centipede_n, int Position[]);

void UpdateScore(int Increase, char C_Score[]);
int GetScore(char C_Score[], char result[]);

void PlayLaserSound();
void PlayKillSound();
void PlayCentipedeHeadShotSound();
void PlayCentipedeSplitSound();
void PlayEatSound();
void PlayMovePlayerSound();

int main()
{
    int State = State_Menu;
    char C_Score[20] = "Score : 0000";
    srand(time(0));
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(570, 665));
    window.setPosition(Vector2i(400, 0));

    while (State != State_Exit)
    {
        if (State == State_Menu)
        {
            Texture BackgroundTexture;
            Sprite BackgroundSprite;
            BackgroundTexture.loadFromFile("Screens/Menu.png");
            BackgroundSprite.setTexture(BackgroundTexture);

            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/MenuMusic.wav");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(70);
            backgroundMusic.play();

            while (window.isOpen() && State == State_Menu)
            {
                window.draw(BackgroundSprite);
                Event e;
                while (window.pollEvent(e))
                {
                    /*
                        Handle Closed Event
                    */
                    if (e.type == Event::Closed)
                    {
                        window.close();
                        State = State_Exit;
                    }

                    if (e.key.code == Keyboard::S)
                    {
                        State = State_Play;
                        break;
                    }
                    if (e.key.code == Keyboard::E)
                    {
                        State = State_Exit;
                        break;
                    }
                }
                window.display();
                window.clear();
            }
        }
        if (State == State_Play)
        {
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
                Sound
                    - Background Music
                    - Firing
            */
            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/background.ogg");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(50);
            backgroundMusic.play();

            /*
            Setup Data
                - Score
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
            int InitialSize = 12;
            bool Initialized = false;
            int EnteringPositionX = rand() % (gameColumns - 2);
            int Position[2] = {gameColumns, -1};
            int ***centepede_ptr = new int **[centepedes_count];

            float Lasers[MAX_LASERS][3]{};
            bool LaserShoted = false;
            Sprite LaserSprites[MAX_LASERS]{};
            Texture LaserTexture;
            LaserTexture.loadFromFile("Textures/bullet.png");
            LaserTexture.setSmooth(true);

            int MushroomsCount = 0;
            int **Mushrooms_Ptr = NULL;
            Texture MushroomTexture;
            MushroomTexture.loadFromFile("Textures/mushroom.png");

            /*
                Text to Display
            */
            Font myfont;
            Text T_Level, T_Score;
            myfont.loadFromFile("Fonts/BebasNeue-Regular.ttf");

            // Display Level
            T_Level.setFont(myfont);
            T_Level.setCharacterSize(48);
            T_Level.setPosition(900, 35);

            // Display Score
            T_Score.setFont(myfont);
            T_Score.setCharacterSize(48);
            T_Score.setPosition(40, 35);

            /*
                Initialization
            */
            GenerateCentipede(centepede_ptr, InitialSize, Position, LEFT, DOWN, centepedes_count);
            GenerateMushrooms(Mushrooms_Ptr, MushroomsCount);

            /*
                Clocks
            */
            Clock PlayerMovementClock;
            Clock LaserClock;
            Clock DeltaClock;
            Clock CentipedeGenerationClock;
            Clock CentipedeClock;

            /*
                Game Main Loops
            */
            while (window.isOpen() && State == State_Play)
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
                        State = State_Exit;
                    }
                    if (e.key.code == Keyboard::P)
                    {
                        State = State_GameOver;
                        break;
                    }
                }

                /*
                    Handle Keyboard
                        - Player Movement
                        - Laser Shots
                        - CentipedeGeneration
                */
                if (PlayerMovementClock.getElapsedTime().asMilliseconds() > 100)
                {
                    HandlePlayer(Player);
                    PlayerMovementClock.restart();
                }
                if (CentipedeClock.getElapsedTime().asMilliseconds() > 100)
                {
                    if (MoveCentepedes(centepede_ptr, centepedes_count, Mushrooms_Ptr, MushroomsCount, C_Score) == -1)
                    {
                        State = State_GameOver;
                        break;
                    }
                    if (!Initialized)
                    {
                        if (centepede_ptr[0][0][x] == (EnteringPositionX))
                        {
                            centepede_ptr[0][0][y]++;
                            centepede_ptr[0][0][x]++;
                            UpdateGrid(Position[x], Position[y], ONone);
                            Initialized = true;
                        }
                    }
                    CentipedeClock.restart();
                }
                if (LaserClock.getElapsedTime().asMilliseconds() > 200)
                {
                    if (Keyboard::isKeyPressed(Keyboard::Z))
                    {
                        PlayLaserSound();
                        LaserShoted = true;
                        SpawnLaser(Lasers, Player, LaserSprites, LaserTexture);
                        LaserClock.restart();
                    }
                }
                if (CentipedeGenerationClock.getElapsedTime().asMilliseconds() > 5000)
                {
                    if (isInPlayerArea(Position))
                    {
                        int Position[2] = {gameColumns, gameRows - ((rand() % 5) + 1)};
                        GenerateCentipede(centepede_ptr, 1, Position, LEFT, DOWN, centepedes_count);
                        CentipedeGenerationClock.restart();
                    }
                }

                /*
                    Real-Time Animations
                        - Lasers
                        - Centepede
                */
                if (LaserShoted)
                {
                    if (MoveLasers(Lasers, Mushrooms_Ptr, MushroomsCount, centepede_ptr, centepedes_count, C_Score) == false)
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

                T_Score.setString(C_Score);
                T_Level.setString("1");

                /*
                    -> Render Objects
                */
                window.draw(BackgroundSprite);
                window.draw(T_Level);
                window.draw(T_Score);
                RenderPlayer(window, Player, PlayerSprite);
                RenderCentepedes(window, CentepedeTexture_HEAD, CentepedeTexture_BODY, centepede_ptr, centepedes_count);
                RenderLasers(window, Lasers, LaserSprites);
                RenderMushrooms(window, MushroomTexture, Mushrooms_Ptr, MushroomsCount);
                /*
                     Refresh Frame
                */
                window.display();
                window.clear();
            }
            // Purge Everything
            for (int i = 0; i < gameRows; i++)
            {
                for (int j = 0; j < gameColumns; j++)
                {
                    gameGrid[i][j] = 0;
                }
            }
            for (int i = 0; i < centepedes_count; i++)
            {
                DeleteCentepede(centepede_ptr, i, centepedes_count);
            }
            PurgeMushrooms(Mushrooms_Ptr, MushroomsCount);
        }
        if (State == State_GameOver)
        {
            Texture BackgroundTexture;
            Sprite BackgroundSprite;
            BackgroundTexture.loadFromFile("Screens/GameOver.png");
            BackgroundSprite.setTexture(BackgroundTexture);

            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/MenuMusic.wav");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(70);
            backgroundMusic.play();

            Font textfont;
            textfont.loadFromFile("Fonts/Baumans-Regular.ttf");

            char C_ScoreOnly[MAX_SCORE_LENGTH + 1]{};
            GetScore(C_Score, C_ScoreOnly);

            Text Score;
            Score.setFont(textfont);
            Score.setCharacterSize(50);
            Score.setString(C_ScoreOnly);
            Score.setPosition(715 - Score.getLocalBounds().width, 518);

            Text Level;
            Level.setFont(textfont);
            Level.setCharacterSize(50);
            Level.setString("1");
            Level.setPosition(715 - Level.getLocalBounds().width, 594);

            Text HighScore;
            HighScore.setFont(textfont);
            HighScore.setCharacterSize(50);
            HighScore.setString("99999");
            HighScore.setPosition(715 - HighScore.getLocalBounds().width, 670);

            while (window.isOpen() && State == State_GameOver)
            {
                window.draw(BackgroundSprite);
                window.draw(Score);
                window.draw(Level);
                window.draw(HighScore);

                Event e;
                while (window.pollEvent(e))
                {
                    /*
                        Handle Closed Event
                    */
                    if (e.type == Event::Closed)
                    {
                        window.close();
                        State = State_Exit;
                    }
                    if (e.key.code == Keyboard::R)
                    {
                        State = State_Play;
                        break;
                    }
                    if (e.key.code == Keyboard::Escape)
                    {
                        State = State_Menu;
                        break;
                    }
                }
                window.display();
                window.clear();
            }
        }
        UpdateScore(0, C_Score);
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
bool isInPlayerArea(int Position[])
{
    if (Position[y] >= (gameRows - 5))
        return true;
    return false;
}
void MovePlayer(int player[], int direction)
{
    PlayMovePlayerSound();

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
bool MoveLasers(float Laser[][3], int **&Mushrooms_Ptr, int &MushroomsCount, int ***&CentipedePtr, int &centipedes_count, char C_Score[])
{
    bool lasers_in_motion = false;
    for (int i = 0; i < MAX_LASERS; i++)
    {
        if (Laser[i][exists] == true)
        {
            lasers_in_motion = true;
            int Position[] = {int(Laser[i][x]), int(ceil(Laser[i][y]))};
            if (gameGrid[Position[y]][Position[x]] != OPlayer)
            {
                UpdateGrid(Position[x], Position[y], ONone);
            }
            Laser[i][y] -= 35 * delta;
            if (Laser[i][y] < -1)
                Laser[i][exists] = false;
            else if (Laser[i][y] > -1)
            {
                int collided_object = 0;
                Position[x] = int(Laser[i][x]);
                Position[y] = int(floor(Laser[i][y]) + 1);
                if (UpdateGrid(Position[x], Position[y], OLaser, collided_object, UP) && collided_object != OPlayer)
                {
                    switch (collided_object)
                    {
                    case ODMushroom:
                    case OPMushroom:
                        PlayKillSound();
                        DestructMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score);
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
                        if (body_index == 0)
                        {
                            DeleteCentepede(CentipedePtr, centipede_n, centipedes_count);
                            UpdateScore(20, C_Score);
                            PlayCentipedeHeadShotSound();
                        }
                        else
                        {
                            PlayCentipedeSplitSound();
                            UpdateScore(10, C_Score);
                            int **PreviousDataP1 = new int *[body_index];
                            int **PreviousDataP2 = new int *[size - body_index];
                            for (int l = 0; l < body_index; l++)
                            {
                                PreviousDataP1[l] = new int[CentipedeBodyDataSize];
                                PreviousDataP1[l][x] = CentipedePtr[centipede_n][l][x];
                                PreviousDataP1[l][y] = CentipedePtr[centipede_n][l][y];
                                PreviousDataP1[l][CDirection] = CentipedePtr[centipede_n][l][CDirection];
                            }
                            for (int l = 0; l < size - body_index; l++)
                            {
                                PreviousDataP2[l] = new int[CentipedeBodyDataSize];
                                PreviousDataP2[l][x] = CentipedePtr[centipede_n][l + body_index][x];
                                PreviousDataP2[l][y] = CentipedePtr[centipede_n][l + body_index][y];
                                PreviousDataP2[l][CDirection] = CentipedePtr[centipede_n][l + body_index][CDirection];
                            }
                            DeleteCentepede(CentipedePtr, centipede_n, centipedes_count);
                            Position[x] += (Direction == LEFT) ? (-body_index) : (body_index);
                            GenerateCentipede(CentipedePtr, body_index, Position, Direction, T_Direction, centipedes_count, PreviousDataP1);
                            Position[x] += (Direction == LEFT) ? (body_index) : (-body_index);
                            GenerateCentipede(CentipedePtr, size - body_index, Position, Direction, T_Direction, centipedes_count, PreviousDataP2, true);
                            UpdateGrid(Position[x], Position[y], ONone);

                            // Freeing Memory
                            for (int l = 0; l < body_index; l++)
                                delete[] PreviousDataP1[l];
                            delete[] PreviousDataP1;
                            for (int l = 0; l < size - body_index; l++)
                                delete[] PreviousDataP2[l];
                            delete[] PreviousDataP2;
                        }
                        if (isInPlayerArea(Position))
                            GenerateMushroom(Mushrooms_Ptr, MushroomsCount, OPMushroom, Position);
                        break;
                    }
                    }
                    Laser[i][exists] = false;
                }
            }
        }
    }
    return lasers_in_motion;
}
void DestructMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[])
{
    int MushroomIndex = GetMushroom(Position, Mushrooms_Ptr, MushroomsCount);
    Mushrooms_Ptr[MushroomIndex][health] -= 1;
    if (Mushrooms_Ptr[MushroomIndex][health] == 0)
        DestroyMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score);
}
void DestroyMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[])
{
    Mushrooms_Ptr[GetMushroom(Position, Mushrooms_Ptr, MushroomsCount)][health] = 0;
    UpdateGrid(Position[x], Position[y], ONone);
    UpdateScore(1, C_Score);
}
void PurgeMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount)
{
    for (int i = 0; i < MushroomsCount; i++)
    {
        delete[] Mushrooms_Ptr[i];
    }
    delete[] Mushrooms_Ptr;
    Mushrooms_Ptr = 0;
}
void GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, bool ManageHeap)
{
    bool Created = false;
    do
    {
        int Grid_X = rand() % gameColumns;
        int Grid_Y = rand() % (gameRows - 5);
        if (gameGrid[Grid_Y][Grid_X] == ONone)
        {
            MushroomsCount++;
            if (ManageHeap)
            {
                int **temp = new int *[MushroomsCount];
                for (int i = 0; i < MushroomsCount - 1; i++)
                {
                    temp[i] = Mushrooms_Ptr[i];
                }
                temp[MushroomsCount - 1] = new int[4];
                delete[] Mushrooms_Ptr;
                Mushrooms_Ptr = temp;
                temp = nullptr;
            }
            UpdateGrid(Grid_X, Grid_Y, Type);
            Mushrooms_Ptr[MushroomsCount - 1][x] = Grid_X;
            Mushrooms_Ptr[MushroomsCount - 1][y] = Grid_Y;
            Mushrooms_Ptr[MushroomsCount - 1][health] = 2;
            Mushrooms_Ptr[MushroomsCount - 1][MType] = Type;
            Created = true;
        }
    } while (!Created);
}
bool GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, int Position[], bool ManageHeap)
{
    int Grid_X = Position[x];
    int Grid_Y = Position[y];
    if (gameGrid[Grid_Y][Grid_X] == ONone)
    {
        MushroomsCount++;
        if (ManageHeap)
        {
            int **temp = new int *[MushroomsCount];
            for (int i = 0; i < MushroomsCount - 1; i++)
            {
                temp[i] = Mushrooms_Ptr[i];
            }
            temp[MushroomsCount - 1] = new int[4];
            delete[] Mushrooms_Ptr;
            Mushrooms_Ptr = temp;
            temp = nullptr;
        }
        UpdateGrid(Grid_X, Grid_Y, Type);
        Mushrooms_Ptr[MushroomsCount - 1][x] = Grid_X;
        Mushrooms_Ptr[MushroomsCount - 1][y] = Grid_Y;
        Mushrooms_Ptr[MushroomsCount - 1][health] = 2;
        Mushrooms_Ptr[MushroomsCount - 1][MType] = Type;
        return true;
    }
    return false;
}
void GenerateMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount)
{
    int RandomCountMushrooms = (rand() % 11) + 20;
    Mushrooms_Ptr = new int *[RandomCountMushrooms];
    for (int i = 0; i < RandomCountMushrooms; i++)
    {
        Mushrooms_Ptr[i] = new int[4];
        GenerateMushroom(Mushrooms_Ptr, MushroomsCount, ODMushroom);
    }
}
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int **&Mushrooms_Ptr, int &MushroomsCount)
{
    for (int i = 0; i < MushroomsCount; i++)
    {
        if (Mushrooms_Ptr[i][health] != 0)
        {
            Sprite mushroom;
            mushroom.setTexture(MushroomTexture);
            int TextureTop = 0, TextureLeft = 0;
            if (Mushrooms_Ptr[i][MType] == ODMushroom)
                TextureTop = boxPixelsY;
            if (Mushrooms_Ptr[i][health] == 1)
                TextureLeft = boxPixelsX;
            mushroom.setTextureRect(IntRect(TextureLeft, TextureTop, boxPixelsX, boxPixelsY));
            mushroom.setPosition(Mushrooms_Ptr[i][x] * boxPixelsX, (Mushrooms_Ptr[i][y] + OFFSET) * boxPixelsY);
            Window.draw(mushroom);
        }
    }
}
int GetMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount)
{
    for (int i = 0; i < MushroomsCount; i++)
        if (Mushrooms_Ptr[i][x] == Position[x] && Mushrooms_Ptr[i][y] == Position[y])
            return i;
    return 0;
}
bool UpdateGrid(int Grid_x, int Grid_y, int object)
{
    if (Grid_x < gameColumns && Grid_x >= 0 && Grid_y < gameRows && Grid_y >= 0)
    {
        if (object != ONone)
        {
            if (gameGrid[Grid_y][Grid_x] != ONone)
            {
                return true;
            }
        }
        gameGrid[Grid_y][Grid_x] = object;
    }
    return false;
}
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction)
{
    if (Grid_x < gameColumns && Grid_x >= 0 && Grid_y < gameRows && Grid_y >= 0)
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
            if (gameGrid[Grid_y][Grid_x] != ONone)
            {
                collidedObject = gameGrid[Grid_y][Grid_x];
                return true;
            }
        }
        gameGrid[Grid_y][Grid_x] = object;
    }
    return false;
}

void GenerateCentipede(int ***&centepede_ptr, int size, int Position[], int Direction, int T_Direction, int &centepedes_count, int **PreviousBody, bool SplitDown)
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
    if (!SplitDown)
    {
        *(centepede_ptr[centepedes_count - 1][0] + x) = Position[x];
        *(centepede_ptr[centepedes_count - 1][0] + y) = Position[y];
        *(centepede_ptr[centepedes_count - 1][0] + CDirection) = Direction;
    }
    else
    {
        *(centepede_ptr[centepedes_count - 1][0] + x) = Position[x] + ((Direction == LEFT) ? (1) : (-1));
        *(centepede_ptr[centepedes_count - 1][0] + y) = Position[y] + ((T_Direction == DOWN) ? (1) : (-1));
        *(centepede_ptr[centepedes_count - 1][0] + CDirection) = (Direction == LEFT) ? (RIGHT) : (LEFT);
    }
    *(centepede_ptr[centepedes_count - 1][0] + TDirection) = T_Direction;
    UpdateGrid(Position[x], Position[y], OCentepede);
    for (int j = 1; j < size; j++)
    {
        centepede_ptr[centepedes_count - 1][j] = new int[3];
        if (PreviousBody == nullptr)
        {
            *(centepede_ptr[centepedes_count - 1][j] + y) = Position[y];
            *(centepede_ptr[centepedes_count - 1][j] + CDirection) = Direction;
            if (Direction == LEFT)
            {
                *(centepede_ptr[centepedes_count - 1][j] + x) = Position[x] + j;
                UpdateGrid(Position[x] + j, Position[y], OCentepede);
            }
            else
            {
                *(centepede_ptr[centepedes_count - 1][j] + x) = Position[x] - j;
                UpdateGrid(Position[x] - j, Position[y], OCentepede);
            }
        }
        else
        {
            *(centepede_ptr[centepedes_count - 1][j] + x) = PreviousBody[j][x];
            *(centepede_ptr[centepedes_count - 1][j] + y) = PreviousBody[j][y];
            *(centepede_ptr[centepedes_count - 1][j] + CDirection) = PreviousBody[j][CDirection];
            UpdateGrid(PreviousBody[j][x], PreviousBody[j][y], OCentepede);
        }
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
int MoveCentepedes(int ***&centepede_ptr, int centepedes_count, int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[])
{
    const int step_size = 1;
    for (int i = 0; i < centepedes_count; i++)
    {
        bool direction_changed = false;
        int collided_object = 0;
        int size = centepede_ptr[i][0][CSize];
        int P_direction = centepede_ptr[i][0][CDirection];
        int T_direction = centepede_ptr[i][0][TDirection];
        int Position[2]{
            centepede_ptr[i][0][x],
            centepede_ptr[i][0][y]};
        int PreviousObject = gameGrid[Position[y]][Position[x]];
        int NextObject = ONone;
        if ((Position[x] < (gameColumns - 1) && Position[x] > 0) && (Position[y] < gameRows && Position[y] > 0))
            NextObject = gameGrid[Position[y]][Position[x] + ((P_direction == RIGHT) ? (1) : (-1))];
        else
            NextObject = gameGrid[Position[y] + ((T_direction == DOWN) ? (1) : (-1))][Position[x]];
        for (int j = 0; j < size; j++)
            UpdateGrid(centepede_ptr[i][j][x], centepede_ptr[i][j][y], ONone);
        for (int j = size - 1; j > 0; j--)
        {
            centepede_ptr[i][j][x] = centepede_ptr[i][j - 1][x];
            centepede_ptr[i][j][y] = centepede_ptr[i][j - 1][y];
            centepede_ptr[i][j][CDirection] = centepede_ptr[i][j - 1][CDirection];
        }
        bool Collision = (PreviousObject == ODMushroom || PreviousObject == OPlayer || NextObject == OPMushroom || PreviousObject == OPMushroom);
        if (Collision || UpdateGrid(Position[x], Position[y], OCentepede, collided_object, P_direction))
        {
            if (collided_object != OCentepede)
            {
                if (PreviousObject == ODMushroom || PreviousObject == OPMushroom)
                {
                    PlayEatSound();
                    DestroyMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score);
                }
                if (PreviousObject == OPlayer)
                {
                    return -1;
                }
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
    return 0;
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
void UpdateScore(int Increase, char C_Score[])
{
    static int Score = 0;
    if (Increase == 0)
        Score = 0;
    Score += Increase;
    int TEMP = Score;
    for (int i = 0; i < MAX_SCORE_LENGTH; i++)
    {
        C_Score[i + 8] = (TEMP / pow(10, 3 - i)) + 48;
        TEMP %= static_cast<int>(pow(10, 3 - i));
    }
}
int GetScore(char C_Score[], char result[])
{
    int number = 0;
    for (int i = MAX_SCORE_LENGTH - 1; i >= 0; i--)
    {
        result[i] = C_Score[i + 8];
        number += C_Score[i + 8] * pow(10, i);
    }
    return number;
}
void PlayLaserSound()
{
    static Sound SLaser;
    static SoundBuffer _SLaser;
    _SLaser.loadFromFile("Sound-Effects/fire.wav");
    SLaser.setBuffer(_SLaser);
    SLaser.setVolume(20);
    SLaser.play();
}
void PlayKillSound()
{
    static Sound SMushroom_D;
    static SoundBuffer _SBMushroom_D;
    _SBMushroom_D.loadFromFile("Sound-Effects/kill.wav");
    SMushroom_D.setBuffer(_SBMushroom_D);
    SMushroom_D.setVolume(50);
    SMushroom_D.play();
}
void PlayCentipedeSplitSound()
{
    static Sound SCentipedeSplit;
    static SoundBuffer _SCentipedeSplit;
    _SCentipedeSplit.loadFromFile("Sound-Effects/split.wav");
    SCentipedeSplit.setBuffer(_SCentipedeSplit);
    SCentipedeSplit.setVolume(60);
    SCentipedeSplit.play();
}
void PlayCentipedeHeadShotSound()
{
    static Sound SCentipedeSplit;
    static SoundBuffer _SCentipedeSplit;
    _SCentipedeSplit.loadFromFile("Sound-Effects/headshot.wav");
    SCentipedeSplit.setBuffer(_SCentipedeSplit);
    SCentipedeSplit.setVolume(60);
    SCentipedeSplit.play();
}
void PlayEatSound()
{
    static Sound SEat;
    static SoundBuffer _SEat;
    _SEat.loadFromFile("Sound-Effects/eat.ogg");
    SEat.setBuffer(_SEat);
    SEat.setVolume(60);
    SEat.play();
}
void PlayMovePlayerSound()
{
    static Sound SPlayer;
    static SoundBuffer _SPlayer;
    _SPlayer.loadFromFile("Sound-Effects/move.wav");
    SPlayer.setBuffer(_SPlayer);
    SPlayer.setVolume(20);
    SPlayer.play();
}