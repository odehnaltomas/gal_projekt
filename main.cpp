#include <iostream>
#include <ogdf/fileformats/GraphIO.h>

#include "cli_opts.h"
#include "algorithms/left_right.h"
#include "algorithms/boyer_myrvold.h"
 
using namespace ogdf;

int main(int argc, char **argv)
{
	// parse and validate arguments, print help, ...
	Args args = parse_args(argc, argv);
	
	Graph g;
    GraphAttributes ga(g);
    if (!GraphIO::read(g, args.source_file)) {
        std::cerr << "Could not load " << args.source_file << std::endl;
        return 1;
    }
	
	bool result;
	if (args.algorithm == Algorithm::LeftRight) {
		AlgorithmLeftRight a(g, ga);
		result = a.isPlanar();
		
	} else if (args.algorithm == Algorithm::BoyerMyrvold) {
		AlgorithmBoyerMyrvold a(g, ga);
		result = a.isPlanar();
		
	}
	
	std::cout << "Result: " << result << std::endl;
	
    return 0;
}