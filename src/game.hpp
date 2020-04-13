#ifndef _GAME_H
#define _GAME_H

#include "raylib.h"

#include "src/cluster.hpp"
#include "src/filesystem.hpp"
#include "src/generator.hpp"

#include <cstdlib>
#include <ctime>

class Game
{
    bool gameOver = false;
    const int screenWidth = 640;
    const int screenHeight = 360;
    const int targetFPS = 60;
    int blockSize = 16;
    int matrixWidth = 10;
    int matrixHeight = 20;
    Filesystem matrix{matrixWidth, matrixHeight, blockSize};
    Game() {}

public:
    static Game &GetInstance()
    {
        static Game instance;

        return instance;
    }

    Game(Game const &) = delete;
    void operator=(Game const &) = delete;

    void Init(const char *title);
    void Update();
    void Draw();
    void UpdateAndDraw()
    {
        Update();
        Draw();
    }
    void Destroy();
};

// static Camera camera = {0};
// static Vector3 cubePosition = {0.0f};

// TODO: Move definitons to game.c

#endif