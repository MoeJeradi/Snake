#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
#include <string>

using namespace std;


Color orange = {255,165,0}; // set color background before the game
Color CoralOrange = {255,127,80,255}; // Food color // 255 has to be the last color code FOR THIS!


int cellSize = 26; //grid ( 25 * 26 = 650)
int cellCount = 25; // grid (cellcount * cell SIZE will give you the pixel size  )
int offset = 75;  // border

double lastUpdateTime = 0; // used to keep track of the time the snake moved 

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i=0; i < deque.size(); i++ ){
        if(Vector2Equals(deque[i], element))
        {
            return true;
        }
    }return false;
}

bool eventTriggered(double interval) // loop making the snake move at a certain interval (WILL SET MOVEMENT KEYS IN the game loop INT MAIN())
{

    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}


class Snake {

        public: 
        deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSegment = false;

        void Draw()
        {

            for(unsigned int i = 0; i < body.size(); i++)
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.75, 6, CoralOrange); // Adds a roundness to the snake rectangle 
        
            }

        }

        void Update(){
                 body.push_front(Vector2Add(body[0], direction));
                if(addSegment == true){
                        addSegment = false;
                }else
                {
                    body.pop_back();
                }
            // moves snakes ass to the front 
            

        }

void Reset(){
    body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
    direction = {1,0};
}

};


class Food {

    public:
        Vector2 position;
        Texture2D texture; // use texture 2D to better CPU procces an image and loads textures clear and faster 
    

                // constructor
            Food(deque<Vector2> snakeBody){

                Image image = LoadImage("Graphics/cartman.png"); // this loads image it is in a constructor that you put ina class a constructor initializes an object when its created
                texture = LoadTextureFromImage(image); 
                UnloadImage(image);
                position = GenerateRandomPos(snakeBody); // free some memory from texture 2D
            }


            // Destructor (Releases any resources the object aqquired (CONSTRUCTOR))
            ~Food(){
                        UnloadTexture(texture);

                }


        void Draw(){

                DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE); 

        }


            Vector2 GenerateRandomCell()
            {
                float x = GetRandomValue(0, cellCount -1);   
                float y = GetRandomValue(0, cellCount -1); 
                return Vector2{x,y};
            }

            Vector2 GenerateRandomPos(deque<Vector2> snakeBody) // instead of having a fixed position it randomizes the FOOD 
            {
                
                Vector2 position = GenerateRandomCell();
                while(ElementInDeque(position, snakeBody))
                {
                        position = GenerateRandomCell();
                }   
                return position;

            }


};

class FontManager {
public:
    Font myFont;
    

    FontManager() {
        // Load your custom font here

        myFont = LoadFont("Font/Starborn.otf");



    }
    
    ~FontManager() {
        UnloadFont(myFont); 
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatWallSound; 



    void Soundvolume(float volume){ // when youput a float or int inside of parentheses of a functiom you can just add it in the main function
    SetSoundVolume(eatWallSound, volume);
}

    Game(){
        InitAudioDevice();
        eatWallSound = LoadSound("Sounds/Timmy.mp3");
    }

    ~Game(){
        UnloadSound(eatWallSound);
        CloseAudioDevice();

    }


    void Draw(){
        food.Draw();
        snake.Draw();
    }

    void Update(){
        if(running)
        {
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithEdges();
        CheckCollisionWithTail();
    }
    }


    void CheckCollisionWithFood()
    { //checks if the food is in the same CELL (GRID)

            if(Vector2Equals(snake.body[0], food.position))
            {
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score ++;
                PlaySound(eatWallSound);
            }

    }


void CheckCollisionWithEdges()
{
if(snake.body[0].x /*Head of the snake is[0]*/ == cellCount|| snake.body[0].x == -1 )
{

GameOver();
}
if(snake.body[0].y == cellCount || snake.body[0].y == -1)
{
GameOver();
}
}

void GameOver()

{
   snake.Reset();
   food.position = food.GenerateRandomPos(snake.body);
   running = false;
   score = 0;
       PlaySound(eatWallSound);
}

void CheckCollisionWithTail(){

    deque<Vector2> headlessBody = snake.body;
    headlessBody.pop_front();
    if(ElementInDeque(snake.body[0],headlessBody))
    {
        GameOver();
    }



}


            

};


class MainMenu {
public:
    bool startPressed = false;
    FontManager& fontManager; // Reference to FontManager

    MainMenu(FontManager& _fontManager) : fontManager(_fontManager) {} // Constructor to initialize FontManager reference

