#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "raylib.h"
#include "src/cluster.hpp"
// #include "src/game.hpp"
#include "src/generator.hpp"

#include <algorithm>
#include <list>
#include <queue>
#include <vector>

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
    std::vector<Block> _elements;
    int _size;

public:
    bool toClear = false;
    auto begin()
    {
        return _elements.begin();
    }

    auto end()
    {
        return _elements.end();
    }

    BlockRow(int size) : _size(size), _elements(size) {}

    ~BlockRow()
    {
        // delete[] _elements;
    }

    void ClearRow()
    {
        for (auto &&el : _elements)
        {
            el.state = BlockState::NONE;
            el.color = COL_BLANK;
        }
    }

    void DeleteRow()
    {
        for (auto &&el : _elements)
            el.state = BlockState::DESTROY;
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

    // Check if the row is full and tag the row and the blocks
    // for removal if it is.
    bool isFull()
    {
        toClear = std::all_of(_elements.begin(), _elements.end(),
                              [](const Block &el) {
                                  return el.state == BLOCK;
                              });

        if (toClear)
            std::for_each(_elements.begin(), _elements.end(),
                          [](Block &el) {
                              el.state = DESTROY;
                          });
        return toClear;
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
    size_t _height;
    // Maximum height of field to accomodate overflows
    size_t _maxHeight;
    // Initialize the field
    void initField();
    // Clear the field
    void clearField();
    // De-initialize the field.
    void destroyField();
    // Tag filled row to be deleted. Used for animation.
    // Returns true if any row is tagged.
    bool tagFieldRow();
    // Delete the tagged filled row.
    void eraseTaggedFieldRows();

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
    int _lockDelay = 30;
    // Lock delay clock/counter
    int _lockCounter = _lockDelay;

    // Time before the row is erased
    int _eraseDelay = 30;
    // Erase delay clock/counter. -1 means no rows to erase.
    int _eraseCounter = -1;

    // Wait time before the next cluster spawns.
    // Also known as ARE (a Japanese word for ???)
    int _entryDelay = 30;
    // Entry delay clock/counter. -1 means cluster has spawned.
    // Can be combined with erase delay.
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
    bool GravityMoveCluster(bool manual);

    // Lock the active cluster to the filesystem matrix.
    void LockCluster();

    void Update();
    void Draw();
};

#endif