/**
 * whery: TupleComparator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/TupleComparator.h"

#include "whery/db/base/Tuple.h"

namespace whery {

//#################### CONSTRUCTORS ####################

TupleComparator::TupleComparator(const std::vector<std::pair<unsigned int,SortDirection> >& fieldIndices)
:	m_fieldIndices(fieldIndices)
{
	if(fieldIndices.empty())
	{
		throw std::invalid_argument("Tuple comparisons must be performed on a non-empty list of fields.");
	}
}

//#################### PUBLIC STATIC METHODS ####################

TupleComparator TupleComparator::make_default(unsigned int n)
{
	std::vector<std::pair<unsigned int,SortDirection> > fieldIndices;
	fieldIndices.reserve(n);
	for(size_t i = 0; i < n; ++i) fieldIndices.push_back(std::make_pair(i, ASC));
	return TupleComparator(fieldIndices);
}

//#################### PUBLIC OPERATORS ####################

bool TupleComparator::operator()(const Tuple& lhs, const Tuple& rhs) const
{
	return compare(lhs, rhs) == -1;
}

//#################### PUBLIC METHODS ####################

int TupleComparator::compare(const Tuple& lhs, const Tuple& rhs) const
{
	// Check that the arities of the two tuples match.
	if(lhs.arity() != rhs.arity())
	{
		throw std::invalid_argument("Tuple comparisons must be performed on tuples of the same arity.");
	}

	for(size_t i = 0, size = m_fieldIndices.size(); i < size; ++i)
	{
		int fieldIndex = m_fieldIndices[i].first;
		SortDirection sortDirection = m_fieldIndices[i].second;

		switch(lhs.field(fieldIndex).compare_to(rhs.field(fieldIndex)))
		{
		case -1:
			return sortDirection == ASC ? -1 : 1;
		case 0:
			continue;
		case 1:
			return sortDirection == DESC ? -1 : 1;
		}
	}

	// If the tuples are equivalent after all of the relevant fields have been compared, then they compare equal.
	return 0;
}

}
