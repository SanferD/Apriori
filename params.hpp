#ifndef _PARAMS_HPP
#define _PARAMS_HPP

#include <cstdlib>

#include "types.hpp"

struct Params {
	int minsup;
	float minconf;
	std::string inputfile, outputfile;
	int hfrange, maxleafsize;
	
	Params(int argc, char *argv[]) {
		minsup = std::atoi(argv[1]);
		minconf = std::atof(argv[2]);
		inputfile = argv[3];
		outputfile = argv[4];
		hfrange = std::atoi(argv[5]);
		maxleafsize = std::atoi(argv[6]);
	}

	void print() {
		printf("minsup: %i minconf: %.3f inputfile: %s outputfile: %s hfrange: %i maxleafsize: %i\n", minsup, minconf, inputfile.c_str(), outputfile.c_str(), hfrange, maxleafsize);
	}
};

#endif