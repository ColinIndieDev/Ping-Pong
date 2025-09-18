#include <cmath>
#include <iostream>

#include "raylib.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600

class Player
{
    public:
    int width = 30;
    int height = SCREEN_HEIGHT / 4;

    float offset = 8;
    float posX = 0 + offset;
    float posY = SCREEN_HEIGHT / 2;
    float speed = 1000;

    Color color = {255, 0, 0, 255};

    void Draw() const
    {
        Rectangle rectangle = Rectangle{posX, posY, (float)width, (float)height};
        DrawRectangleRounded(rectangle, 2, 1, color);
    }

    void Update()
    {
        if (IsKeyDown(KEY_W))
        {
            posY -= speed * GetFrameTime();
        }
        else if (IsKeyDown(KEY_S))
        {
            posY += speed * GetFrameTime();
        }

        if (posY < 0)
        {
            posY = 0;
        }
        else if (posY + height > SCREEN_HEIGHT)
        {
            posY = SCREEN_HEIGHT - height;
        }
    }

};

class Player2
{
public:
    int width = 30;
    int height = SCREEN_HEIGHT / 4;

    float offset = 8;
    float posX = SCREEN_WIDTH - width - offset;
    float posY = SCREEN_HEIGHT / 2;
    float speed = 1000;

    Color color = {0, 0, 255, 255};

    void Draw() const
    {
        Rectangle rectangle = Rectangle{posX, posY, (float)width, (float)height};
        DrawRectangleRounded(rectangle, 2, 1, color);
    }

    void Update()
    {
        if (IsKeyDown(KEY_UP))
        {
            posY -= speed * GetFrameTime();
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            posY += speed * GetFrameTime();
        }

        if (posY < 0)
        {
            posY = 0;
        }
        else if (posY + height > SCREEN_HEIGHT)
        {
            posY = SCREEN_HEIGHT - height;
        }
    }

};

class Ball
{
    public:
    float radius = 20;
    float speedX = 500, speedY = speedX;

    float posX = SCREEN_WIDTH / 2;
    float posY = SCREEN_HEIGHT / 2;

    bool isRestarting = true;

    void Draw() const
    {
        DrawCircle((int)posX, (int)posY, radius, WHITE);
    }

    void Update()
    {
        posX += speedX * GetFrameTime();
        posY += speedY * GetFrameTime();

        if (isRestarting)
        {
            float factor = 0.025f * GetFrameTime();
            float additionX = speedX < 0 ? speedX : -speedX;
            float additionY = speedY < 0 ? speedY : -speedY;

            speedX = speedX + speedX * factor;
            speedY = speedY + speedY * factor;
        }
    }

    void Reset()
    {
        isRestarting = false;
        speedX = speedY = 500;

        int randomDirX = GetRandomValue(0, 1) == 0 ? -1 : 1;
        int randomDirY = GetRandomValue(0, 1) == 0 ? -1 : 1;

        posX = static_cast<float>(SCREEN_WIDTH) / 2;
        posY = static_cast<float>(SCREEN_HEIGHT) / 2;
        speedX *= randomDirX, speedY *= randomDirY;

        isRestarting = true;
    }

    void checkCollisionWithEnd()
    {
        if (posX >= SCREEN_WIDTH - radius)
        {
            posX = SCREEN_WIDTH - radius;
            speedX *= -1;
        }
        else if (posX <= 0 - radius)
        {
            posX = radius;
            speedX *= -1;
        }

        if (posY >= SCREEN_HEIGHT - radius)
        {
            posY = SCREEN_HEIGHT - radius;
            speedY *= -1;
        }
        else if (posY <= 0 + radius)
        {
            posY = radius;
            speedY *= -1;
        }
    }
};

class Decoration
{
    public:
    static void Draw()
    {
        DrawLine(static_cast<float>(SCREEN_WIDTH) / 2, 0, static_cast<float>(SCREEN_WIDTH) / 2, static_cast<float>(SCREEN_HEIGHT), WHITE);
        DrawCircleLines(0, 200, static_cast<float>(SCREEN_HEIGHT) / 2, RED);
        DrawCircleLines(static_cast<float>(SCREEN_WIDTH), 200, static_cast<float>(SCREEN_HEIGHT) / 2, BLUE);
    }
};

int scorePlayer1 = 0;
int scorePlayer2 = 0;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ping Pong");

    Image iconImage = LoadImage("../assets/images/icon.png");
    ImageFormat(&iconImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    SetWindowIcon(iconImage);
    UnloadImage(iconImage);

    // Start
    auto player = Player();
    auto player2 = Player2();
    auto ball = Ball();

    while (!WindowShouldClose())
    {
        // Loop
        player.Update();
        player2.Update();
        ball.Update();
        player.speed = static_cast<float>(abs(static_cast<int>(ball.speedX)));
        player2.speed = static_cast<float>(abs(static_cast<int>(ball.speedX)));
        ball.checkCollisionWithEnd();
        if (IsKeyPressed(KEY_ESCAPE)) WindowShouldClose();

        if (ball.posX <= player.offset + static_cast<float>(player.width) / 2)
        {
            scorePlayer2++;
            ball.Reset();
        }
        else if (ball.posX >= SCREEN_WIDTH - (player2.offset + static_cast<float>(player2.width) / 2))
        {
            scorePlayer1++;
            ball.Reset();
        }

        // Collisions
        if (CheckCollisionCircleLine(Vector2{ball.posX, ball.posY}, 10,
            Vector2{player.posX + static_cast<float>(player.width), player.posY},
            Vector2{player.posX + static_cast<float>(player.width), player.posY + static_cast<float>(player.height)}))
        {
            ball.posX = player.posX + static_cast<float>(player.width) + ball.radius;
            ball.speedX *= -1;
        }

        if (CheckCollisionCircleLine(Vector2{ball.posX, ball.posY}, 10, Vector2{(float)SCREEN_WIDTH - player2.width, player2.posY}, Vector2{(float)SCREEN_WIDTH - player2.width, player2.posY + player2.height}))
        {
            ball.posX = player2.posX - ball.radius;
            ball.speedX *= -1;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        Decoration::Draw();
        player.Draw();
        player2.Draw();
        ball.Draw();

        // Texts
        DrawText(TextFormat("FPS: %i", GetFPS()), SCREEN_WIDTH - 125, SCREEN_HEIGHT - 25, 20, WHITE);
        std::string scoreText = TextFormat("%i : %i", scorePlayer1, scorePlayer2);
        constexpr int scoreFontSize = 60;
        const int textWidth = MeasureText(scoreText.c_str(), scoreFontSize);
        DrawText(scoreText.c_str(), SCREEN_WIDTH / 2 - textWidth / 2, 40, scoreFontSize, WHITE);

        std::string player1Text = "Player 1";
        std::string player2Text = "Player 2";
        constexpr int fontSize = 40;
        const int textWidth1 = MeasureText(player1Text.c_str(), fontSize);
        const int textWidth2 = MeasureText(player2Text.c_str(), fontSize);
        DrawText(player1Text.c_str(), SCREEN_WIDTH / 4 - textWidth1 / 2, 20, fontSize, Color{255, 0, 0, 255});
        DrawText(player2Text.c_str(), SCREEN_WIDTH / 1.3 - textWidth2 / 2, 20, fontSize, Color{0, 0, 255, 255});

        EndDrawing();
    }

    CloseWindow();

    return 0;
}