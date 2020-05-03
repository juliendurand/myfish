#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED

#include "position.h"

namespace chess{
	double eval(Position* position);
	std::string search(Position* position, int depth);
}

#endif // #ifndef SEARCH_H_INCLUDED
