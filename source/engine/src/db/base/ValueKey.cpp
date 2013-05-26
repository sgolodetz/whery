/**
 * whery: ValueKey.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/base/ValueKey.h"

namespace whery {

//#################### CONSTRUCTORS ####################

ValueKey::ValueKey(
	const std::vector<const FieldManipulator*>& fieldManipulators,
	const std::vector<unsigned int>& fieldIndices)
:	FreshTuple(TupleManipulator(fieldManipulators, fieldIndices)),
	m_fieldIndices(fieldIndices)
{}

//#################### PUBLIC METHODS ####################

const std::vector<unsigned int>& ValueKey::field_indices() const
{
	return m_fieldIndices;
}

}
