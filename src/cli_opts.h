#pragma once
#include <string>

#include "algorithms.h"
	
struct Args {
	Algorithm algorithm;
	bool algorithm_provided;
	
	std::string source_file;
	bool source_file_provided;
};
typedef struct Args Args;

Args parse_args(int argc, char **args);
