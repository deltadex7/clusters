#include "src/filesystem.hpp"
#include <cstdio>
#include <cstring>

Filesystem::Filesystem(int width, int height, int blockSize)
{
    _width = width;
    _height = height;
    _maxHeight = _height * 2;
    _blockSize = blockSize;
    initField();
    fillQueue();
    GetNextCluster();
}

Filesystem::~Filesystem()
{
    destroyField();
}

void Filesystem::initField()
{
    // _field = new Block *[_height];

    // for (int i = 0; i < _height; i++)
    //     _field[i] = new Block[_width];
    for (int i = 0; i < _maxHeight; i++)
    {
        _field.push_back(BlockRow(_width));
    }

    clearField();
}

void Filesystem::clearField()
{
    // for (int i = 0; i < _height; ++i)
    //     for (int j = 0; j < _width; ++j)
    //     {
    //         _field[i][j].state = BlockState::NONE;
    //         _field[i][j].color = COL_BLANK;
    //     }

    // Field list iterator
    for (auto &&row : _field)
    {
        row.ClearRow();
    }
}

void Filesystem::destroyField()
{
    // for (int i = 0; i < _height; i++)
    //     delete[] _field[i];

    // delete[] _field;
}

bool Filesystem::RotateCluster(RotationState targetRotation)
{
    //todo, watch OLC
    RotationState targetState =
        (RotationState)((activeCluster.orientation + targetRotation) % 4);

    BlockState newState[5][5];
    int x, y;
    for (int i = 0; i < CLSIZE; i++)
        for (int j = 0; j < CLSIZE; j++)
        {
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
            newState[i][j] = activeCluster.blockData[y][x];
        }

    if (isClusterValid(newState))
    {
        activeCluster.orientation = targetState;
        activeCluster.SetBlockData(newState);
        return true;
    }
    else
        return false;
}

bool Filesystem::MoveCluster(int deltaX, int deltaY)
{
    if (isClusterValid(activeCluster.blockData, deltaX, deltaY))
    {
        activeX += deltaX;
        activeY += deltaY;
        return true;
    }
    else
        return false;
}

bool Filesystem::isOccupied(int px, int py)
{
    auto iterator = _field.begin();
    for (int i = 0; i < py; ++i)
        iterator++;

    return ((*iterator).GetElement(px).state == BlockState::BLOCK);
}

bool Filesystem::isClusterValid(const BlockState blockData[CLSIZE][CLSIZE],
                                int offsetX, int offsetY)

{
    // current piece at next x position
    int x = activeX - (CLSIZE / 2) + offsetX;
    // current piece at next y position
    int y = activeY + (CLSIZE / 2) + offsetY;
    for (int i = 0; i < CLSIZE; ++i)
    {
        for (int j = 0; j < CLSIZE; ++j)
        {
            // // First, check if the cluster is out of bounds
            if (
                blockData[i][j] == BlockState::BLOCK &&
                (!isInbound(x + j, y - i) ||
                 isOccupied(x + j, y - i)))
                return false;
            // 1. if the current piece is a block &&
            // 2a. if the current piece at next position is out of bounds ||
            // 2b. if the current piece at next position is already occupied
        }
    }

    return true;
}

void Filesystem::Update()
{

    // Handle rotations, only when pressed (not held)
    if (IsKeyPressed('R'))
        GetNextCluster();
    if (IsKeyPressed('F'))
        RotateCluster(RIGHT);
    if (IsKeyPressed('D'))
        RotateCluster(LEFT);
    if (IsKeyPressed('S'))
        RotateCluster(DOUBLE);

    // Handle motion
    if (IsKeyPressed('J'))
    {
        MoveCluster(-1, 0);
        _lastPressed = 'J';
        counter = _shiftDelay;
    }
    if (IsKeyPressed('L'))
    {
        MoveCluster(1, 0);
        _lastPressed = 'L';
        counter = _shiftDelay;
    }

    // Handle held keys
    if (IsKeyDown('J') || IsKeyDown('L'))
    {
        if (counter <= 0)
        {
            counter += _repeatDelay;
            if (_lastPressed == 'J')
                MoveCluster(-1, 0);
            else if (_lastPressed == 'L')
                MoveCluster(1, 0);
        }

        if (IsKeyReleased('J'))
        {
            MoveCluster(1, 0);
            _lastPressed = 'L';
            counter = _shiftDelay;
        }
        else if (IsKeyReleased('L'))
        {
            MoveCluster(-1, 0);

            _lastPressed = 'J';
            counter = _shiftDelay;
        }

        counter--;
    }
    else
    {
        counter = _shiftDelay;
    }

    // Handle drop
    if (IsKeyPressed('K'))
        MoveCluster(0, -1);
}