    void Draw() {
    if (!startPressed) {
        const char* buttonText = "Enter";
        int fontSize = 30;

        // Calculate text width and height with the specific font
        int textWidth = MeasureTextEx(fontManager.myFont, buttonText, (float)fontSize, 1).x;
        int textHeight = fontSize;

        // Add increased padding to the text width and height to create a larger margin around the text
        int paddingX = 20;  // Increased padding horizontally
        int paddingY = 10;  // Increased padding vertically

        // Calculate the button width and height based on text size and increased padding
        int buttonWidth = textWidth + 2 * paddingX;
        int buttonHeight = textHeight + 2 * paddingY;

        // Calculate the position for the button to be centered horizontally
        int buttonX = (GetScreenWidth() - buttonWidth) / 2;
        int buttonY = GetScreenHeight() / 2 - buttonHeight / 2;

        // Create a rectangle around the text to act as a button
        Rectangle buttonRect = { (float)buttonX, (float)buttonY, (float)buttonWidth, (float)buttonHeight };

        // Check if the mouse is over the button area
        if (CheckCollisionPointRec(GetMousePosition(), buttonRect)) {
            // Draw filled button when hovered
            DrawRectangleRec(buttonRect, PURPLE);
        } else {
            // Draw outlined button when not hovered
            DrawRectangleLinesEx(buttonRect, 3, WHITE);
        }

        // Calculate the position for the text to be centered within the button
        int textX = buttonX + paddingX;
        int textY = buttonY + paddingY;

        // Draw the text on top of the button
        DrawTextEx(fontManager.myFont, buttonText, Vector2{(float)textX, (float)textY}, fontSize, 0, WHITE);

        // Check for mouse click within the button area
        if (CheckCollisionPointRec(GetMousePosition(), buttonRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            startPressed = true;
        }
    }
}
};
int main () {

        cout << "starting the game..." << endl;
        InitWindow(2*offset + cellSize * cellCount,2*offset + cellSize * cellCount, "Mohamad's Snake Game"); // Sets the canvas and the size and the name of the GAME
        SetTargetFPS(60); // THE FPS THE USER AUTOMATICALLY HAS TO GET CUH

        
        Game game = Game();
        FontManager fontManager;
        MainMenu mainMenu(fontManager);
        game.Soundvolume(0.1);
        
        while (mainMenu.startPressed == false && WindowShouldClose() == false) {
            BeginDrawing();

                ClearBackground(orange);
                    DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, WHITE);
             
           

                mainMenu.Draw();
               
         if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        mainMenu.startPressed = true;}
        if (IsKeyPressed(KEY_ENTER)) {
            mainMenu.startPressed = true;
        }

        EndDrawing();
    }

        while(WindowShouldClose() == false) // if i press esc the game loop ends or if i press X the game loop ends
        {

            BeginDrawing(); // blank canvas so we can draw our game objects on

                if(eventTriggered(0.2)){
                    
                    game.Update();
                }

                if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
                {
                    game.snake.direction = {0, -1};
                    game.running = true; 
                }
                if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
                    game.snake.direction = {0, 1};
                    game.running = true; 
                }
                 if(IsKeyPressed(KEY_LEFT)  && game.snake.direction.x != 1){
                    game.snake.direction = {-1, 0};
                    game.running = true; 
                }
                 if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
                    game.snake.direction = {1, 0};
                    game.running = true; 
                }

                
                if(IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
                {
                    game.snake.direction = {0, -1};
                    game.running = true; 
                }
                if(IsKeyPressed(KEY_S) && game.snake.direction.y != -1){
                    game.snake.direction = {0, 1};
                    game.running = true; 
                }
                 if(IsKeyPressed(KEY_A)  && game.snake.direction.x != 1){
                    game.snake.direction = {-1, 0};
                    game.running = true; 
                }
                 if(IsKeyPressed(KEY_D) && game.snake.direction.x != -1){
                    game.snake.direction = {1, 0};
                    game.running = true; 
                }
               


                //DRAWIN COLOR FOR BACKGROUND || This code clears the background and changes it to your color code!
                ClearBackground(orange);
                DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float) cellSize*cellCount + 10, (float)cellSize*cellCount+10}, 5, WHITE);

                mainMenu.Draw();

                int textWidth = MeasureText("Moe's Snake Game", 40);
                int textX = (GetScreenWidth() - textWidth) / 2;

                DrawTextEx(fontManager.myFont, "Moe's Snake Game", Vector2{(float)(textX - 2), 18}, 40, 0, WHITE);
                DrawTextEx(fontManager.myFont, "Moe's Snake Game", Vector2{(float)(textX + 2), 22}, 40, 0, WHITE);
                 DrawTextEx(fontManager.myFont, "Moe's Snake Game", Vector2{(float)(textX), 20}, 40, 0, BLACK);

        // DrawTextEx with stroke (outline) for the score
                    DrawTextEx(fontManager.myFont, TextFormat("Score - %i", game.score), Vector2{(float)(offset - 7), (float)(offset + cellSize * cellCount + 10)}, 40, 0, WHITE);
                        DrawTextEx(fontManager.myFont, TextFormat("Score - %i", game.score), Vector2{(float)(offset - 3), (float)(offset + cellSize * cellCount + 10)}, 40, 0, WHITE);
                             DrawTextEx(fontManager.myFont, TextFormat("Score - %i", game.score), Vector2{(float)(offset - 5), (float)(offset + cellSize * cellCount + 12)}, 40, 0, BLACK);
            
                game.Draw();


                  


            EndDrawing(); // The end of the canvas 
        }


            CloseWindow();

    return 0;
}