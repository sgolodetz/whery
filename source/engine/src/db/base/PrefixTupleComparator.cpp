/**
 * whery: PrefixTupleComparator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/PrefixTupleComparator.h"

#include "whery/db/base/Tuple.h"

namespace whery {

//#################### PUBLIC OPERATORS ####################

bool PrefixTupleComparator::operator()(const Tuple& lhs, const Tuple& rhs) const
{
	return compare(lhs, rhs) == -1;
}

//#################### PUBLIC METHODS ####################

int PrefixTupleComparator::compare(const Tuple& lhs, const Tuple& rhs) const
{
	for(size_t i = 0, size = std::min(lhs.arity(), rhs.arity()); i < size; ++i)
	{
		switch(lhs.field(i).compare_to(rhs.field(i)))
		{
		case -1:
			return -1;
		case 0:
			continue;
		case 1:
			return 1;
		}
	}

	// If the tuples are equivalent up to this point, the result depends on their arities.
	if(lhs.arity() < rhs.arity()) return -1;
	else if(lhs.arity() > rhs.arity()) return 1;
	else return 0;
}

}
