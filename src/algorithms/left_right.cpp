#include "left_right.h"
 
using namespace ogdf;

AlgorithmLeftRight::AlgorithmLeftRight(Graph& g, GraphAttributes& ga): G(g), GA(ga) {}

bool AlgorithmLeftRight::isPlanar() {
	return false;
}