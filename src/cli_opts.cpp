#include <iostream>
#include <getopt.h>

#include "algorithms.h"
#include "cli_opts.h"

Args parse_args(int argc, char **argv)
{
	Args res;
	
	for(;;)
	{
		switch(getopt(argc, argv, "a:f:tn:h:gpn:m:o:"))
		{
			case 'a':
				try {
					res.algorithm = algorithm_lookup_table.at(optarg);
					res.algorithm_provided = true;
					
				} catch (const std::out_of_range& ex) {
					std::cerr << "Specified algorithm was not found: " << optarg << std::endl;
					exit(1);
				}
				continue;

			case 'f':
				res.source_file = optarg;
				res.source_file_provided = true;
				continue;

		    case 't':
		        res.test_mode = true;
                continue;

		    case 'g':
		        res.generate_graph = true;
                continue;

            case 'p':
                res.want_planar = true;
                continue;

            case 'n':
                res.num_nodes = std::stoi(optarg, nullptr, 10);
                continue;

            case 'm':
                res.num_edges = std::stoi(optarg, nullptr, 10);
                continue;

            case 'o':
                res.output_file = optarg;
                res.output_file_provided = true;
                continue;

			case '?':
			case 'h':
			default :
				std::cerr << "Usage example: " << std::endl <<
				"\t" << "-a {left-right|hopcroft-tarjan} - choose algorithm to test if graph is planar." << std::endl <<
				"\t" << "-f PATH - path to source file of graph." << std::endl <<
				"\t" << "-t - turns on testing mode (outputs running time)" << std::endl <<
				"\t" << "-g - generate (non-)planar graph:" << std::endl <<
				"\t" << "-p - use this option to generate planar graph. NOTE: Non-planar graph will be complete graph" << std::endl <<
				"\t" << "-n X - generated graph will have X nodes" << std::endl <<
				"\t" << "-m X - generated graph will have X edges (!for planar graph!) n <= X <= (3n-6). X is adjusted if is out of bounds." << std::endl <<
				"\t" << "-o PATH - generated graph is saved in file PATH" << std::endl;
				exit(1);

			case -1:
				break;
		}

		break;
	}

	if (!res.generate_graph) {
        if (!res.algorithm_provided) {
            std::cerr << "No algorithm has been selected!" << std::endl;
            exit(1);
        }

        if (!res.source_file_provided) {
            std::cerr << "No graph source file has been provided!" << std::endl;
            exit(1);
        }
    } else {
        if (!res.output_file_provided) {
            std::cerr << "No output file for generated graph has been provided!" << std::endl;
            exit(1);
        }
    }
	
	return res;
}
