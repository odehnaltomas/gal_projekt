#pragma once
#include <string>
#include <map>

enum Algorithm {
	LeftRight,
	HopcroftTarjan,
};

const std::map<const std::string, Algorithm> algorithm_lookup_table = {
	{"left-right", Algorithm::LeftRight},
	{"hopcroft-tarjan", Algorithm::HopcroftTarjan},
};
