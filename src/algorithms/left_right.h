#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
 
using namespace ogdf;

class AlgorithmLeftRight {
public:
	Graph G;
	GraphAttributes GA;
	
    AlgorithmLeftRight(Graph& g, GraphAttributes& ga);
	
	bool isPlanar();
};
