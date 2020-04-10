#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "src/cluster.hpp"

#include <stdlib.h>

class Generator
{
    int clusterBag[7] = {-1, -1, -1, -1, -1, -1, -1};
    int piecesGenerated = 0;
    long seed = 0;
    void GenerateClusterBag();

public:
    ClusterUnit GetNextCluster();
    Generator()
    {
        seed = rand();
    }
    Generator(int seed)
    {
        this->seed = seed;
    }
};

#endif