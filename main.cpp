/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2020 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "src/game.hpp"

const char programName[] = "Clusters";

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    Game::GetInstance().Init(programName);

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        Game::GetInstance().UpdateAndDraw();
    }

    Game::GetInstance().Destroy();

    return 0;
}