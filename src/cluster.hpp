#ifndef _CLUSTER_H
#define _CLUSTER_H

#include "raylib.h"

#include <stdlib.h>
#include <string.h>

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

struct Cluster
{
    ClusterUnit blockType;
    int blockData[CLSIZE][CLSIZE];
    Color color;
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
        orientation = ZERO;
        switch (type)
        {
        case T:
            SetBlockData(CLUSTER_T);
            color = PURPLE;
            break;
        case L:
            SetBlockData(CLUSTER_L);
            color = ORANGE;
            break;
        case J:
            SetBlockData(CLUSTER_J);
            color = BLUE;
            break;
        case S:
            SetBlockData(CLUSTER_S);
            color = GREEN;
            break;
        case Z:
            SetBlockData(CLUSTER_Z);
            color = RED;
            break;
        case O:
            SetBlockData(CLUSTER_O);
            color = YELLOW;
            break;
        case I:
            SetBlockData(CLUSTER_I);
            color = SKYBLUE;
        default:
            break;
        }
    }

    void SetBlockData(const int data[CLSIZE][CLSIZE])
    {
        for (int i = 0; i < CLSIZE; i++)
            for (int j = 0; j < CLSIZE; j++)
                blockData[i][j] = data[i][j];
    }

    void Rotate(RotationState targetRotation)
    {
        //todo, watch OLC
        orientation = (RotationState)((orientation + targetRotation) % 4);

        int newState[5][5];
        for (int i = 0; i < CLSIZE; i++)
            for (int j = 0; j < CLSIZE; j++)
            {
                int x, y;
                switch (targetRotation)
                {
                case RIGHT:
                    x = i;
                    y = CLSIZE - j - 1;
                    break;
                case DOUBLE:
                    x = CLSIZE - j - 1;
                    y = CLSIZE - i - 1;
                    break;
                case LEFT:
                    x = CLSIZE - i - 1;
                    y = j;
                    break;
                case ZERO:
                default:
                    y = i;
                    break;
                }
                newState[i][j] = blockData[y][x];
            }

        SetBlockData(newState);
    }
};

#endif
