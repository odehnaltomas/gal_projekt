#pragma once
#include <string>
#include <map>

enum Algorithm {
	LeftRight,
	HopcroftTarjan,
	Reference   // Added after presentation
};

const std::map<const std::string, Algorithm> algorithm_lookup_table = {
	{"left-right", Algorithm::LeftRight},
	{"hopcroft-tarjan", Algorithm::HopcroftTarjan},
    {"ref", Algorithm::Reference},  // Added after presentation
};
