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

    activeCluster.SetUnit(pieceGenerator.GetNextCluster());
}

void Game::Update()
{
    // UpdateCamera(&camera);
    if (IsKeyPressed('R'))
        activeCluster.SetUnit(pieceGenerator.GetNextCluster());

    if (IsKeyPressed('F'))
        activeCluster.Rotate(RIGHT);
    if (IsKeyPressed('D'))
        activeCluster.Rotate(LEFT);
    if (IsKeyPressed('S'))
        activeCluster.Rotate(DOUBLE);
}

void Game::Draw()
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    // BeginMode3D(camera);

    // DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
    // DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
    // DrawGrid(10, 1.0f);

    // EndMode3D();

    char piceText[3];
    switch (activeCluster.blockType)
    {
    case T:
        strcpy(piceText, "T");
        break;
    case L:
        strcpy(piceText, "L");
        break;
    case J:
        strcpy(piceText, "J");
        break;
    case S:
        strcpy(piceText, "S");
        break;
    case Z:
        strcpy(piceText, "Z");
        break;
    case O:
        strcpy(piceText, "O");
        break;
    case I:
        strcpy(piceText, "I");
        break;
    default:
        break;
    }

    switch (activeCluster.orientation)
    {
    case ZERO:
        strcat(piceText, "0");
        break;
    case RIGHT:
        strcat(piceText, "R");
        break;
    case LEFT:
        strcat(piceText, "L");
        break;
    case DOUBLE:
        strcat(piceText, "2");

    default:
        break;
    }

    DrawText(piceText, 10, 40, 20, DARKGRAY);

    DrawFPS(10, 10);

    // Draw current piece
    Vector2 drawOrigin;
    drawOrigin.x = (screenWidth / 2) - (blockSize * 5 / 2);
    drawOrigin.y = (screenHeight / 2) - (blockSize * 5 / 2);

    int originX = drawOrigin.x;

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (activeCluster.blockData[i][j] & BLOCK)
            {
                DrawRectangle(drawOrigin.x, drawOrigin.y, blockSize, blockSize, activeCluster.color);
            }
            drawOrigin.x += blockSize;
        }

        drawOrigin.x = originX;
        drawOrigin.y += blockSize;
    }

    EndDrawing();
}

void Game::Destroy()
{
    CloseWindow(); // Close window and OpenGL context
}
