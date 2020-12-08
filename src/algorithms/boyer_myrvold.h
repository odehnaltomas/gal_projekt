#pragma once
#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/GraphAttributes.h>
#include "../utils.h"
 
using namespace ogdf;

class AlgorithmBoyerMyrvold
{
public:
	Graph G;
	GraphAttributes GA;
	DFS::DFS dfs;
	
    AlgorithmBoyerMyrvold(Graph& g, GraphAttributes& ga);

	bool isPlanar();

protected:
	std::map<int, bool> edge_old;
	std::map<int, bool> node_old;
	
	void mark_old(node v);
	void mark_old(adjEntry adj);
	
	bool is_old(node v);
	bool is_old(adjEntry adj);

	std::vector<std::pair<node, adjEntry>> pathfinder(node v);

	std::map<int, int> stNumbering();
};
