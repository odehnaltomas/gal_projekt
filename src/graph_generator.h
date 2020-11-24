#pragma once
#include <ogdf/basic/graph_generators.h>
#include <ogdf/layered/DfsAcyclicSubgraph.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>
#include <ogdf/planarity/BoothLueker.h>

#include <iostream>

using namespace ogdf;

class GraphGenerator {
public:
    GraphGenerator();
    void generatePlanarGraph(unsigned int num_nodes, unsigned int num_edges, std::string fileName);
    void generateNonplanarGraph(unsigned int num_nodes, std::string fileName);

private:
    void createLayoutAndSave(Graph g, std::string fileName);
};
