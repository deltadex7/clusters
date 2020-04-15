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
    SpawnNextCluster();
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
    // current piece at next x position, start from left
    int x = activeX - (CLSIZE / 2) + offsetX;
    // current piece at next y position, start from top
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

bool Filesystem::RotateCluster(RotationState targetRotation)
{
    // Do not do any action during ARE.
    if (_entryCounter >= 0)
        return false;

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
        _lockCounter = _lockDelay;
        _hasLanded = !isClusterValid(activeCluster.blockData, 0, -1);
        return true;
    }
    else
        return false;
}

bool Filesystem::MoveCluster(int deltaX, int deltaY)
{
    // Do not do any action during ARE.
    if (_entryCounter >= 0)
        return false;

    if (isClusterValid(activeCluster.blockData, deltaX, deltaY))
    {
        activeX += deltaX;
        activeY += deltaY;
        _lockCounter = _lockDelay;
        _hasLanded = !isClusterValid(activeCluster.blockData, 0, -1);
        return true;
    }
    else
        return false;
}

bool Filesystem::GravityMoveCluster(bool manual)
{
    bool dropped = MoveCluster(0, -1);
    // Reset gravity counter if done manually
    if (manual)
        _gravCounter = _gravDenom;

    if (dropped)
        return true;
    else
    {
        _hasLanded = true;
        return false;
    }
}

void Filesystem::SpawnNextCluster()
{
    _gravCounter = _gravDenom;
    _lockCounter = _lockDelay;
    _hasLanded = false;
    activeCluster = getNext();
    activeX = _width / 2 + (_width & 1 ? 0 : -1);
    activeY = _height - 2;
}

void Filesystem::LockCluster()
{
    // Do not do any action during ARE.
    if (_entryCounter >= 0)
        return;

    // Now copy the activeCluster.blockData into the _field
    // current piece at x position, start from left
    int x = activeX - (CLSIZE / 2);
    // current piece at y position, start from bottom
    int y = activeY - (CLSIZE / 2);

    // Field row iterator, points to current row
    auto rowIterator = _field.begin();

    // Initial position is zero, so increment iterator
    // to reach desired row. If y = 5, then increment 5x.
    for (int i = 0; i < y; i++)
    {
        rowIterator++;
    }

    // From here, assign the blockData into its respective x coords.
    for (int j = 0; j < CLSIZE; j++)
    {
        // Only check if y is inbound, so x is hard-coded to be inbound
        if (!isInbound(0, y + j))
            continue;

        for (int i = 0; i < CLSIZE; i++)
        {
            if (!isInbound(x + i, y + j))
                continue;

            BlockState dataToPush = activeCluster.blockData[CLSIZE - j - 1][i];
            if (dataToPush == BLOCK)
            {
                rowIterator->SetElement(x + i, dataToPush, activeCluster.color);
            }
        }
        rowIterator++;
    }

    // Piece has locked, set a delay before the next piece spawns.
    _entryCounter = _entryDelay;
}

