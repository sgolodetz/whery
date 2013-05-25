/**
 * whery: FieldTupleComparator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/FieldTupleComparator.h"

#include "whery/db/FieldTuple.h"

namespace whery {

//#################### CONSTRUCTORS ####################

FieldTupleComparator::FieldTupleComparator(const std::vector<std::pair<unsigned int,SortDirection> >& fieldIndices)
:	m_fieldIndices(fieldIndices)
{
	if(fieldIndices.empty())
	{
		throw std::invalid_argument("Field tuple comparisons must be performed on a non-empty list of fields.");
	}
}

//#################### PUBLIC OPERATORS ####################

bool FieldTupleComparator::operator()(const FieldTuple& lhs, const FieldTuple& rhs) const
{
	return compare(lhs, rhs) == -1;
}

//#################### PUBLIC METHODS ####################

int FieldTupleComparator::compare(const FieldTuple& lhs, const FieldTuple& rhs) const
{
	// Check that the arities of the two tuples match.
	if(lhs.arity() != rhs.arity())
	{
		throw std::invalid_argument("Field tuple comparisons must be performed on tuples of the same arity.");
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

	// If the field tuples are equivalent after all of the relevant fields have been compared,
	// then they compare equal.
	return 0;
}

}
