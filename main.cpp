#include <iostream>
#include <chrono>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/basic/extended_graph_alg.h>
#include <clocale>

#include "cli_opts.h"
#include "algorithms/left_right.h"
#include "algorithms/hopcroft_tarjan.h"
#include "graph_generator.h"

using namespace ogdf;

///
/// source: https://stackoverflow.com/a/55553669/3078351
///
double microtime() {
    return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
}

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
	double time_start = microtime(), time_end;

	if (args.algorithm == Algorithm::LeftRight) {
		AlgorithmLeftRight a(g, ga);
		result = a.isPlanar();
		
	} else if (args.algorithm == Algorithm::HopcroftTarjan) {
		AlgorithmHopcroftTarjan a(g, ga);
		result = a.isPlanar();
	} else if (args.algorithm == Algorithm::Reference) { // Added after presentation
	    result = isPlanar(g);
	}

	time_end = microtime();
	if (args.test_mode) {
        setlocale(LC_ALL, "cs_CZ");
		printf("%f\n", time_end - time_start);
    } else
		std::cout << "Is planar? " << (result ? "Yes" : "No") << std::endl;
	
    return 0;
}