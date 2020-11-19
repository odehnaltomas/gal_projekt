#include "boyer_myrvold.h"
 
using namespace ogdf;

AlgorithmBoyerMyrvold::AlgorithmBoyerMyrvold(Graph& g, GraphAttributes& ga): G(g), GA(ga) {}

bool AlgorithmBoyerMyrvold::isPlanar() {
	return false;
}