// Draw the matrix filesystem.
// Must be in between BeginDrawing() and EndDrawing().
void Filesystem::Draw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

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

    DrawText(piceText, 10, 40, 20, LIGHTGRAY);

    DrawFPS(10, 10);

    // Helper variable to draw
    Vector2 drawOrigin;
    // Stores the origin value of x coord
    int originX;

    // Draw the matrix

    // Draw from left
    drawOrigin.x = (screenWidth / 2) - (_width * _blockSize / 2);
    // Draw from bottom
    drawOrigin.y = (screenHeight / 2) + (_height * _blockSize / 2);

    originX = drawOrigin.x;

    // Row iterator
    auto fieldRow = _field.begin();
    for (int i = 0; i < _height; i++)
    {
        for (int j = 0; j < _width; j++)
        {
            Block cell = (*fieldRow).GetElement(j);
            switch (cell.state)
            {

            case BlockState::BLOCK:
                // Draw rectangle from top left
                DrawRectangle(drawOrigin.x, drawOrigin.y - _blockSize, _blockSize, _blockSize, ColorFromCode(cell.color));
                break;
            case BlockState::DESTROY:
                // Draw rectangle from top left
                DrawRectangle(drawOrigin.x, drawOrigin.y - _blockSize, _blockSize, _blockSize, LIGHTGRAY);
                break;
            case NONE:
            default:
                // Draw horizontally from bottom left to bottom right
                DrawLine(drawOrigin.x, drawOrigin.y, drawOrigin.x + _blockSize, drawOrigin.y, LIGHTGRAY);
                // Draw vertically from bottom left to top left
                DrawLine(drawOrigin.x, drawOrigin.y, drawOrigin.x, drawOrigin.y - _blockSize, LIGHTGRAY);
                // Draw horizontally from bottom right to top right
                DrawLine(drawOrigin.x + _blockSize, drawOrigin.y, drawOrigin.x + _blockSize, drawOrigin.y - _blockSize, LIGHTGRAY);
                // Draw horizontally from top left to top right
                DrawLine(drawOrigin.x, drawOrigin.y - _blockSize, drawOrigin.x + _blockSize, drawOrigin.y - _blockSize, LIGHTGRAY);
                break;
            }
            drawOrigin.x += _blockSize;
        }
        drawOrigin.x = originX;
        drawOrigin.y -= _blockSize;
        fieldRow++;
    }

    // Draw current piece
    // Draw from left
    int activeLeft = (activeX - CLSIZE / 2) * _blockSize;
    drawOrigin.x = (screenWidth / 2) - (_width * _blockSize / 2) + activeLeft;
    // Draw from top
    int activeTop = (activeY + CLSIZE / 2 + 1) * _blockSize;
    drawOrigin.y = (screenHeight / 2) + (_height * _blockSize / 2) - activeTop;

    originX = drawOrigin.x;

    for (int i = 0; i < CLSIZE; i++)
    {
        for (int j = 0; j < CLSIZE; j++)
        {
            if (activeCluster.blockData[i][j] & BLOCK)
            {
                DrawRectangle(drawOrigin.x, drawOrigin.y, _blockSize, _blockSize, ColorFromCode(activeCluster.color));
            }
            drawOrigin.x += _blockSize;
        }

        drawOrigin.x = originX;
        drawOrigin.y += _blockSize;
    }
}
