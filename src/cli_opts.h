#pragma once
#include <string>

#include "algorithms.h"
	
struct Args {
    Args() : generate_graph(false), want_planar(false), num_nodes(0), num_edges(0) {}    // Set default value of 'want_planar' to true

	Algorithm algorithm;
	bool algorithm_provided;
	
	std::string source_file;
	bool source_file_provided;

	bool generate_graph;
	bool want_planar;
	unsigned int num_nodes;
	unsigned int num_edges;

	std::string output_file;
	bool output_file_provided;

	bool test_mode = false;
};
typedef struct Args Args;

Args parse_args(int argc, char **args);
