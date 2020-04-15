#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "raylib.h"
#include "src/cluster.hpp"
// #include "src/game.hpp"
#include "src/generator.hpp"

#include <list>
#include <queue>

struct Block
{
    BlockState state;
    ColorCodes color;

    Block() : state(BlockState::NONE), color(COL_BLANK) {}
    Block(BlockState st, ColorCodes cl) : state(st), color(cl) {}

    Block(const Block &bl)
    {
        state = bl.state;
        color = bl.color;
    }
};

class BlockRow
{
    Block *_elements;
    int _size;
    bool toClear = false;

public:
    BlockRow(int size)
    {
        _size = size;
        _elements = new Block[size];
    }

    ~BlockRow()
    {
        // delete[] _elements;
    }

    void ClearRow()
    {
        for (int i = 0; i < _size; i++)
        {
            _elements[i].state = BlockState::NONE;
            _elements[i].color = COL_BLANK;
        }
    }

    void DeleteRow()
    {
        for (int i = 0; i < _size; i++)
        {
            _elements[i].state = BlockState::DESTROY;
        }
    }

    Block &GetElement(int index)
    {
        return _elements[index];
    }

    // void SetElement(int index, Block &block)
    // {
    //     _elements[index] = block;
    // }

    void SetElement(int index, BlockState st, ColorCodes cl)
    {
        _elements[index].state = st;
        _elements[index].color = cl;
    }
};

enum SystemStatus
{
    ACTIVE,
    INACTIVE,
    DEADLOCK,
    OVERFLOWN
};

class Filesystem
{
private:
    // Cluster generator using 7-piece bag system
    Generator _clusterGen;
    // Incoming cluster units
    std::queue<Cluster> _clusterQueue;
    // Cluster queue size
    short _queueSize = 5;

    // Fill the cluster queue so that it stays at max capacity
    void fillQueue()
    {
        while (_clusterQueue.size() < _queueSize)
        {
            _clusterQueue.push(_clusterGen.GetNextClUnit());
        }
    }

    // Get the next cluster unit, pop one from the queue,
    // and refill the queue.
    Cluster getNext()
    {
        Cluster value = _clusterQueue.front();
        _clusterQueue.pop();
        fillQueue();
        return value;
    }

    // Cluster position in X
    int activeX;
    // Cluster position in Y
    int activeY;

    // Filesystem field, a list of BlockRows
    // for easier deletion
    std::list<BlockRow> _field;
    // Width (column count) of field
    int _width;
    // Height (row count) of field
    int _height;
    // Maximum height of field to accomodate overflows
    int _maxHeight;
    // Initialize the field
    void initField();
    // Clear the field
    void clearField();
    // De-initialize the field
    void destroyField();

    // Check if the given coordinate is within the bounds of
    // the field.
    bool isInbound(int px, int py)
    {
        return (0 <= px && px < _width && py >= 0);
    }

    // Check if the block in given coordinates exist.
    bool isOccupied(int px, int py);

    // Check if the shape and move is possible at offset position
    bool isClusterValid(const BlockState blockData[CLSIZE][CLSIZE], int offsetX, int offsetY);

    // Check if the shape and move is possible at zero offset
    bool isClusterValid(const BlockState blockData[CLSIZE][CLSIZE])
    {
        return isClusterValid(blockData, 0, 0);
    }

    // Size of block to draw
    int _blockSize;
    // Time in frames before autoshift begins.
    // Also known as Delayed Auto Shift (DAS)
    int _shiftDelay = 12;
    // Time in frames between autoshifts.
    // Also known as Auto Repeat Rate (ARR)
    int _repeatDelay = 2;
    // Last motion key pressed
    int _lastPressed = 0;
    // Autoshift clock/counter
    int _shiftCounter = _shiftDelay;

    // Gravity numerator value
    int _gravNum = 5;
    // Gravity denominator value
    int _gravDenom = 300;
    // Soft drop speed multiplier
    int _gravSoftMult = 20;
    // Drop clock/counter
    int _gravCounter = _gravDenom;
    // // Is the piece being soft dropped?
    // bool _softDropping = false;

    // Conditional variable if the piece has landed
    bool _hasLanded = false;
    // Time in frames to lock current cluster
    int _lockDelay = 60;
    // Lock delay clock/counter
    int _lockCounter = _lockDelay;

    // Wait time before the next cluster spawns.
    // Also known as ARE (a Japanese word for ???)
    int _entryDelay = 60;
    // Entry delay clock/counter. -1 means cluster has spawned.
    int _entryCounter = -1;

public:
    //  Filesystem active status

    // Currently active cluster
    Cluster activeCluster;
    // Initialize filesystem matrix with size parameters
    Filesystem(int width, int height, int blockSize);
    ~Filesystem();

    void SpawnNextCluster();

    // Rotates the piece into targetRot. Return condition
    // when rotation is successful.
    bool RotateCluster(RotationState targetRot);

    // Moves the piece sideways. Return condition
    // when motion is successful.
    bool MoveCluster(int deltaX, int deltaY);

    // Let the gravity move the piece. Return condition
    // when motion is successful.
    bool GravityMoveCluster();

    // Lock the active cluster to the filesystem matrix.
    void LockCluster();

    void Update();
    void Draw();
};

#endif