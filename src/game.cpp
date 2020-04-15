#include "src/game.hpp"

void Game::Init(const char *title)
{
    InitWindow(screenWidth, screenHeight, title);
    srand(time(NULL));

    // camera.position = (Vector3){10.0f, 10.0f, 8.0f};
    // camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    // camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    // camera.fovy = 60.0f;
    // camera.type = CAMERA_PERSPECTIVE;

    // SetCameraMode(camera, CAMERA_ORBITAL);

    SetTargetFPS(targetFPS); // Set our game to run at 60 frames-per-second
}

void Game::Update()
{
    // UpdateCamera(&camera);
    matrix.Update();
}

void Game::Draw()
{
    BeginDrawing();

    ClearBackground(BLACK);

    DrawFPS(8, 8);

    // BeginMode3D(camera);

    // DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
    // DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
    // DrawGrid(10, 1.0f);

    // EndMode3D();

    matrix.Draw();

    EndDrawing();
}

void Game::Destroy()
{
    CloseWindow(); // Close window and OpenGL context
}
