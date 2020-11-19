#pragma once
#include <string>
#include <map>

enum Algorithm {
	LeftRight,
	BoyerMyrvold,
};

const std::map<const std::string, Algorithm> algorithm_lookup_table = {
	{"left-right", Algorithm::LeftRight},
	{"boyer-myrvold", Algorithm::BoyerMyrvold},
};
