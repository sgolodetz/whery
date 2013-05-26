/**
 * whery: TupleManipulator.cpp
 * Copyright Stuart Golodetz, 2013. All rights reserved.
 */

#include "whery/db/TupleManipulator.h"

#include <cassert>

#include "whery/db/FieldManipulator.h"
#include "whery/util/AlignmentTracker.h"

namespace whery {

//#################### CONSTRUCTORS ####################

TupleManipulator::TupleManipulator(const std::vector<const FieldManipulator*>& fieldManipulators)
{
	initialise(fieldManipulators);
}

TupleManipulator::TupleManipulator(const boost::assign_detail::generic_list<const FieldManipulator*>& fieldManipulators)
{
	initialise(fieldManipulators);
}

TupleManipulator::TupleManipulator(
	const std::vector<const FieldManipulator*>& fieldManipulators,
	const std::vector<unsigned int>& fieldIndices)
{
	std::vector<const FieldManipulator*> projectedFieldManipulators;
	size_t fieldCount = fieldIndices.size();
	projectedFieldManipulators.reserve(fieldCount);
	for(size_t i = 0; i < fieldCount; ++i)
	{
		assert(fieldIndices[i] < fieldManipulators.size());
		projectedFieldManipulators.push_back(fieldManipulators[fieldIndices[i]]);
	}

	initialise(projectedFieldManipulators);
}

//#################### PUBLIC METHODS ####################

unsigned int TupleManipulator::arity() const
{
	return m_fieldManipulators.size();
}

Field TupleManipulator::field(char *tupleLocation, unsigned int i) const
{
	assert(i < m_fieldManipulators.size());
	return Field(tupleLocation + m_fieldOffsets[i], *m_fieldManipulators[i]);
}

const std::vector<const FieldManipulator*>& TupleManipulator::field_manipulators() const
{
	return m_fieldManipulators;
}

unsigned int TupleManipulator::size() const
{
	return m_size;
}

//#################### PRIVATE METHODS ####################

void TupleManipulator::initialise(const std::vector<const FieldManipulator*>& fieldManipulators)
{
	if(fieldManipulators.empty())
	{
		throw std::invalid_argument("Tuples must contain at least one field.");
	}

	m_fieldManipulators = fieldManipulators;

	// Calculate the memory offsets of the fields (in bytes) from the start of a target tuple.
	AlignmentTracker alignmentTracker;
	m_fieldOffsets.reserve(m_fieldManipulators.size());
	for(size_t i = 0, size = m_fieldManipulators.size(); i < size; ++i)
	{
		alignmentTracker.advance_to_boundary(m_fieldManipulators[i]->alignment_requirement());
		m_fieldOffsets.push_back(alignmentTracker.offset());
		alignmentTracker.advance(m_fieldManipulators[i]->size());
	}

	// Advance to the next maximum-alignment boundary and store the size of the tuple.
	alignmentTracker.advance_to_boundary(alignmentTracker.max_alignment());
	m_size = alignmentTracker.offset();
}

}
