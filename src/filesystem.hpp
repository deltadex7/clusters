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

    Block GetElement(int index)
    {
        return _elements[index];
    }
};

class Filesystem
{
private:
    // Cluster generator using 7-piece bag system
    Generator _clusterGen;
    // Incoming cluster units
    std::queue<ClusterUnit> _clusterQueue;
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
    ClusterUnit getNext()
    {
        ClusterUnit value = _clusterQueue.front();
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

    bool isInbound(int px, int py)
    {
        return (0 <= px && px < _width && py >= 0);
    }
    bool isOccupied(int px, int py);
    bool isClusterValid(const BlockState blockData[CLSIZE][CLSIZE], int offsetX, int offsetY);
    bool isClusterValid(const BlockState blockData[CLSIZE][CLSIZE])
    {
        return isClusterValid(blockData, 0, 0);
    }

    int _blockSize;
    int _shiftDelay = 12;
    int _repeatDelay = 2;
    int _lastPressed = 0;
    int counter = _shiftDelay;
    bool supercharge = false;

public:
    // Currently active cluster
    Cluster activeCluster;
    // Initialize filesystem matrix with size parameters
    Filesystem(int width, int height, int blockSize);
    ~Filesystem();

    void GetNextCluster()
    {
        activeCluster.SetUnit(getNext());
        activeX = _width / 2 + (_width & 1 ? -1 : 0);
        activeY = _height - 2;
    }

    // Rotates the piece into targetRot, return condition
    // when rotation is successful.
    bool RotateCluster(RotationState targetRot);

    // Moves the piece sideways, return condition
    // when motion is successful.
    bool MoveCluster(int deltaX, int deltaY);

    void Update();
    void Draw();
};

#endif