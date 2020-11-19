#pragma once
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
 
using namespace ogdf;

class AlgorithmBoyerMyrvold {
public:
	Graph G;
	GraphAttributes GA;
	
    AlgorithmBoyerMyrvold(Graph& g, GraphAttributes& ga);
	
	bool isPlanar();
};
