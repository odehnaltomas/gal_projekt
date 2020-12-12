#include <iostream>
#include <ogdf/fileformats/GraphIO.h>

#include "cli_opts.h"
#include "algorithms/left_right.h"
#include "algorithms/hopcroft_tarjan.h"
#include "graph_generator.h"

using namespace ogdf;

int main(int argc, char **argv)
{
	// parse and validate arguments, print help, ...
	Args args = parse_args(argc, argv);

	if (args.generate_graph) {
        auto g = new GraphGenerator();

	    if (args.want_planar) {
            g->generatePlanarGraph(args.num_nodes, args.num_edges, args.output_file);

//            std::cout << "Generated PLANAR graph '" << args.output_file << "' with " <<
//                args.num_nodes << " and " << args.num_edges << " edges." << std::endl;
	    } else {
	        g->generateNonplanarGraph(args.num_nodes, args.output_file);

//            std::cout << "Generated NON-PLANAR graph '" << args.output_file << "' with " <<
//                      args.num_nodes << " and " << args.num_edges << " edges." << std::endl;
	    }
        return 0;
    }

	Graph g;
    GraphAttributes ga(g, GraphAttributes::nodeLabel | GraphAttributes::edgeLabel);
    if (!GraphIO::read(ga, g, args.source_file, GraphIO::readGML)) {
        std::cerr << "Could not load " << args.source_file << std::endl;
        return 1;
    }
	
	bool result;
	if (args.algorithm == Algorithm::LeftRight) {
		AlgorithmLeftRight a(g, ga);
		result = a.isPlanar();
		
	} else if (args.algorithm == Algorithm::HopcroftTarjan) {
		AlgorithmHopcroftTarjan a(g, ga);
		result = a.isPlanar();
	}
	
	std::cout << "Result: " << result << std::endl;
	
    return 0;
}