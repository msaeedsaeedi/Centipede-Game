#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

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

const int MAX_SCORE_LENGTH = 4; // Score Digits

const int MAX_LASERS = 2;            // Max Number of Bullets
const int MAX_MASHROOMS = 30;        // Max Number of Mushrooms
const int MAX_CENTEPEDE_LENGTH = 12; // Max Centipede Length

const int CentipedeHeadDataSize = 5; // Used for heap purpose
const int CentipedeBodyDataSize = 3; // Used for heap purpose

/*
 ** Readability **
 */
const int x = 0; // x-coordinate
const int y = 1; // y-coordinate

const int ONone = 0;      // Nothing Object
const int OPlayer = 1;    // Player Object
const int ODMushroom = 2; // Default Mushroom Object
const int OPMushroom = 3; // Poisonous Mushroom Object
const int OLaser = 4;     // Laser Object
const int OCentipede = 5; // Centipede Object
const int OWalls = 6;     // Walls

const int exists = 2; // Specifically for [Bullet]

const int health = 2; // Specifically for [Mushroom]
const int MType = 3;  // Specifically for [Mushroom] [ Mushroom Type ]

const int CDirection = 2; // Centipede Direction
const int CSize = 3;      // Centipede Segments length
const int TDirection = 4; // Transcending Direction [UP, DOWN]

const int UP = 0;    // Move UP
const int DOWN = 1;  // Move Down
const int RIGHT = 2; // Move Right
const int LEFT = 3;  // Move Left

const int State_Menu = 0;     // Main Menu
const int State_Play = 1;     // Main Play
const int State_GameOver = 2; // Game over
const int State_Exit = 3;     // Game End

/*
    Global Variables
*/
float delta = 0;
int gameGrid[gameRows][gameColumns] = {};

/*
- Player
    - RenderPlayer      -> Displays Player on the screen
    - MovePlayer        -> Move Player in specific Direction
    - IsInPlayerArea    -> Check wheather the given co-ordinates are in player area?
    - HandlePlayerInput -> Move Player on keyboard keys
*/
void RenderPlayer(RenderWindow &window, int player[], Sprite &playersprite);
int MovePlayer(int player[], int direction);
bool isInPlayerArea(int Position[]);
int HandlePlayerInput(int player[2]);

/*
- Laser
    - SpawnLaser    -> Spawn Laser on the grid from player location
    - RenderLaser   -> Display Laser on the screen
    - MoveLasers    -> Move Lasers ( laser collision implements )
*/
void SpawnLaser(float Lasers[][3], const int player[], Sprite LaserSprites[], Texture &LaserTexture);
void RenderLasers(RenderWindow &window, float Lasers[][3], Sprite LaserSprite[]);
int MoveLasers(float Laser[][3], int **&Mushrooms_Ptr, int &MushroomsCount, int ***&CentipedePtr, int &centipedes_count, char C_Score[]);

/*
- Mushrooms
    - GenerateMushroom  -> Generate a single mushroom [overloaded]
    - GenerateMushrooms -> Generate Mushrooms on initial
    - RenderMushrooms   -> Display Mushrooms on the screen
    - DestructMushroom  -> Decrease 1 life of mushroom
    - DestroyMushroom   -> Delete Mushroom
    - GetMushroom       -> Get the mushroom at specific position
    - PurgeMushrooms    -> Empty heap of mushrooms array
    - GetMushroomsCount -> Returns mushroom count [only active]
*/
void GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, bool ManageHeap = false);
bool GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, int Position[], bool ManageHeap = true);
void GenerateMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount, int n);
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int **&Mushrooms_Ptr, int &MushroomsCount);
void DestructMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[]);
void DestroyMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[], bool updateScore = true);
int GetMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount);
void PurgeMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount);
int GetMushroomsCount(int **Mushrooms_Ptr, int MushroomsCount, int Type);

/*
- Centipede
    - GenerateCentipede -> Generate Centipede in the heap
    - DeleteCentipede   -> Delete Centipede From the heap
    - RenderCentipedes  -> Display All the centipedes on the screeen
    - MoveCentipede     -> Move Centipede on the screen     ( Centipede Collision )
    - GetCentipede      -> Return centipede index at specific location
    - GetCentipedeBodyIndex -> Return centipede's body index based on position
*/
void GenerateCentipede(int ***&centipede_ptr, int size, int Position[], int Direction, int T_Direction, int &centipedes_count, int **PreviousBody = nullptr, bool SplitDown = false);
void DeleteCentipede(int ***&centipede_ptr, int n, int &centipedes_count);
void RenderCentipedes(RenderWindow &Window, Texture CentipedeTexture_HEAD, Texture CentipedeTexture_BODY, int ***&centipede_ptr, int centipedes_count);
int MoveCentipedes(int ***&centipede_ptr, int &centipedes_count, int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[], Clock &CentipedeGenerationClock);
int GetCentipede(int ***&centipedeptr, int centipedes_count, int Position[]);
int GetCentipedeBodyIndex(int ***&centipedeptr, int centipede_n, int Position[]);

/*
- Miscellaneous
    - UpdateGrid    -> Set Object on the grid for collision [overloaded]
    - UpdateScore   -> Update Score     [Static variable used]
    - GetScore      -> Returns current score based on the character array
*/
bool UpdateGrid(int Grid_x, int Grid_y, int object);
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction);
void UpdateScore(int Increase, char C_Score[]);
int GetScore(char C_Score[], char result[]);