void Filesystem::Update()
{

    // Handle rotations, only when pressed (not held)
    // if (IsKeyPressed('A'))
    //     SpawnNextCluster();
    // Handle entry delay
    if (_entryCounter > 0)
    {
        // Cluster hasn't spawned, decrement counter
        _entryCounter--;
    }
    else
    {
        if (_entryCounter == 0)
        {
            SpawnNextCluster();
            _entryCounter--;

            // Handle initial hold.
            // Also known as Initial Hold System (IHS),
            // Pioneered by Arika in TGM3.

            // Handle initial rotations.
            // Also known as Initial Rotation System (IRS),
            // Pioneered by Arika and the #TGM_series.
            if (IsKeyDown('F'))
                RotateCluster(RIGHT);
            if (IsKeyDown('D'))
                RotateCluster(LEFT);
            if (IsKeyDown('S'))
                RotateCluster(DOUBLE);

            // If rotation fails, no initial rotation done.
            // It is already by design.
            // Note that kicks may be performed during IRS.
        }
    }

    // Handle Instantaneous actions
    // 1. Handle hold
    // 2. Handle rotations
    if (IsKeyPressed('F'))
        RotateCluster(RIGHT);
    if (IsKeyPressed('D'))
        RotateCluster(LEFT);
    if (IsKeyPressed('S'))
        RotateCluster(DOUBLE);

    // 3. Handle motion
    if (IsKeyPressed('J'))
    {
        MoveCluster(-1, 0);
        _lastPressed = 'J';
        _shiftCounter = _shiftDelay;
    }
    if (IsKeyPressed('L'))
    {
        MoveCluster(1, 0);
        _lastPressed = 'L';
        _shiftCounter = _shiftDelay;
    }

    // Handle drop
    if (IsKeyPressed('K'))
    {
        if (GravityMoveCluster(true))
        {
            // increment score by 1
        }
    }

    // Handle dropping
    if (IsKeyDown('K'))
    {
        _gravCounter -= _gravNum * _gravSoftMult;
    }
    else
    {
        _gravCounter -= _gravNum;
    }

    // Check if the piece has landed
    if (!_hasLanded)
    {

        while (_gravCounter <= 0)
        {
            if (GravityMoveCluster(false) && IsKeyDown('K'))
            {
                // increment score
            }
            _gravCounter += _gravDenom;
        }
    }
    else
    {

        // Handle locking
        if (_lockCounter <= 0)
            LockCluster();
        else
            _lockCounter--;
    }

    if (IsKeyPressed(' '))
    {
        // The distance from current position to the lowest level possible.
        int lockDistance = 0;
        // First, check if the cluster has not already landed.
        if (!_hasLanded)
        {
            // Move the cluster down until it can't.
            while (GravityMoveCluster(false))
            {
                // And if it can drop, increase this counter.
                ++lockDistance;
            }
        }
        _lockCounter = 0;
    }

    // Handle held keys
    if (IsKeyDown('J') || IsKeyDown('L'))
    {
        if (_shiftCounter <= 0)
        {
            _shiftCounter += _repeatDelay;
            if (_lastPressed == 'J')
                MoveCluster(-1, 0);
            else if (_lastPressed == 'L')
                MoveCluster(1, 0);
        }

        if (IsKeyReleased('J'))
        {
            MoveCluster(1, 0);
            _lastPressed = 'L';
            _shiftCounter = _shiftDelay;
        }
        else if (IsKeyReleased('L'))
        {
            MoveCluster(-1, 0);

            _lastPressed = 'J';
            _shiftCounter = _shiftDelay;
        }

        _shiftCounter--;
    }
    else
    {
        _shiftCounter = _shiftDelay;
    }
}

// Draw the matrix filesystem.
// Must be in between BeginDrawing() and EndDrawing().
void Filesystem::Draw()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    char piceText;
    switch (_clusterQueue.front().blockType)
    {
    case T:
        piceText = 'T';
        break;
    case L:
        piceText = 'L';
        break;
    case J:
        piceText = 'J';
        break;
    case S:
        piceText = 'S';
        break;
    case Z:
        piceText = 'Z';
        break;
    case O:
        piceText = 'O';
        break;
    case I:
        piceText = 'I';
        break;
    default:
        break;
    }

    // switch (activeCluster.orientation)
    // {
    // case ZERO:
    //     strcat(piceText, "0");
    //     break;
    // case RIGHT:
    //     strcat(piceText, "R");
    //     break;
    // case LEFT:
    //     strcat(piceText, "L");
    //     break;
    // case DOUBLE:
    //     strcat(piceText, "2");

    // default:
    //     break;
    // }

    char debugData[20];

    sprintf(debugData, "%c %d %d %d %d", piceText, _gravCounter, _lockCounter, _shiftCounter, _entryCounter);

    DrawText(debugData, 10, 40, 20, LIGHTGRAY);

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

    // // Draw next piece
    // // Draw from left
    // int nextLeft = (activeX - CLSIZE / 2) * _blockSize;
    // drawOrigin.x = (screenWidth / 2) - (_width * _blockSize / 2) + nextLeft;
    // // Draw from top
    // int activeTop = (activeY + CLSIZE / 2 + 1) * _blockSize;
    // drawOrigin.y = (screenHeight / 2) - (_height * _blockSize / 2) - activeTop;

    // originX = drawOrigin.x;

    // for (int i = 0; i < CLSIZE; i++)
    // {
    //     for (int j = 0; j < CLSIZE; j++)
    //     {
    //         if (_clusterQueue.front() & BLOCK)
    //         {
    //             DrawRectangle(drawOrigin.x, drawOrigin.y, _blockSize, _blockSize, ColorFromCode(activeCluster.color));
    //         }
    //         drawOrigin.x += _blockSize;
    //     }

    //     drawOrigin.x = originX;
    //     drawOrigin.y += _blockSize;
    // }
}
