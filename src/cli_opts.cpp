#include <iostream>
#include <getopt.h>

#include "algorithms.h"
#include "cli_opts.h"

Args parse_args(int argc, char **argv)
{
	Args res;
	
	for(;;)
	{
		switch(getopt(argc, argv, "a:f:n:h"))
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

			case '?':
			case 'h':
			default :
				std::cerr << "Usage example: " << std::endl;
				exit(1);

			case -1:
				break;
		}

		break;
	}
	
	if (!res.algorithm_provided) {
		std::cerr << "No algorithm has been selected!" << std::endl;
		exit(1);
	}
	
	if (!res.source_file_provided) {
		std::cerr << "No graph source file has been provided!" << std::endl;
		exit(1);
	}
	
	return res;
}
