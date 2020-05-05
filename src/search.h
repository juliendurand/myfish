#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include "position.h"

namespace chess{
	double eval(Position* position);
	double alphabetaMax(double alpha, double beta, Position* position, int depth);
	double alphabetaMin(double alpha, double beta, Position* position, int depth);
	std::string search(Position* position, int depth);
}

#endif // #ifndef SEARCH_H_INCLUDED