/*
    Play Sounds for specific purpose
*/
void PlayLaserSound();
void PlayKillSound();
void PlayCentipedeHeadShotSound();
void PlayCentipedeSplitSound();
void PlayEatSound();
void PlayMovePlayerSound();

int main()
{
    int State = State_Menu;            // Initial State of game
    char C_Score[20] = "Score : 0000"; // Score Character Array
    int levelindex = 1;                // Current Level of the game
    srand(time(0));
    /*
        Window Setup
    */
    RenderWindow window(VideoMode(resolutionX, resolutionY), "Centipede", Style::Close | Style::Titlebar);
    window.setSize(Vector2u(570, 665));
    window.setPosition(Vector2i(400, 0));

    /*
        State Management
    */
    while (State != State_Exit)
    {
        if (State == State_Menu) // Main Menu
        {
            /*
                Rendering Objects for Background
            */
            Texture BackgroundTexture;
            Sprite BackgroundSprite;
            BackgroundTexture.loadFromFile("Screens/Menu.png");
            BackgroundTexture.setSmooth(true);
            BackgroundSprite.setTexture(BackgroundTexture);

            /*
                Background Music
            */
            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/MenuMusic.wav");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(70);
            backgroundMusic.play();

            /*
                Main Menu Screen Loop
            */
            while (window.isOpen() && State == State_Menu)
            {
                window.draw(BackgroundSprite); // Draw Background
                Event e;
                while (window.pollEvent(e))
                {
                    if (e.type == Event::Closed) // Return to exit state
                    {
                        window.close();
                        State = State_Exit;
                    }
                    if (e.key.code == Keyboard::S) // Start the game
                    {
                        State = State_Play;
                        break;
                    }
                    if (e.key.code == Keyboard::E) // Exit Game
                    {
                        State = State_Exit;
                        break;
                    }
                }
                window.display();
                window.clear();
            }
        }
        if (State == State_Play) // Main Game State
        {
            bool levelup = false;
            /*
                Setup Objects For Rendering
                    - Background
                    - Player
                    - Centipede
                    - Mushrooms
                    - Lasers
            */
            Texture BackgroundTexture;
            Sprite BackgroundSprite;
            BackgroundTexture.loadFromFile("Textures/background.png");
            BackgroundTexture.setSmooth(true);
            BackgroundSprite.setTexture(BackgroundTexture);

            Texture PlayerTexture;
            Sprite PlayerSprite;
            PlayerTexture.loadFromFile("Textures/player.png");
            PlayerSprite.setTexture(PlayerTexture);

            Texture CentipedeTexture_HEAD;
            Texture CentipedeTexture_BODY;
            CentipedeTexture_HEAD.loadFromFile("Textures/c_head_left_walk.png");
            CentipedeTexture_BODY.loadFromFile("Textures/c_body_left_walk.png");

            Texture MushroomTexture;
            MushroomTexture.loadFromFile("Textures/mushroom.png");

            Sprite LaserSprites[MAX_LASERS]{};
            Texture LaserTexture;
            LaserTexture.loadFromFile("Textures/bullet.png");
            LaserTexture.setSmooth(true);

            /*
                Background Music
            */
            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/background.ogg");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(50);
            backgroundMusic.play();

            /*
                Setup Data
                    - Player
                    - Centipede
                    - Lasers
                    - Mushrooms
                */

            int Player[2]{};               // Initial Position of Player
            Player[x] = (gameColumns / 2); // Player x
            Player[y] = (gameRows - 5);    // Player y

            int centipedes_count = 0;                // Centipedes count [live update]
            int InitialSize = 12;                    // Initial size of centipede
            int Position[2] = {gameColumns - 3, -1}; // Initial Position of centipede
            bool Initialized = false;                // Used for entering animation
            int ***centipede_ptr = 0;                // Centipede Data Pointer

            float Lasers[MAX_LASERS][3]{}; // Lasers array

            int **Mushrooms_Ptr = NULL; // Mushrooms Data Pointer
            int MushroomsCount = 0;     // Number of mushrooms [live]
            int min_Mushrooms = 20;     // Minimum number of mushrooms

            /*
                Typography
                    - Font
                    - Level
                    - Score
            */
            Font myfont;
            myfont.loadFromFile("Fonts/BebasNeue-Regular.ttf");

            Text T_Level;
            T_Level.setFont(myfont);
            T_Level.setCharacterSize(48);
            T_Level.setPosition(900, 35);

            Text T_Score;
            T_Score.setFont(myfont);
            T_Score.setCharacterSize(48);
            T_Score.setPosition(40, 35);

            /*
                Game Levels loop    [Max levels : 9]
            */
            for (levelindex = 1; levelindex < 10 && State == State_Play; levelindex++)
            {
                levelup = false;                 // reset value
                bool GeneratedPMushroom = false; // bool used for game ending

                UpdateGrid(Player[x], Player[y], OPlayer); // Add Player to grid

                centipede_ptr = new int **[centipedes_count]; // Initialize centipede Array [Base]

                int EnteringPositionX = rand() % (gameColumns - 2); // Radom Entering position for centipede
                Initialized = false;                                // reset value

                int generateMushroomsCount = (rand() % 11) + min_Mushrooms; // Numeber of mushrooms for each level

                /*
                    Initialization
                        - Generate Centipede (Single)
                        - Generate Mushrooms (All)
                */
                GenerateCentipede(centipede_ptr, InitialSize, Position, LEFT, DOWN, centipedes_count);
                GenerateMushrooms(Mushrooms_Ptr, MushroomsCount, generateMushroomsCount);

                /*
                    Clocks
                        - PlayerMovementClock
                        - LaserClock
                        - DeltaClock
                        - CentipedeGenerationClock
                        - CentipedeClock
                */
                Clock PlayerMovementClock;
                Clock LaserClock;
                Clock DeltaClock;
                Clock CentipedeGenerationClock;
                Clock CentipedeClock;

                /*
                    Game Main Loops
                */
                while (window.isOpen() && State == State_Play && levelup == false)
                {
                    /*
                        Calculating Delta Seconds for Speed
                    */
                    Time elapsed = DeltaClock.restart();
                    delta = elapsed.asSeconds();

                    /*
                        -> Handle Events
                    */
                    Event e;
                    while (window.pollEvent(e))
                    {
                        if (e.type == Event::Closed) // Close Window
                        {
                            window.close();
                            State = State_Exit;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::LAlt)) // Cheat Keys for debugging
                        {
                            if (Keyboard::isKeyPressed(Keyboard::P))
                            {
                                levelup = true;
                                break;
                            }
                            if (Keyboard::isKeyPressed(Keyboard::E))
                            {
                                State = State_GameOver;
                                break;
                            }
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Escape)) // Return to main menu
                        {
                            State = State_Menu;
                            break;
                        }
                    }
                    if (centipedes_count == 0) // Level up
                    {
                        if (GeneratedPMushroom)
                        {
                            if (GetMushroomsCount(Mushrooms_Ptr, MushroomsCount, OPMushroom) == 0)
                            {
                                levelup = true;
                            }
                        }
                        else
                        {
                            levelup = true;
                        }
                    }
                    if (PlayerMovementClock.getElapsedTime().asMilliseconds() > 100) // Player Movement
                    {
                        if (HandlePlayerInput(Player) == -1)
                        {
                            State = State_GameOver;
                            break;
                        }
                        PlayerMovementClock.restart();
                    }
                    if (CentipedeClock.getElapsedTime().asMilliseconds() > (100 - (levelindex * 5))) // Centipede Movement
                    {
                        if (MoveCentipedes(centipede_ptr, centipedes_count, Mushrooms_Ptr, MushroomsCount, C_Score, CentipedeGenerationClock) == -1)
                        {
                            State = State_GameOver;
                            break;
                        }
                        if (!Initialized)
                        {
                            if (centipede_ptr[0][0][x] == (EnteringPositionX))
                            {
                                centipede_ptr[0][0][y]++;
                                centipede_ptr[0][0][x]++;
                                UpdateGrid(Position[x], Position[y], ONone);
                                Initialized = true;
                            }
                        }
                        CentipedeClock.restart();
                    }
                    if (LaserClock.getElapsedTime().asMilliseconds() > 200) // Laser Shots
                    {
                        if (Keyboard::isKeyPressed(Keyboard::Z))
                        {
                            PlayLaserSound();
                            SpawnLaser(Lasers, Player, LaserSprites, LaserTexture);
                            LaserClock.restart();
                        }
                    }

                    /*
                        Move Lasers [Delta changes on level]
                    */
                    delta = delta + delta * (levelindex * 0.05);
                    if (MoveLasers(Lasers, Mushrooms_Ptr, MushroomsCount, centipede_ptr, centipedes_count, C_Score) == 1)
                    {
                        GeneratedPMushroom = true;
                    }

                    T_Score.setString(C_Score);               // Set Score Text
                    T_Level.setString(char(levelindex + 48)); // Set Level Text

                    /*
                        -> Render Objects [all]
                    */
                    window.draw(BackgroundSprite);
                    window.draw(T_Level);
                    window.draw(T_Score);
                    RenderPlayer(window, Player, PlayerSprite);
                    RenderCentipedes(window, CentipedeTexture_HEAD, CentipedeTexture_BODY, centipede_ptr, centipedes_count);
                    RenderLasers(window, Lasers, LaserSprites);
                    RenderMushrooms(window, MushroomTexture, Mushrooms_Ptr, MushroomsCount);

                    /*
                         Refresh Frame
                    */
                    window.display();
                    window.clear();
                }
                /*
                    Purge Every array and reset variables
                */
                {
                    for (int i = 0; i < gameRows; i++) // Reset Game Grid
                    {
                        for (int j = 0; j < gameColumns; j++)
                        {
                            gameGrid[i][j] = 0;
                        }
                    }
                    for (int i = 0; i < centipedes_count; i++) // Reset Centipedes
                    {
                        int size = centipede_ptr[i][0][CSize];
                        for (int p = 0; p < size; p++)
                        {
                            delete[] centipede_ptr[i][p];
                        }
                        delete[] centipede_ptr[i];
                    }
                    delete[] centipede_ptr;
                    centipede_ptr = 0;
                    centipedes_count = 0;

                    PurgeMushrooms(Mushrooms_Ptr, MushroomsCount); // Reset Mushroom

                    if (State == State_Play) // Increase Mushrooms Density
                    {
                        min_Mushrooms += 5;
                    }
                }
            }
        }
        if (State == State_GameOver) // Game Over State
        {
            /*
                Background Image
            */
            Texture BackgroundTexture;
            Sprite BackgroundSprite;
            BackgroundTexture.loadFromFile("Screens/GameOver.png");
            BackgroundTexture.setSmooth(true);
            BackgroundSprite.setTexture(BackgroundTexture);

            /*
                Background Music
            */
            Music backgroundMusic;
            backgroundMusic.openFromFile("Music/MenuMusic.wav");
            backgroundMusic.setLoop(true);
            backgroundMusic.setVolume(70);
            backgroundMusic.play();

            /*
                Setup Data
            */
            char C_ScoreOnly[MAX_SCORE_LENGTH + 1]{};
            GetScore(C_Score, C_ScoreOnly);

            /*
                - Font
                - Text
                    - Score
                    - Level
                    - HighScore
            */
            Font textfont;
            textfont.loadFromFile("Fonts/Baumans-Regular.ttf");

            Text Score;
            Score.setFont(textfont);
            Score.setCharacterSize(50);
            Score.setString(C_ScoreOnly); // Set Score String
            Score.setPosition(715 - Score.getLocalBounds().width, 518);

            Text Level;
            Level.setFont(textfont);
            Level.setCharacterSize(50);
            Level.setString(char(levelindex + 47)); // Set Level String
            Level.setPosition(715 - Level.getLocalBounds().width, 594);

            Text HighScore;
            HighScore.setFont(textfont);
            HighScore.setCharacterSize(50);
            HighScore.setString("99999");
            HighScore.setPosition(715 - HighScore.getLocalBounds().width, 670);

            while (window.isOpen() && State == State_GameOver) // Game Over Screen Loop
            {
                Event e;
                while (window.pollEvent(e))
                {
                    if (e.type == Event::Closed) // Close Window
                    {
                        window.close();
                        State = State_Exit;
                    }
                    if (e.key.code == Keyboard::R) // Restart Game
                    {
                        State = State_Play;
                        break;
                    }
                    if (e.key.code == Keyboard::Escape) // Return to main menu
                    {
                        State = State_Menu;
                        break;
                    }
                }

                /*
                    Draw Objects [all]
                */
                window.draw(BackgroundSprite);
                window.draw(Score);
                window.draw(Level);
                window.draw(HighScore);
                window.display();
                window.clear();
            }
        }
        UpdateScore(0, C_Score); // reset Score
    }
    return 0;
}
int HandlePlayerInput(int player[2])
{
    /* IMPORTANT
        This Function returns if player collides with anything except walls
    */

    if (Keyboard::isKeyPressed(Keyboard::Up)) // Move Player UP
        return MovePlayer(player, UP);
    else if (Keyboard::isKeyPressed(Keyboard::Down)) // Move Player Down
        return MovePlayer(player, DOWN);
    else if (Keyboard::isKeyPressed(Keyboard::Left)) // Move Player Left
        return MovePlayer(player, LEFT);
    else if (Keyboard::isKeyPressed(Keyboard::Right)) // Move Player Right
        return MovePlayer(player, RIGHT);
    return 0;
}
bool isInPlayerArea(int Position[])
{
    /* IMPORTANT
        This Function returns if Player is in the Player area or not?
    */

    if (Position[y] >= (gameRows - 5))
        return true;
    return false;
}
int MovePlayer(int player[], int direction)
{
    PlayMovePlayerSound();                   // Play Move Sound
    UpdateGrid(player[x], player[y], ONone); // Reset Current Location

    /*
        Move Based on direction [except walls]
    */
    switch (direction)
    {
    case UP:
        if (player[y] >= (gameRows - 4))
        {
            player[y] -= 1;
        }
        break;
    case DOWN:
        if (player[y] <= (gameRows - 2))
        {
            player[y] += 1;
        }
        break;
    case LEFT:
        if (player[x] >= 1)
        {
            player[x] -= 1;
        }
        break;
    case RIGHT:
        if (player[x] <= (gameColumns - 2))
        {
            player[x] += 1;
        }
        break;
    }
    if (UpdateGrid(player[x], player[y], OPlayer)) // Check Collision
    {
        return -1;
    }
    return 0;
}
void RenderPlayer(RenderWindow &window, int player[], Sprite &playersprite)
{
    playersprite.setPosition(player[x] * boxPixelsX, (player[y] + OFFSET) * boxPixelsY); // Set Position of player
    window.draw(playersprite);
}
void SpawnLaser(float Lasers[][3], const int player[], Sprite LaserSprites[], Texture &LaserTexture)
{
    for (int i = 0; i < MAX_LASERS; i++) // Iterate over all lasers
    {
        if (Lasers[i][exists] == false) // Turn on non-existant laser
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
    for (int i = 0; i < MAX_LASERS; i++) // Render all the existant lasers
    {
        if (Lasers[i][exists] == true)
        {
            LaserSprite[i].setPosition(Lasers[i][x] * boxPixelsX, (Lasers[i][y] + OFFSET) * boxPixelsY);
            window.draw(LaserSprite[i]);
        }
    }
}
int MoveLasers(float Laser[][3], int **&Mushrooms_Ptr, int &MushroomsCount, int ***&CentipedePtr, int &centipedes_count, char C_Score[])
{
    int return_response = 0;             // poisonous mushroom spawned ?
    for (int i = 0; i < MAX_LASERS; i++) // Move All lasers
    {
        if (Laser[i][exists] == true) // Move only existant
        {
            int Position[] = {int(Laser[i][x]), int(ceil(Laser[i][y]))}; // Position of laser
            if (gameGrid[Position[y]][Position[x]] != OPlayer)           // Reset Grid
            {
                UpdateGrid(Position[x], Position[y], ONone);
            }
            Laser[i][y] -= 30 * delta; // Mover Laser

            if (Laser[i][y] < -1) // Destroy Laser
                Laser[i][exists] = false;

            else if (Laser[i][y] > -1)
            {
                int collided_object = 0;                   // Collision detection object
                Position[x] = int(Laser[i][x]);            // Position X [new]
                Position[y] = int(floor(Laser[i][y]) + 1); // Position Y [new]

                if (UpdateGrid(Position[x], Position[y], OLaser, collided_object, UP) && collided_object != OPlayer) // Check Collision
                {
                    switch (collided_object) // Check collided object
                    {
                    case ODMushroom: // Handle Mushroom Collision
                    case OPMushroom:
                        PlayKillSound();
                        DestructMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score);
                        break;
                    case OCentipede:
                    {
                        int centipede_n = GetCentipede(CentipedePtr, centipedes_count, Position); // Hit centipede index
                        if (centipede_n == -1)                                                    // Not Found
                            break;

                        int size = CentipedePtr[centipede_n][0][CSize]; // Size of centipede

                        int body_index = GetCentipedeBodyIndex(CentipedePtr, centipede_n, Position); // Hit Body index
                        if (body_index == -1)                                                        // Not Found
                            break;

                        int Direction = CentipedePtr[centipede_n][0][CDirection];   // Direction of centipede
                        int T_Direction = CentipedePtr[centipede_n][0][TDirection]; // Transcending Direction

                        if (body_index == 0) // Handle headshot
                        {
                            DeleteCentipede(CentipedePtr, centipede_n, centipedes_count);
                            UpdateScore(20, C_Score);
                            PlayCentipedeHeadShotSound();
                        }
                        else // Split Centipede
                        {
                            PlayCentipedeSplitSound();
                            UpdateScore(10, C_Score);

                            int **PreviousDataP1 = new int *[body_index];        // Centipede Hit First Part
                            int **PreviousDataP2 = new int *[size - body_index]; // Centipede Hit Second Part

                            for (int l = 0; l < body_index; l++) // Copy First Part
                            {
                                PreviousDataP1[l] = new int[CentipedeBodyDataSize];
                                PreviousDataP1[l][x] = CentipedePtr[centipede_n][l][x];
                                PreviousDataP1[l][y] = CentipedePtr[centipede_n][l][y];
                                PreviousDataP1[l][CDirection] = CentipedePtr[centipede_n][l][CDirection];
                            }

                            for (int l = 0; l < size - body_index; l++) // Copy Second Part
                            {
                                PreviousDataP2[l] = new int[CentipedeBodyDataSize];
                                PreviousDataP2[l][x] = CentipedePtr[centipede_n][l + body_index][x];
                                PreviousDataP2[l][y] = CentipedePtr[centipede_n][l + body_index][y];
                                PreviousDataP2[l][CDirection] = CentipedePtr[centipede_n][l + body_index][CDirection];
                            }

                            DeleteCentipede(CentipedePtr, centipede_n, centipedes_count); // Delete hit centipede

                            Position[x] += (Direction == LEFT) ? (-body_index) : (body_index); // First Part Position
                            GenerateCentipede(CentipedePtr, body_index, Position, Direction, T_Direction, centipedes_count, PreviousDataP1);

                            Position[x] += (Direction == LEFT) ? (body_index) : (-body_index); // Second Part Position
                            GenerateCentipede(CentipedePtr, size - body_index, Position, Direction, T_Direction, centipedes_count, PreviousDataP2, true);

                            UpdateGrid(Position[x], Position[y], ONone); // Update Grid

                            /*
                                Purge Heap Memory
                                    - Part 1
                                    - Part 2
                            */
                            for (int l = 0; l < body_index; l++)
                                delete[] PreviousDataP1[l];
                            delete[] PreviousDataP1;

                            for (int l = 0; l < size - body_index; l++)
                                delete[] PreviousDataP2[l];
                            delete[] PreviousDataP2;
                        }
                        if (isInPlayerArea(Position)) // Generate Poisonous Mushrooms
                        {
                            GenerateMushroom(Mushrooms_Ptr, MushroomsCount, OPMushroom, Position);
                            return_response = 1;
                        }
                        break;
                    }
                    }
                    Laser[i][exists] = false; // Destroy Mushroom
                }
            }
        }
    }
    return return_response;
}
void DestructMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[])
{
    /* IMPORTANT
        This function decreases the life of the centipede and deletes if it reaches to health 0
    */

    int MushroomIndex = GetMushroom(Position, Mushrooms_Ptr, MushroomsCount);
    Mushrooms_Ptr[MushroomIndex][health] -= 1;
    if (Mushrooms_Ptr[MushroomIndex][health] == 0)
        DestroyMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score);
}
void DestroyMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[], bool updateScore)
{
    /* IMPORTANT
        This function destroys the mushroom
    */

    Mushrooms_Ptr[GetMushroom(Position, Mushrooms_Ptr, MushroomsCount)][health] = 0;
    UpdateGrid(Position[x], Position[y], ONone);
    if (updateScore)
        UpdateScore(1, C_Score);
}
void PurgeMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount)
{
    /* IMPORTANT
        This function Resets the mushrooms
    */
    for (int i = 0; i < MushroomsCount; i++)
    {
        delete[] Mushrooms_Ptr[i];
    }
    delete[] Mushrooms_Ptr;
    Mushrooms_Ptr = 0;
    MushroomsCount = 0;
}
void GenerateMushroom(int **&Mushrooms_Ptr, int &MushroomsCount, int Type, bool ManageHeap)
{
    bool Created = false;
    do // Spawn Mushroom if not done
    {
        int Grid_X = rand() % gameColumns;    // Random X Position
        int Grid_Y = rand() % (gameRows - 5); // Random Y Position

        if (gameGrid[Grid_Y][Grid_X] == ONone) // Check if Position is empty or not
        {
            MushroomsCount++;
            if (ManageHeap) // Manage heap [Copies previous values to newer pointer]
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

            UpdateGrid(Grid_X, Grid_Y, Type); // Update Grid

            /*
                Initialize Data
            */
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
    /* IMPORTANT
        This function is overload, this creates mushroom only on specific position
    */
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
void GenerateMushrooms(int **&Mushrooms_Ptr, int &MushroomsCount, int n)
{
    Mushrooms_Ptr = new int *[n]; // Initialize Mushroom Pointer
    for (int i = 0; i < n; i++)   // Generate n mushrooms
    {
        Mushrooms_Ptr[i] = new int[4];
        GenerateMushroom(Mushrooms_Ptr, MushroomsCount, ODMushroom);
    }
}
void RenderMushrooms(RenderWindow &Window, Texture &MushroomTexture, int **&Mushrooms_Ptr, int &MushroomsCount)
{
    for (int i = 0; i < MushroomsCount; i++) // Render all Mushrooms
    {
        if (Mushrooms_Ptr[i][health] != 0) // Not Destroyed
        {
            Sprite mushroom;
            mushroom.setTexture(MushroomTexture);

            int TextureTop = 0, TextureLeft = 0; // For offsetting

            if (Mushrooms_Ptr[i][MType] == ODMushroom) // Change Texture based on type
                TextureTop = boxPixelsY;

            if (Mushrooms_Ptr[i][health] == 1) // Change Texture based on health
                TextureLeft = boxPixelsX;

            /*
                Render Mushroom
            */
            mushroom.setTextureRect(IntRect(TextureLeft, TextureTop, boxPixelsX, boxPixelsY));
            mushroom.setPosition(Mushrooms_Ptr[i][x] * boxPixelsX, (Mushrooms_Ptr[i][y] + OFFSET) * boxPixelsY);
            Window.draw(mushroom);
        }
    }
}
int GetMushroom(int Position[], int **&Mushrooms_Ptr, int &MushroomsCount)
{
    for (int i = 0; i < MushroomsCount; i++) // Return Corresponding Mushroom
        if (Mushrooms_Ptr[i][x] == Position[x] && Mushrooms_Ptr[i][y] == Position[y])
            return i;
    return 0;
}
int GetMushroomsCount(int **Mushrooms_Ptr, int MushroomsCount, int Type)
{
    int count = 0;
    for (int i = 0; i < MushroomsCount; i++) // Return Active Mushrooms
        if (Mushrooms_Ptr[i][health] > 0 && Mushrooms_Ptr[i][MType] == Type)
            count++;
    return count;
}
bool UpdateGrid(int Grid_x, int Grid_y, int object)
{
    if (Grid_x < gameColumns && Grid_x >= 0 && Grid_y < gameRows && Grid_y >= 0) // Check if is in grid
    {
        if (object != ONone) // Check if not nothing
        {
            if (gameGrid[Grid_y][Grid_x] != ONone) // Check Collision
            {
                return true;
            }
        }
        gameGrid[Grid_y][Grid_x] = object; // Commit change
    }
    return false;
}
bool UpdateGrid(int Grid_x, int Grid_y, int object, int &collidedObject, int Direction)
{
    if (Grid_x < gameColumns && Grid_x >= 0 && Grid_y < gameRows && Grid_y >= 0) // Check if is in grid
    {
        switch (Direction) // Check wall collision
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

        if (object != ONone) // Check object collision
        {
            if (gameGrid[Grid_y][Grid_x] != ONone)
            {
                collidedObject = gameGrid[Grid_y][Grid_x];
                return true;
            }
        }
        gameGrid[Grid_y][Grid_x] = object; // Commit change
    }
    return false;
}

void GenerateCentipede(int ***&centipede_ptr, int size, int Position[], int Direction, int T_Direction, int &centipedes_count, int **PreviousBody, bool SplitDown)
{
    centipedes_count++; // Increase Centipede Count

    int ***D_temp = 0;                     // Temp 3D array for centipede
    D_temp = new int **[centipedes_count]; // Initialize

    for (int i = 0; i < centipedes_count - 1; i++) // Copy centipdes [previous]
    {
        D_temp[i] = centipede_ptr[i];
    }

    D_temp[centipedes_count - 1] = new int *[size];                // Create new centipede
    D_temp[centipedes_count - 1][0] = new int[5];                  // Create Data for head
    *(D_temp[centipedes_count - 1][0] + CSize) = size;             // Initialize size of centipede
    *(D_temp[centipedes_count - 1][0] + TDirection) = T_Direction; // Initialize Transending Direction

    if (!SplitDown) // Initialize Data
    {
        *(D_temp[centipedes_count - 1][0] + x) = Position[x];
        *(D_temp[centipedes_count - 1][0] + y) = Position[y];
        *(D_temp[centipedes_count - 1][0] + CDirection) = Direction;
    }
    else // Initialize Data [Split]
    {
        *(D_temp[centipedes_count - 1][0] + x) = Position[x] + ((Direction == LEFT) ? (1) : (-1));
        *(D_temp[centipedes_count - 1][0] + y) = Position[y] + ((T_Direction == DOWN) ? (1) : (-1));
        *(D_temp[centipedes_count - 1][0] + CDirection) = (Direction == LEFT) ? (RIGHT) : (LEFT);
    }

    UpdateGrid(Position[x], Position[y], OCentipede); // Update Grid

    for (int j = 1; j < size; j++) // Iterate Body Segments
    {
        D_temp[centipedes_count - 1][j] = new int[3]; // Create Data for body Segments

        if (PreviousBody == nullptr) // Initialize centipede Body
        {
            *(D_temp[centipedes_count - 1][j] + y) = Position[y];
            *(D_temp[centipedes_count - 1][j] + CDirection) = Direction;
            if (Direction == LEFT)
            {
                *(D_temp[centipedes_count - 1][j] + x) = Position[x] + j;
                UpdateGrid(Position[x] + j, Position[y], OCentipede);
            }
            else
            {
                *(D_temp[centipedes_count - 1][j] + x) = Position[x] - j;
                UpdateGrid(Position[x] - j, Position[y], OCentipede);
            }
        }
        else // Copy previous body   [Used for splitting purpose]
        {
            *(D_temp[centipedes_count - 1][j] + x) = PreviousBody[j][x];
            *(D_temp[centipedes_count - 1][j] + y) = PreviousBody[j][y];
            *(D_temp[centipedes_count - 1][j] + CDirection) = PreviousBody[j][CDirection];
            UpdateGrid(PreviousBody[j][x], PreviousBody[j][y], OCentipede);
        }
    }
    /*
        Memory reset
    */
    delete[] centipede_ptr;
    centipede_ptr = D_temp;
    D_temp = 0;
}

void DeleteCentipede(int ***&centipede_ptr, int n, int &centipedes_count)
{
    int size_of_centipede = centipede_ptr[n][0][CSize]; // Size of centipede

    for (int j = 0; j < size_of_centipede; j++) // Iterate through Body Segments
    {
        UpdateGrid(centipede_ptr[n][j][x], centipede_ptr[n][j][y], ONone); // Resets grid
        delete[] centipede_ptr[n][j];                                      // Free Segment body
    }

    delete[] centipede_ptr[n]; // Delete nth Centipede
    centipede_ptr[n] = 0;      // reset pointer

    int ***D_temp = 0; // Temp array for removing extra space

    if (centipedes_count != 1) // If other centipedes exist
        D_temp = new int **[centipedes_count - 1];

    for (int i = 0, j = 0; i < centipedes_count; i++) // Copy values to new temp pointer
    {
        if (centipede_ptr[i] != 0) // NOT EMPTY POINTER
            D_temp[j] = centipede_ptr[i], j++;
    }

    delete[] centipede_ptr; // Free Centipede Pointer
    centipede_ptr = D_temp; // Reassign new Pointer
    centipedes_count--;     // Decrease Count
}
void RenderCentipedes(RenderWindow &Window, Texture CentipedeTexture_HEAD, Texture CentipedeTexture_BODY, int ***&centipede_ptr, int centipedes_count)
{
    for (int i = 0; i < centipedes_count; i++)
    {
        int size = centipede_ptr[i][0][CSize];

        /*
            HEAD Rendering
        */
        {
            Sprite head;
            int X = (centipede_ptr[i][0][x]);
            int Y = (centipede_ptr[i][0][y]);
            head.setTexture(CentipedeTexture_HEAD);
            head.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            head.setPosition(X * boxPixelsX, (Y + OFFSET) * boxPixelsY);
            if (centipede_ptr[i][0][CDirection] == RIGHT)
                head.setOrigin(boxPixelsX, boxPixelsY), head.setRotation(180);
            Window.draw(head);
        }
        /*
            Body Rendering
        */
        for (int j = 1; j < size; j++)
        {
            Sprite segment;
            int X = (centipede_ptr[i][j][x]);
            int Y = (centipede_ptr[i][j][y]);
            segment.setTexture(CentipedeTexture_BODY);
            segment.setTextureRect(IntRect(0, 0, boxPixelsX, boxPixelsY));
            segment.setPosition(X * boxPixelsX, (Y + OFFSET) * boxPixelsY);
            if (centipede_ptr[i][j][CDirection] == RIGHT)
                segment.setOrigin(boxPixelsX, boxPixelsY), segment.setRotation(180);
            Window.draw(segment);
        }
    }
}
int MoveCentipedes(int ***&centipede_ptr, int &centipedes_count, int **&Mushrooms_Ptr, int &MushroomsCount, char C_Score[], Clock &CentipedeGenerationClock)
{
    const int step_size = 1; // Step of centipede Movement [GRID UNITS]

    for (int i = 0; i < centipedes_count; i++) // Move All centipedes
    {
        bool direction_changed = false; // Direction changed
        int collided_object = 0;        // Collided Object

        int size = centipede_ptr[i][0][CSize];             // Size of centipede
        int Direction = centipede_ptr[i][0][CDirection];   // Direction of centipede
        int T_direction = centipede_ptr[i][0][TDirection]; // Transending Direction

        int Position[2] = {centipede_ptr[i][0][x], centipede_ptr[i][0][y]}; // Position of centipede's head

        int PreviousObject = gameGrid[Position[y]][Position[x]]; // Previous Object for collision
        int NextObject = ONone;

        /*
            Calculate Next object
        */
        if ((Position[x] < (gameColumns - 1) && Position[x] > 0) && (Position[y] < gameRows && Position[y] > 0))
            NextObject = gameGrid[Position[y]][Position[x] + ((Direction == RIGHT) ? (1) : (-1))];
        else
            NextObject = gameGrid[Position[y] + ((T_direction == DOWN) ? (1) : (-1))][Position[x]];

        /*
            Reset Grid
        */
        for (int j = 0; j < size; j++)
            UpdateGrid(centipede_ptr[i][j][x], centipede_ptr[i][j][y], ONone);

        /*
            Body Follow Head
        */
        for (int j = size - 1; j > 0; j--)
        {
            centipede_ptr[i][j][x] = centipede_ptr[i][j - 1][x];
            centipede_ptr[i][j][y] = centipede_ptr[i][j - 1][y];
            centipede_ptr[i][j][CDirection] = centipede_ptr[i][j - 1][CDirection];
        }

        /*
            Collision Conditions
        */
        bool Collision = (PreviousObject == ODMushroom || NextObject == ODMushroom || PreviousObject == OPlayer || NextObject == OPMushroom || PreviousObject == OPMushroom);
        if (isInPlayerArea(Position))
        {
            if (CentipedeGenerationClock.getElapsedTime().asSeconds() > 10) // Generate Heads in player area
            {
                int Position[2] = {gameColumns, gameRows - ((rand() % 5) + 1)};
                GenerateCentipede(centipede_ptr, 1, Position, LEFT, DOWN, centipedes_count);
                CentipedeGenerationClock.restart();
            }
        }
        if (Collision || UpdateGrid(Position[x], Position[y], OCentipede, collided_object, Direction)) // Check Collision
        {
            if (collided_object != OCentipede) // NOT CENTIPEDE [TO AVOID SELF COLLISION]
            {
                if (PreviousObject == ODMushroom || PreviousObject == OPMushroom) // Handle Mushroom Collision
                {
                    PlayEatSound();
                    DestroyMushroom(Position, Mushrooms_Ptr, MushroomsCount, C_Score, false);
                }
                if (PreviousObject == OPlayer) // Handle Player Collision
                {
                    return -1;
                }

                centipede_ptr[i][0][CDirection] = ((centipede_ptr[i][0][CDirection]) == RIGHT) ? (LEFT) : (RIGHT); // Change Direction

                if (isInPlayerArea(Position)) // Change Transending Direction at th bottom
                {
                    if (Position[y] == (gameRows - 1))
                    {
                        centipede_ptr[i][0][TDirection] = UP;
                    }
                }
                else
                {
                    centipede_ptr[i][0][TDirection] = DOWN;
                }

                /*
                    Move Downward [Change Direction]
                */
                if (centipede_ptr[i][0][TDirection] == DOWN)
                    centipede_ptr[i][0][y] += step_size;
                else
                    centipede_ptr[i][0][y] -= step_size;
                Direction = centipede_ptr[i][0][CDirection]; // Updated Direction
                direction_changed = true;
            }
        }
        /*
            Below Lines are coded intentionally to match the speed of each centipede
        */
        if (!direction_changed) // reset grid
            UpdateGrid(centipede_ptr[i][0][x], centipede_ptr[i][0][y], ONone);

        if (!direction_changed) // move centipede
        {
            if (Direction == LEFT)
                centipede_ptr[i][0][x] -= step_size;
            else
                centipede_ptr[i][0][x] += step_size;
        }

        for (int j = 0; j < size; j++) // Update Grid
        {
            if (!direction_changed)
                UpdateGrid(centipede_ptr[i][j][x], centipede_ptr[i][j][y], OCentipede);
        }
    }
    return 0;
}
int GetCentipede(int ***&centipedeptr, int centipedes_count, int Position[])
{
    /* IMPORTANT
        This Function return the centipede based on position
    */
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
    /* IMPORTANT
        This function returns the centipede body index based on position
    */
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
    /* IMPORTANT
        This function Increases the score
        Reset's the score if 0 is increase
    */

    static int Score = 0; // Main Score Static Variable
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
    /* IMPORTANT
        This function return score extracted from character array
    */

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