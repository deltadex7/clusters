#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "src/cluster.hpp"

#include <stdlib.h>

// Cluster unit generator using 7-bag system.
// Pseudo-random generation using seed to be implemented.
class Generator
{
    int clusterBag[7] = {-1, -1, -1, -1, -1, -1, -1};
    int piecesGenerated = 0;
    long seed = 0;
    void GenerateClusterBag();

public:
    // Get next cluster unit from the bag
    ClusterUnit GetNextClUnit();
    // Initialize generator with random seed
    Generator()
    {
        seed = rand();
    }
    //Initialize generator with specified seed
    Generator(int seed)
    {
        this->seed = seed;
    }
};

#endif