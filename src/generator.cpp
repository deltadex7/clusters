#include "src/generator.hpp"

void Generator::GenerateClusterBag()
{
    for (int i = 0; i < 7; i++)
        clusterBag[i] = i;
}

ClusterUnit Generator::GetNextClUnit()
{
    if (piecesGenerated % 7 == 0)
    {
        GenerateClusterBag();
    }

    int i = rand() % 7;
    while (clusterBag[i] == (-1))
    {
        i = rand() % 7;
    }

    ClusterUnit unit;
    unit = (ClusterUnit)clusterBag[i];

    // activeCluster.SetUnit(&activeCluster, unit);

    clusterBag[i] = -1;

    // prevent overflow
    if (piecesGenerated > __SHRT_MAX__ - 5)
        piecesGenerated %= 7;

    piecesGenerated++;

    return unit;
}
