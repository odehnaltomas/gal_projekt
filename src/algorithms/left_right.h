#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/EdgeArray.h>
 
using namespace ogdf;

class AlgorithmLeftRight {
public:
	Graph G;
	GraphAttributes GA;

    int height_max;
    std::vector<node> roots;
    NodeArray<int> height;
    EdgeArray<int> nestingDepth;
    EdgeArray<int> lowopt;
    EdgeArray<int> lowopt2;
    NodeArray<edge> parentEdgeArr;
    EdgeArray<bool> oriented;
    NodeArray<std::vector<edge>> nodeOutEdges;
    int allOriented;
	
    AlgorithmLeftRight(Graph& g, GraphAttributes& ga);
	
	bool isPlanar();

private:
    void doDFS1(node v);
    bool doDFS2(node v);
};
