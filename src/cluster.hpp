#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "raylib.h"

#include <cstdlib>

// Cluster size in squares
#define CLSIZE (5)

enum BlockState
{
    NONE = 0,
    BLOCK = 1,
    CORNER_CHECK_MINI = 2,
    CORNER_CHECK = 4,
    DESTROY = 8
};

enum RotationState
{
    ZERO,
    RIGHT,
    DOUBLE,
    LEFT,
};

enum ClusterUnit
{
    T,
    L,
    J,
    S,
    Z,
    O,
    I
};

enum ColorCodes
{
    // Clear color
    COL_BLANK,
    COL_RED,
    COL_ORANGE,
    COL_YELLOW,
    COL_GREEN,
    // Sky blue
    COL_CYAN,
    COL_BLUE,
    COL_PURPLE,
    COL_WHITE,
    COL_GRAY,
    // Light gray
    COL_LIGHTGRAY,
    // Dark gray
    COL_DARKGRAY,
    COL_BLACK,
};

// Helper function to retrieve raylib colors from
// aforementioned ColorCodes
Color ColorFromCode(ColorCodes code);

#pragma region CLUSTER_DATA
const int CLUSTER_T[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 6, 1, 6, 0},
                                       {0, 1, 1, 1, 0},
                                       {0, 4, 0, 4, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_L[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 6, 6, 1, 0},
                                       {0, 1, 1, 1, 0},
                                       {0, 4, 4, 0, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_J[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 1, 6, 6, 0},
                                       {0, 1, 1, 1, 0},
                                       {0, 0, 4, 4, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_S[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 6, 1, 1, 6},
                                       {4, 1, 1, 4, 0},
                                       {0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_Z[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {6, 1, 1, 6, 0},
                                       {0, 4, 1, 1, 4},
                                       {0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_O[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 0, 1, 1, 0},
                                       {0, 0, 1, 1, 0},
                                       {0, 0, 0, 0, 0},
                                       {0, 0, 0, 0, 0}};

const int CLUSTER_I[CLSIZE][CLSIZE] = {{0, 0, 0, 0, 0},
                                       {0, 0, 4, 6, 0},
                                       {0, 1, 1, 1, 1},
                                       {0, 0, 4, 6, 0},
                                       {0, 0, 0, 0, 0}};
#pragma endregion CLUSTER_DATA

struct Cluster
{
    ClusterUnit blockType;
    BlockState blockData[CLSIZE][CLSIZE];
    ColorCodes color;
    RotationState orientation;

    Cluster() : Cluster(T) {}

    Cluster(ClusterUnit type)
    {
        SetUnit(type);
    }

    Cluster(Cluster &other)
    {
        SetBlockData(other.blockData);
        blockType = other.blockType;
        color = other.color;
        orientation = other.orientation;
    }

    void SetUnit(ClusterUnit type)
    {
        blockType = type;
        orientation = RotationState::ZERO;
        switch (type)
        {
        case T:
            SetBlockData(CLUSTER_T);
            color = COL_PURPLE;
            break;
        case L:
            SetBlockData(CLUSTER_L);
            color = COL_ORANGE;
            break;
        case J:
            SetBlockData(CLUSTER_J);
            color = COL_BLUE;
            break;
        case S:
            SetBlockData(CLUSTER_S);
            color = COL_GREEN;
            break;
        case Z:
            SetBlockData(CLUSTER_Z);
            color = COL_RED;
            break;
        case O:
            SetBlockData(CLUSTER_O);
            color = COL_YELLOW;
            break;
        case I:
            SetBlockData(CLUSTER_I);
            color = COL_CYAN;
        default:
            break;
        }
    }

    void SetBlockData(const int data[CLSIZE][CLSIZE])
    {
        for (int i = 0; i < CLSIZE; i++)
            for (int j = 0; j < CLSIZE; j++)
                blockData[i][j] = (BlockState)(data[i][j]);
    }

    void SetBlockData(const BlockState data[CLSIZE][CLSIZE])
    {
        for (int i = 0; i < CLSIZE; i++)
            for (int j = 0; j < CLSIZE; j++)
                blockData[i][j] = data[i][j];
    }
};

#endif
